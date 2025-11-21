# ATMSIM - ATM Simulator Project

An ESP32-based ATM simulator using NFC card reading and Nokia 5110 LCD display.

## Features

- **NFC Card Reading**: Reads card data from NTAG NFC cards
- **LCD Display**: Nokia 5110 (84x48) LCD for user interface
- **ATM Operations**: Balance check, Withdraw, Deposit
- **Card Data Management**: Reads and writes card data (CardID, Balance, Username, PIN)
- **PIN Authentication**: Secure PIN entry via Serial Monitor
- **Serial Control**: Menu navigation and input via Serial Monitor

## Hardware Requirements

- ESP32 Development Board
- Nokia 5110 LCD Display (PCD8544)
- PN532 NFC Reader Module
- NTAG NFC Cards

## Pin Connections

### Nokia 5110 LCD
- RST → GPIO 25
- CE (CS) → GPIO 26
- DC → GPIO 27
- DIN (MOSI) → GPIO 14
- CLK (SCK) → GPIO 13
- LED → 3.3V
- VCC → 3.3V
- GND → GND

### PN532 NFC Reader
- SDA → GPIO 21
- SCL → GPIO 22
- VCC → 3.3V or 5V
- GND → GND

## Software Requirements

- Arduino IDE with ESP32 board support
- Adafruit_PN532 library (install via Library Manager)

## Installation

1. Clone this repository
2. Open `ATMSIM.ino` in Arduino IDE
3. Install required libraries:
   - Adafruit_PN532 (via Library Manager)
4. Select your ESP32 board in Arduino IDE
5. Upload the code

## Card Data Format

NFC cards should contain data in the following format:
```
CardID,Balance,Username,PIN
```

Example:
```
12345,1000.00,John Doe,1234
```

## Usage

1. Power on the ESP32
2. Open Serial Monitor (115200 baud)
3. Place NFC card on reader
4. Enter PIN when prompted
5. Select menu option (1-4) via Serial Monitor
6. Follow on-screen instructions

## Project Structure

```
ATMSIM/
├── ATMSIM.ino              # Main sketch file
├── APP_Cfg.h                # Configuration and pin definitions
├── src/
│   ├── App/
│   │   └── ATM/
│   │       ├── ATM.h        # ATM application header
│   │       ├── ATM.cpp       # ATM application implementation
│   │       ├── ATM_Display.h # Display functions header
│   │       ├── ATM_Display.cpp # Display functions implementation
│   │       ├── ATM_Controller.h # Controller header
│   │       └── ATM_Controller.cpp # Controller implementation
│   ├── Hal/
│   │   ├── LCD/
│   │   │   ├── LCD.h        # LCD driver header
│   │   │   └── LCD.cpp      # LCD driver implementation
│   │   └── NFC/
│   │       ├── NFC.h        # NFC driver header
│   │       └── NFC.cpp      # NFC driver implementation
└── README.md
```

## License

This project is open source and available for educational purposes.

