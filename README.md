# ATM Simulator Project (ESP32-Based)

## 📋 Overview
This is an ATM (Automated Teller Machine) simulator built using ESP32 WROOM microcontroller. The project simulates a real ATM machine with NFC card reading, PIN authentication, account management, and transaction processing capabilities. The ESP32 provides enhanced features like WiFi connectivity, more memory, and faster processing compared to traditional Arduino boards.

## 🎯 Features

### Core Features
- ✅ **NFC Card Authentication** - Read and authenticate NFC/RFID cards
- ✅ **New Account Creation** - First-time card registration with account setup
- ✅ **PIN Management** - Secure 4-digit PIN entry and validation
- ✅ **Balance Inquiry** - Check current account balance
- ✅ **Cash Withdrawal** - Withdraw money from account
- ✅ **Cash Deposit** - Deposit money into account
- ✅ **Transaction History** - View recent transactions
- ✅ **Audio Feedback** - Buzzer sounds for user interactions
- ✅ **Persistent Storage** - Account data saved to SPIFFS/NVS (Non-Volatile Storage)
- ✅ **WiFi Connectivity** - Optional cloud sync and remote monitoring
- ✅ **Web Dashboard** - Monitor transactions via web interface (optional)
- ✅ **OTA Updates** - Over-the-Air firmware updates

### Account Creation Flow
1. Insert new NFC card
2. System detects it's a new card
3. User creates account with:
   - Account name entry
   - PIN creation (4 digits)
   - PIN confirmation
   - Initial deposit (optional)
4. Account saved to NVS (Non-Volatile Storage)
5. User can immediately start using the ATM

## 🛠️ Hardware Requirements

| Component | Specification | Quantity |
|-----------|--------------|----------|
| **Microcontroller** | ESP32 WROOM (DevKit V1) | 1 |
| **NFC Module** | PN532 NFC RFID Reader (V3) | 1 |
| **NFC Cards** | MIFARE Classic 1K or compatible | Multiple |
| **Keypad** | 4×4 Matrix Keypad | 1 |
| **Display** | Nokia 5110 LCD (84×48 pixels) | 1 |
| **Buzzer** | Active or Passive Buzzer | 1 |
| **Breadboard** | Standard size | 1 |
| **Jumper Wires** | Male-to-Male, Male-to-Female | Multiple |
| **Power Supply** | 5V DC via USB | 1 |

### ESP32 WROOM DevKit V1

<img src="docs/images/esp32-wroom-pinout.png" alt="ESP32 WROOM Pinout" width="600">

**Note:** ESP32 operates at 3.3V logic level. Ensure all components are 3.3V compatible or use level shifters.

## 🔌 Pin Connections (ESP32 WROOM)

### Complete Wiring Diagram

<img src="docs/images/wiring-diagram.png" alt="Complete Wiring Diagram" width="800">

### Nokia 5110 LCD (SPI)
```
LCD Pin    →  ESP32 Pin    →  Physical Pin
CLK (SCLK) →  GPIO 18      →  D18
DIN (MOSI) →  GPIO 23      →  D23
DC (D/C)   →  GPIO 16      →  D16
CE (CS)    →  GPIO 5       →  D5
RST        →  GPIO 17      →  D17
VCC        →  3.3V         →  3V3
BL (LED)   →  3.3V         →  3V3 (with 330Ω resistor)
GND        →  GND          →  GND
```

### PN532 NFC Module (I2C Mode)
```
NFC Pin  →  ESP32 Pin    →  Physical Pin
VCC      →  3.3V         →  3V3
GND      →  GND          →  GND
SDA      →  GPIO 21      →  D21
SCL      →  GPIO 22      →  D22
```

**Important:** Set PN532 to I2C mode using onboard switches (SW1: ON, SW2: OFF)

### 4×4 Keypad
```
Keypad Pin  →  ESP32 Pin    →  Physical Pin
ROW1        →  GPIO 13      →  D13
ROW2        →  GPIO 12      →  D12
ROW3        →  GPIO 14      →  D14
ROW4        →  GPIO 27      →  D27
COL1        →  GPIO 26      →  D26
COL2        →  GPIO 25      →  D25
COL3        →  GPIO 33      →  D33
COL4        →  GPIO 32      →  D32
```

**Keypad Layout:**
```
[1] [2] [3] [A]
[4] [5] [6] [B]
[7] [8] [9] [C]
[*] [0] [#] [D]
```

### Buzzer
```
Buzzer Pin  →  ESP32 Pin    →  Physical Pin
Positive    →  GPIO 4       →  D4
Negative    →  GND          →  GND
```

