# ATMSIM - ATM Simulator Project

An ESP32-based ATM simulator using NFC card reading and Nokia 5110 LCD display.

## Features

- **NFC Card Reading**: Reads card data from NTAG NFC cards
- **LCD Display**: Nokia 5110 (84x48) LCD for user interface
- **ATM Operations**: Balance check, Withdraw, Deposit
- **Card Data Management**: Reads and writes card data (CardID, Balance, Username, PIN)
- **PIN Authentication**: Secure PIN entry via the physical keypad
- **Keypad Control**: Menu navigation and transaction entry using a 4x4 keypad

## Hardware Requirements

- ESP32 Development Board
- Nokia 5110 LCD Display (PCD8544)
- PN532 NFC Reader Module
- NTAG NFC Cards
- 4x4 Matrix Keypad
- Piezo buzzer

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

### Piezo Buzzer
- Signal → GPIO 33
- VCC → 3.3V (through driver/transistor if needed)
- GND → GND

### 4x4 Keypad
- ROW0 → GPIO 16
- ROW1 → GPIO 17
- ROW2 → GPIO 18
- ROW3 → GPIO 19
- COL0 → GPIO 23
- COL1 → GPIO 5
- COL2 → GPIO 4
- COL3 → GPIO 2

## Software Requirements

- Arduino IDE with ESP32 board support
- Adafruit_PN532 library (install via Library Manager)
- Keypad library (by Mark Stanley & Alexander Brevig)

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
2. (Optional) Open Serial Monitor at 115200 baud for debug logs
3. Place NFC card on reader
4. Enter your PIN on the keypad (digits + `A` to confirm, `D` delete, `C` cancel)
5. Use the keypad to select menu options (press the number, then `A`)
6. Enter withdrawal/deposit amounts on the keypad (`*` adds a decimal) and confirm with `A`
7. Follow on-screen instructions and remove the card when prompted

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
│   │   ├── NFC/
│   │   │   ├── NFC.h        # NFC driver header
│   │   │   └── NFC.cpp      # NFC driver implementation
│   │   ├── Buzzer/
│   │   │   ├── Buzzer.h     # Buzzer HAL
│   │   │   └── Buzzer.cpp   # Buzzer HAL implementation
│   │   └── Keypad/
│   │       ├── KeypadInput.h # Keypad helper
│   │       └── KeypadInput.cpp
└── README.md
```

## License

This project is open source and available for educational purposes.

## THIS A Patates from AhmadHelmii