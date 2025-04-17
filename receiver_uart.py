import serial
import csv
import os

# === CONFIGURATION ===
PORT = "COM7"
BAUDRATE = 115200
csv_filename = "uwb_timestamps.csv"

# === Initialisation du fichier CSV ===
if os.path.exists(csv_filename):
    os.remove(csv_filename)

with open(csv_filename, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["receiver_id", "timestamp"])

# === Ouverture du port série ===
ser = serial.Serial(PORT, BAUDRATE, timeout=1)
print(f"Lecture UART sur {PORT} à {BAUDRATE} bauds...")

# === Boucle principale ===
while True:
    try:
        line = ser.readline().decode('utf-8').strip()
        if line:
            print("Reçu :", line)

            if line.startswith("<UWB_TS:") and line.endswith(">"):
                try:
                    data = line.strip("<>").split(";")
                    timestamp_str = data[0].split(":")[1]
                    receiver_id_str = data[1].split(":")[1]

                    timestamp = int(timestamp_str)
                    receiver_id = int(receiver_id_str)

                    # Enregistrement dans le CSV
                    with open(csv_filename, mode='a', newline='') as file:
                        writer = csv.writer(file)
                        writer.writerow([receiver_id, timestamp])

                    print(f"Enregistré dans CSV : ID={receiver_id}, TS={timestamp}")

                except Exception as e:
                    print("Erreur de parsing :", e)

    except KeyboardInterrupt:
        print("\nArrêt par l'utilisateur.")
        break