### Connection Summary Table
| ESP32 Pin | Component | Function |
|-----------|-----------|----------|
| GPIO 4 | Buzzer | Output |
| GPIO 5 | Nokia 5110 | CE (Chip Enable) |
| GPIO 12-14 | Keypad | Rows 2, 3, 4 |
| GPIO 13 | Keypad | Row 1 |
| GPIO 16 | Nokia 5110 | DC (Data/Command) |
| GPIO 17 | Nokia 5110 | RST (Reset) |
| GPIO 18 | Nokia 5110 | CLK (SPI Clock) |
| GPIO 21 | PN532 | SDA (I2C Data) |
| GPIO 22 | PN532 | SCL (I2C Clock) |
| GPIO 23 | Nokia 5110 | DIN (SPI Data) |
| GPIO 25-26 | Keypad | Columns 2, 1 |
| GPIO 27 | Keypad | Row 4 |
| GPIO 32-33 | Keypad | Columns 4, 3 |
| 3.3V | All Components | Power Supply |
| GND | All Components | Ground |

## 📁 Project Structure

```
ATM_Simulator/
├── src/
│   ├── app/                    # Application layer
│   │   ├── atm_app.h/cpp      # Main ATM application
│   │   ├── states/            # State machine implementations
│   │   │   ├── idle_state
│   │   │   ├── card_read_state
│   │   │   ├── card_detection_state
│   │   │   ├── account_creation_state
│   │   │   ├── pin_creation_state
│   │   │   ├── pin_entry_state
│   │   │   ├── menu_state
│   │   │   └── transaction_state
│   │   └── ui/                # User interface
│   │       ├── screens
│   │       └── menu_manager
│   │
│   ├── hal/                    # Hardware Abstraction Layer
│   │   ├── nfc/               # PN532 NFC driver
│   │   ├── keypad/            # 4×4 keypad driver
│   │   ├── display/           # Nokia 5110 driver
│   │   └── audio/             # Buzzer driver
│   │
│   ├── mcal/                   # Microcontroller Abstraction Layer
│   │   ├── gpio/              # GPIO control
│   │   ├── spi/               # SPI communication
│   │   ├── i2c/               # I2C communication
│   │   ├── timer/             # Timer functions
│   │   └── nvs/               # NVS storage (ESP32)
│   │
│   └── services/               # Business logic services
│       ├── authentication/     # User authentication
│       ├── database/          # Account management
│       ├── validation/        # Input validation
│       ├── transaction/       # Transaction processing
│       └── logger/            # Event logging
│
├── inc/                        # Header files
│   ├── config.h               # Configuration settings
│   ├── types.h                # Data structures
│   └── pin_config.h           # Pin definitions
│
├── lib/                        # External libraries
│   └── PN532/                 # PN532 library (if needed)
│
├── test/                       # Unit tests
│   ├── test_keypad.cpp
│   ├── test_nfc.cpp
│   └── test_display.cpp
│
├── docs/                       # Documentation
│   ├── hardware_connections.md
│   ├── state_diagram.md
│   └── api_reference.md
│
├── ATM_Simulator.ino          # Main Arduino sketch
└── README.md                  # This file
```

## 🚀 Getting Started

### Installation

1. **Install Arduino IDE**
   ```
   Download from: https://www.arduino.cc/en/software
   ```

2. **Add ESP32 Board Support**
   - Open Arduino IDE
   - Go to: File → Preferences
   - Add to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
   - Go to: Tools → Board → Boards Manager
   - Search "ESP32" and install "esp32 by Espressif Systems"

3. **Install Required Libraries**
   - Go to: Sketch → Include Library → Manage Libraries
   - Search and install:
     - `Adafruit PN532`
     - `Adafruit GFX Library`
     - `Adafruit PCD8544 Nokia 5110 LCD library`
     - `Keypad by Mark Stanley`
     - `ArduinoJson` (for web features)
     - `ESPAsyncWebServer` (optional, for web dashboard)

4. **Select ESP32 Board**
   - Go to: Tools → Board → ESP32 Arduino
   - Select: "ESP32 Dev Module" or "DOIT ESP32 DEVKIT V1"

5. **Configure Board Settings**
   ```
   Board: "ESP32 Dev Module"
   Upload Speed: "921600"
   CPU Frequency: "240MHz (WiFi/BT)"
   Flash Frequency: "80MHz"
   Flash Mode: "QIO"
   Flash Size: "4MB (32Mb)"
   Partition Scheme: "Default 4MB with spiffs"
   Core Debug Level: "None"
   PSRAM: "Disabled"
   ```

