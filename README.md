
# Fit Buddy - UWB Receiver (DWM3001CDK)

Ce dépôt contient tout le travail réalisé pour la réception UWB dans le cadre du projet **Fit Buddy**, dans lequel j’étais en charge de la partie **capteurs et réseaux**, et plus précisément de la gestion des modules **Qorvo DWM3001CDK** en réception.

## 🎯 Objectifs

- Déclencher la réception UWB à partir d’un signal UART ou Bluetooth simulé.
- Lire et horodater les signaux UWB (TDoA) sur des modules autonomes (balises).
- Transmettre les horodatages à la Raspberry Pi via UART (BLE prévu mais non implémenté).
- Assurer la compatibilité avec Zephyr RTOS et le SDK nRF Connect 2.9.1.
- Adapter la configuration SPI + Devicetree pour le module DW3000 de Qorvo.

## 📁 Structure du projet

```bash
uwb_receiver2/
├── src/
│   └── main.c                  # Code principal (réception, horodatage, UART)
├── modules/
│   └── hal/
│       └── qorvo/              # Driver DW3000 (ajouté manuellement)
├── app.overlay                 # Overlay Devicetree (SPI, GPIO...)
├── prj.conf                   # Configuration Zephyr (SPI, UART, logs...)
├── CMakeLists.txt             # Build du projet avec les fichiers DW3000
├── west.yml                   # Dépendances du projet, incluant hal_qorvo
└── README.md                  # Ce fichier
```
## 🚀 Commandes importantes

### 1. Cloner le dépôt avec les modules nécessaires

bash
west init -m https://github.com/ton-utilisateur/uwb_receiver_fitbuddy.git --mr main
cd uwb_receiver_fitbuddy
west update

### 2. Nettoyer et recompiler le projet
west build -b nrf52840dk_nrf52840 . --pristine=always

### 3. Flasher le firmware sur la carte
west flash

## ⚙️ Fonctionnalités principales

- Initialisation SPI du DWM3001CDK
- Réception d’une trame UART simulée : `"BT"`
- Déclenchement de la fonction `dwt_rxenable()`
- Récupération et affichage de l'horodatage avec `dwt_readrxtimestamp()`
- Envoi de l’horodatage via UART

## 📎 Fichiers Python associés

Deux scripts Python ont été développés pour compléter le projet sur Raspberry Pi :
- **receive_uart.py** : permet de lire les trames envoyées par le module UWB via port série.
- **tdoa_calc.py** : calcule la position en temps réel à partir de plusieurs horodatages (TDoA).

## 🧩 Difficultés rencontrées

- Intégration manuelle du module `hal_qorvo` dans `west.yml`.
- Erreurs fréquentes dans le fichier `Kconfig` et `prj.conf` dues à des symboles non définis.
- Problèmes de compilation à cause de définitions multiples de fonctions DW3000.
- Build interrompu par des erreurs de linkage (`multiple definition of 'decamutexon'`).
- Export vers Raspberry Pi via BLE non finalisé, UART utilisé à la place.

## ✅ Résultat

Un prototype de réception UWB autonome a été développé, testé et validé, répondant aux priorités techniques du cahier des charges (réception, horodatage, envoi des données vers Raspberry Pi).

## 📜 Auteur

Projet réalisé par **Ange Mopiane** dans le cadre du module **Capteurs & Réseaux** du projet Fit Buddy – ESILV 2024-2025.
