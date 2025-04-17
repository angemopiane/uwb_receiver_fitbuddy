#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "deca_device_api.h"
#include "dw3000_deca_regs.h"

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)
#define MAX_STORED_TSTAMPS 20
#define RECEIVER_ID 1
#define SYNC_TIMEOUT_MS 60000

static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
K_THREAD_STACK_DEFINE(uart_stack, 1024);
struct k_thread uart_thread;
K_SEM_DEFINE(bt_cmd_sem, 0, 1);

// Buffer de timestamps
static uint64_t timestamps[MAX_STORED_TSTAMPS];
static int ts_index = 0;
static uint64_t synced_time_ref = 0;
static int64_t last_sync_time = 0;

void store_timestamp(uint64_t ts_val) {
    timestamps[ts_index] = ts_val;
    ts_index = (ts_index + 1) % MAX_STORED_TSTAMPS;
}

void uart_thread_fn(void *a, void *b, void *c) {
    uint8_t bt_cmd[2] = {0};
    size_t idx = 0;
    static char sync_buf[32];
    static int sync_idx = 0;

    while (1) {
        uint8_t ch;
        if (uart_poll_in(uart_dev, &ch) == 0) {
            if (ch == 'B' || ch == 'T') {
                bt_cmd[idx++] = ch;
                if (idx == 2 && bt_cmd[0] == 'B' && bt_cmd[1] == 'T') {
                    printk("\n[UART] Commande BT reçue. Passage en réception UWB...\n");
                    k_sem_give(&bt_cmd_sem);
                    idx = 0;
                }
            } else {
                idx = 0;
            }

            if (ch == 'S') {
                sync_buf[sync_idx++] = ch;
            } else if (sync_idx > 0) {
                sync_buf[sync_idx++] = ch;
                if (sync_idx >= 5 && strncmp(sync_buf, "SYNC:", 5) == 0) {
                    if (ch == '\n' || ch == '\r') {
                        sync_buf[sync_idx] = '\0';
                        uint64_t ts = strtoull(sync_buf + 5, NULL, 10);
                        synced_time_ref = ts;
                        last_sync_time = k_uptime_get();
                        printk("[SYNC] Réception synchronisation horloge : %llu\n", synced_time_ref);
                        sync_idx = 0;
                    }
                } else if (sync_idx >= sizeof(sync_buf)) {
                    sync_idx = 0;
                }
            }
        }
        k_msleep(10);
    }
}

int main(void)
{
    printk("\n[BOOT] Initialisation...\n");

    if (!device_is_ready(uart_dev)) {
        printk("[ERREUR] UART non prêt\n");
        return 1;
    }

    dwt_softreset(0);
    if (dwt_initialise(DWT_DW_INIT) != DWT_SUCCESS) {
        printk("[ERREUR] Échec init DW3000\n");
        return 1;
    }

    printk("[OK] DW3000 initialisé\n");

    dwt_setchannel(5);
    dwt_configuretxrf(NULL);
    dwt_enableautoack(0, 0);
    dwt_setrxtimeout(0);

    k_thread_create(&uart_thread, uart_stack, K_THREAD_STACK_SIZEOF(uart_stack),
                    uart_thread_fn, NULL, NULL, NULL, 5, 0, K_NO_WAIT);

    uint8_t rx_buffer[127];
    uint8_t ts[5];
    int msg_count = 0;

    printk("[READY] Attente d’une commande BT...\n");

    while (1) {
        k_sem_take(&bt_cmd_sem, K_FOREVER);
        dwt_rxenable(DWT_START_RX_IMMEDIATE);

        int timeout = 500;
        while (timeout-- > 0) {
            if (dwt_checkirq() && dwt_readrdbstatus() & DWT_RDB_STATUS_RXFCG0_BIT_MASK) {
                dwt_readrxdata(rx_buffer, sizeof(rx_buffer), 0);
                dwt_readrxtimestamp(ts, 0);

                uint64_t ts_val = 0;
                for (int i = 0; i < 5; i++) {
                    ts_val |= ((uint64_t)ts[i]) << (i * 8);
                }

                if (synced_time_ref > 0 && (k_uptime_get() - last_sync_time > SYNC_TIMEOUT_MS)) {
                    printk("[SYNC] Timeout - horloge désynchronisée\n");
                    synced_time_ref = 0;
                }

                uint64_t ts_relative = (synced_time_ref > 0) ? ts_val - synced_time_ref : ts_val;

                store_timestamp(ts_val);
                msg_count++;

                printk("\n[%d] UWB reçu : %s\n", msg_count, rx_buffer);
                printk("[TDoA] Timestamp : %02X%02X%02X%02X%02X | %llu\n",
                       ts[4], ts[3], ts[2], ts[1], ts[0], ts_relative);

                char msg[80];
                snprintf(msg, sizeof(msg), "<UWB_TS:%llu;ID:%02d>\n", ts_relative, RECEIVER_ID);

                for (int i = 0; msg[i] != '\0'; i++) {
                    uart_poll_out(uart_dev, msg[i]);
                }

                dwt_writesysstatuslo(DWT_INT_RXFCG_BIT_MASK);

                if (msg_count % 10 == 0) {
                    printk("[INFO] %d messages traités.\n", msg_count);
                }

                break;
            }
            k_msleep(10);
        }

        if (timeout <= 0) {
            printk("[UWB] Timeout réception\n");
        }
    }
}