6. **Clone/Download Project**
   ```bash
   git clone https://github.com/yourusername/ATM_Simulator.git
   cd ATM_Simulator
   ```

7. **Open Project**
   - Open `ATM_Simulator.ino` in Arduino IDE

8. **Configure Hardware**
   - Wire components according to pin connections above
   - Verify pin definitions in `inc/pin_config.h`

9. **Upload Code**
   - Connect ESP32 via USB
   - Select COM port: Tools → Port
   - Click Upload button
   - Hold BOOT button if upload fails

### Installation - PlatformIO (Recommended)

1. **Install VS Code**
   ```
   Download from: https://code.visualstudio.com/
   ```

2. **Install PlatformIO Extension**
   - Open VS Code
   - Go to Extensions (Ctrl+Shift+X)
   - Search "PlatformIO IDE"
   - Click Install

3. **Create Project**
   ```bash
   git clone https://github.com/yourusername/ATM_Simulator.git
   cd ATM_Simulator
   code .
   ```

4. **Configure platformio.ini**
   ```ini
   [env:esp32dev]
   platform = espressif32
   board = esp32dev
   framework = arduino
   monitor_speed = 115200
   upload_speed = 921600
   
   lib_deps = 
       adafruit/Adafruit PN532@^1.3.1
       adafruit/Adafruit GFX Library@^1.11.3
       adafruit/Adafruit PCD8544 Nokia 5110 LCD library@^2.0.2
       chris--a/Keypad@^3.1.1
       bblanchon/ArduinoJson@^6.21.3
       me-no-dev/ESPAsyncWebServer@^1.2.3
   
   build_flags = 
       -DCORE_DEBUG_LEVEL=0
       -DBOARD_HAS_PSRAM
   ```

5. **Build and Upload**
   - Click PlatformIO icon in sidebar
   - Click "Upload" under PROJECT TASKS
   - Or use: `pio run -t upload`

## 💻 Usage Guide

### First Time Setup
1. Power on the ATM system via USB or battery
2. Wait for "Insert Card" message on display
3. System initializes WiFi (if configured)
4. Place NFC card on reader

### Creating New Account
1. System displays: "New Card! Create Account?"
2. Press `1` to create account, `2` to cancel
3. **Enter Name:**
   - Use keypad numbers (T9-style text entry)
   - Press `#` to confirm
   - Press `*` to backspace
4. **Set PIN:**
   - Enter 4-digit PIN
   - Press `#` to confirm
5. **Confirm PIN:**
   - Re-enter same PIN
   - Press `#` to confirm
6. **Initial Deposit (Optional):**
   - Enter amount or `0` for no deposit
   - Press `#` to confirm
7. **Confirm Creation:**
   - Press `1` to create account
   - Press `2` to cancel
8. Account saved to NVS (survives power loss)

### Using Existing Account
1. **Card Authentication:**
   - Place card on reader
   - System recognizes existing card
2. **PIN Entry:**
   - Enter 4-digit PIN
   - Press `#` to confirm
3. **Main Menu:**
   - `1` - Balance Inquiry
   - `2` - Withdraw Cash
   - `3` - Deposit Cash
   - `4` - Transaction History
   - `5` - Change PIN
   - `#` - Exit

## 🔧 Configuration

### Edit `inc/config.h` for customization:

```cpp
// Account Settings
#define MAX_ACCOUNTS 50
#define PIN_LENGTH 4
#define MAX_NAME_LENGTH 20

// Transaction Limits
#define MIN_WITHDRAWAL 10.0
#define MAX_WITHDRAWAL 1000.0
#define MAX_DAILY_WITHDRAWAL 2000.0

// Initial Deposit
#define ALLOW_ZERO_INITIAL_DEPOSIT true
#define MIN_INITIAL_DEPOSIT 0.0
#define MAX_INITIAL_DEPOSIT 10000.0

// Security
#define MAX_PIN_ATTEMPTS 3
#define PIN_TIMEOUT 30000            // 30 seconds
#define CARD_TIMEOUT 60000           // 60 seconds

// Display
#define SCREEN_REFRESH_RATE 100
#define LCD_CONTRAST 60              // 0-127
#define BACKLIGHT_TIMEOUT 60000      // 60 seconds
```

## 📊 State Machine Diagram

