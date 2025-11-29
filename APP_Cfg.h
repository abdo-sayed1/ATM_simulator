

#define WIFI_SSID      "YourWiFi"
#define WIFI_PASSWORD  "YourPassword"

#define BOT_TOKEN      "123456789:ABCDEF..."
#define CHAT_ID        "987654321"

/*
 * APP_Cfg.h - Application Configuration Header
 * Configuration parameters for the application layer
 */

#ifndef APP_CFG_H
#define APP_CFG_H

/* Nokia 5110 LCD Pin Definitions */
#define LCD_RST_PIN  25  // Reset pin
#define LCD_CE_PIN   26  // Chip Enable (CS)
#define LCD_DC_PIN   27  // Data/Command
#define LCD_DIN_PIN  14  // Data Input (MOSI)
#define LCD_CLK_PIN  13  // Clock (SCK)

/* NFC Reader Pin Definitions */
#define NFC_SDA_PIN  21  // I2C Data (SDA)
#define NFC_SCL_PIN  22  // I2C Clock (SCL)

/* Buzzer Pin */
#define BUZZER_PIN   33  // Piezo buzzer output

/* Keypad Definitions */
#define KEYPAD_ROWS      4
#define KEYPAD_COLS      4
#define KEYPAD_ROW0_PIN  4
#define KEYPAD_ROW1_PIN  16
#define KEYPAD_ROW2_PIN  17
#define KEYPAD_ROW3_PIN  5
#define KEYPAD_COL0_PIN  18
#define KEYPAD_COL1_PIN  19
#define KEYPAD_COL2_PIN  23
#define KEYPAD_COL3_PIN  32

#endif /* APP_CFG_H */

