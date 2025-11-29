/*
 * ATMSIM - ATM Simulation Project
 * Main Arduino Sketch
 */
 
#include "Hal/WiFi/WiFiModule.h"
#include "App/Notifier/TelegramNotifier.h"
#include "APP_Cfg.h"
#include "src/Hal/LCD/LCD.h"
#include "src/Hal/NFC/NFC.h"
#include "src/Hal/Buzzer/Buzzer.h"
#include "src/Hal/Keypad/KeypadInput.h"
#include "src/App/ATM/ATM.h"
#include "src/App/ATM/ATM_Controller.h"

// Create LCD object with pin definitions
LCD lcd(LCD_RST_PIN, LCD_CE_PIN, LCD_DC_PIN, LCD_DIN_PIN, LCD_CLK_PIN);

// Create NFC object with pin definitions
NFC nfc(NFC_SDA_PIN, NFC_SCL_PIN);

// Create buzzer object
Buzzer buzzer(BUZZER_PIN);

// Create keypad object
KeypadInput keypad;

void setup() {
    
    WiFiModule::begin(WIFI_SSID, WIFI_PASSWORD);
    TelegramNotifier::begin(BOT_TOKEN, CHAT_ID);
    
    // Initialize ATM Controller
    ATM_Controller_Init(&lcd, &nfc, &buzzer, &keypad);
    
    // Run setup
    ATM_Controller_Setup();
}

void loop() {
    // Main loop - all logic is in ATM_Controller
    ATM_Controller_Loop();
}