```
[IDLE]
  ↓ Card Detected
[CARD_READ] ← Read NFC UID
  ↓
[CARD_DETECTION] ← Check if card exists?
  ↓                           ↓
  YES                         NO
  ↓                           ↓
[PIN_ENTRY]              [ACCOUNT_CREATION]
  ↓                           ↓
  Correct?              [PIN_CREATION]
  ↓                           ↓
[MENU]                   [PIN_CONFIRMATION]
  ↓                           ↓
  1,2,3,4,5,6            [INITIAL_DEPOSIT]
  ↓                           ↓
[BALANCE]               [CONFIRM_CREATION]
[WITHDRAW]                    ↓
[DEPOSIT]                Account saved to NVS
[HISTORY]                     ↓
[CHANGE_PIN]            [MENU] (Account Created)
[WIFI_SETTINGS]
  ↓
[IDLE] ← Card Removed or Exit
```

## 🔐 Security Features

- **PIN Protection:** 4-digit PIN required for all transactions
- **PIN Attempts Limit:** Lock account after 3 failed attempts
- **Card UID Verification:** Each card uniquely identified
- **Timeout Protection:** Auto-logout after inactivity
- **Data Storage:** Secure persistent data storage in EEPROM/NVS
- **PIN Masking:** PIN displayed as asterisks during entry

## 📡 Future Features (ESP32 Capabilities)

- [ ] WiFi connectivity for remote monitoring
- [ ] Web dashboard for transaction viewing
- [ ] Cloud data synchronization
- [ ] OTA (Over-The-Air) firmware updates
- [ ] Bluetooth mobile app integration
- [ ] Multi-language support
- [ ] Voice feedback
- [ ] Enhanced security with encryption

## 🐛 Troubleshooting

### ESP32 Issues

#### ESP32 Won't Upload
- Hold BOOT button while clicking Upload
- Press RESET button after upload starts
- Check USB cable (must be data cable)
- Try different USB port

#### Display Issues
- Verify all pin connections
- Check 3.3V power supply
- Adjust LCD_CONTRAST in config.h
- Test with simple SPI example

#### NFC Not Reading
- Verify I2C connections (GPIO 21, 22)
- Set PN532 switches: SW1=ON, SW2=OFF
- Move card closer to reader
- Test with I2C scanner sketch

#### Keypad Not Working
- Check all 8 pin connections
- Verify pin numbers in pin_config.h
- Test with keypad example code
- Check for loose wires

#### Buzzer Silent
- Verify GPIO 4 connection
- Check buzzer polarity
- Test with simple tone code
- Try different GPIO pin

## 📚 Libraries Used

### Core Libraries (Built-in)
- **WiFi.h** - ESP32 WiFi support
- **Wire.h** - I2C communication
- **SPI.h** - SPI communication
- **Preferences.h** - ESP32 NVS storage

### External Libraries
- **Adafruit_PN532** - NFC/RFID communication
- **Adafruit_GFX** - Graphics primitives
- **Adafruit_PCD8544** - Nokia 5110 display driver
- **Keypad** - Matrix keypad handling

## 🎓 Learning Resources

### ESP32 & Components
- [ESP32 Official Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [ESP32 Pinout Reference](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [PN532 User Manual](https://www.nxp.com/docs/en/user-guide/141520.pdf)
- [Nokia 5110 Datasheet](https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf)

## ⚠️ Limitations

- Maximum 50 accounts (configurable, limited by storage)
- No real money handling (simulation only)
- Single-user operation (no concurrent access)
- Limited display size (84×48 pixels)
- Basic security implementation (educational project)

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 Future Enhancements

- [ ] Multiple account types (Savings, Checking)
- [ ] Transfer between accounts
- [ ] Receipt printing simulation
- [ ] Admin mode for system management
- [ ] WiFi connectivity and cloud sync
- [ ] Transaction encryption
- [ ] Biometric authentication
- [ ] Mobile app integration
- [ ] Multi-language support

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👨‍💻 Author

**Abdo Sayed**
- GitHub: [@abdo-sayed1](https://github.com/abdo-sayed1)
- Email: abdo_siyed1@gmail.com

## 🙏 Acknowledgments

- Espressif Systems for ESP32 platform
- Adafruit for excellent libraries
- Arduino and PlatformIO communities
- NXP Semiconductors for PN532 documentation
- All contributors and testers

## 📞 Support

For issues, questions, or suggestions:
- Open an issue on [GitHub](https://github.com/abdo-sayed1/ATM_Simulator/issues)
- Email: abdo_siyed1@gmail.com
- Project Repository: [ATM_Simulator](https://github.com/abdo-sayed1/ATM_Simulator)

---
