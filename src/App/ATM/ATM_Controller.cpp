/*
 * ATM_Controller.cpp - ATM Controller Implementation
 * Main ATM logic and state management
 */

#include "ATM_Controller.h"
#include "ATM_Display.h"
#include "APP_Cfg.h"
#include <string.h>

/* Private Variables */
static LCD* _lcd = NULL;
static NFC* _nfc = NULL;

// Card reading state
static bool cardPresent = false;
static bool cardDataRead = false;
static unsigned long lastCardCheck = 0;
static const unsigned long CARD_CHECK_INTERVAL = 500;  // Check every 500ms

// Menu state
static int currentMenuOption = 1;  // Current menu selection (1-4)
static int lastDisplayedOption = 0;  // Track menu display state

// PIN entry state
static String enteredPIN = "";
static bool pinEntryActive = false;

/* Private Function Prototypes */
static void handleSerialInput(void);
static void handleCardDetection(void);
static void handleATMStates(void);
static void processWithdraw(float amount);
static void processDeposit(float amount);
static void updateCardData(void);

void ATM_Controller_Init(LCD* lcd, NFC* nfc) {
    _lcd = lcd;
    _nfc = nfc;
    ATM_Display_Init(lcd);
}

void ATM_Controller_Setup(void) {
    // Initialize Serial communication
    Serial.begin(115200);
    delay(1000);
    
    // Initialize LCD display
    if (_lcd != NULL) {
        _lcd->begin();
        _lcd->setContrast(0x3F);  // Set contrast (adjust as needed)
        _lcd->clear();
        
        // Display welcome message
        _lcd->setCursor(0, 0);
        _lcd->print("ATMSIM");
        _lcd->setCursor(0, 1);
        _lcd->print("Init...");
    }
    
    Serial.println("ATMSIM initialized");
    Serial.println("Nokia 5110 LCD initialized");
    
    // Initialize NFC reader
    delay(500);
    Serial.println("Initializing NFC Reader...");
    Serial.print("I2C SDA: ");
    Serial.println(NFC_SDA_PIN);
    Serial.print("I2C SCL: ");
    Serial.println(NFC_SCL_PIN);
    
    if (_nfc != NULL && _nfc->begin()) {
        ATM_Display_NFCReady();
        Serial.println("NFC Reader initialized successfully!");
    } else {
        ATM_Display_NFCError();
        Serial.println("NFC Reader initialization failed!");
        Serial.println("Please check:");
        Serial.println("1. I2C connections (SDA=21, SCL=22)");
        Serial.println("2. Power supply to NFC module");
        Serial.println("3. Install Adafruit_PN532 library");
        delay(3000);
    }
    
    // Initialize ATM application
    ATM_Init();
    
    // Display welcome screen
    ATM_Display_InitScreen();
    
    delay(1000);
}

void ATM_Controller_Loop(void) {
    ATM_Process();
    
    // Handle Serial input
    handleSerialInput();
    
    // Check for card periodically
    handleCardDetection();
    
    // Handle ATM states
    handleATMStates();
}

static void handleSerialInput(void) {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        
        ATM_State_t atmState = ATM_GetState();
        
        if (atmState == ATM_STATE_PIN_ENTRY && pinEntryActive) {
            // PIN entry
            enteredPIN = input;
            pinEntryActive = false;
            
            CardData_t* card = ATM_GetCardData();
            
            // Validate PIN
            if (enteredPIN.equals(card->pinCode)) {
                Serial.println("PIN Correct!");
                ATM_Display_PINCorrect();
                delay(2000);
                
                // Show menu
                ATM_SetState(ATM_STATE_MENU);
                currentMenuOption = 1;
                lastDisplayedOption = 0;
                ATM_Display_Menu();
                
                // Serial only for input
                Serial.println("\n=== Enter Menu Choice (1-4) ===");
                Serial.print("Choice: ");
            } else {
                Serial.println("PIN Incorrect! Access Denied.");
                ATM_Display_PINIncorrect();
                delay(2000);
                
                // Reset and wait for card removal
                ATM_Reset();
                enteredPIN = "";
                pinEntryActive = false;
                
                ATM_Display_ScanCard();
            }
        } else if (atmState == ATM_STATE_MENU) {
            // Menu navigation
            int choice = input.toInt();
            
            if (choice >= 1 && choice <= 4) {
                currentMenuOption = choice;
                
                switch (currentMenuOption) {
                    case 1: {  // Balance
                        ATM_SetState(ATM_STATE_BALANCE);
                        lastDisplayedOption = 0;  // Reset menu display
                        ATM_Display_Balance();
                        Serial.print("Current Balance: $");
                        CardData_t* card = ATM_GetCardData();
                        Serial.println(card->balance);
                        Serial.println("\nPress Enter to return to menu...");
                        break;
                    }
                        
                    case 2: {  // Withdraw
                        ATM_SetState(ATM_STATE_WITHDRAW);
                        lastDisplayedOption = 0;  // Reset menu display
                        ATM_Display_WithdrawPrompt();
                        
                        Serial.print("Enter amount to withdraw: $");
                        // Wait for amount input
                        while (!Serial.available()) {
                            delay(10);
                        }
                        String amountStr = Serial.readStringUntil('\n');
                        amountStr.trim();
                        float withdrawAmount = amountStr.toFloat();
                        
                        ATM_Display_Processing();
                        processWithdraw(withdrawAmount);
                        break;
                    }
                        
                    case 3: {  // Deposit
                        ATM_SetState(ATM_STATE_DEPOSIT);
                        lastDisplayedOption = 0;  // Reset menu display
                        ATM_Display_DepositPrompt();
                        
                        Serial.print("Enter amount to deposit: $");
                        // Wait for amount input
                        while (!Serial.available()) {
                            delay(10);
                        }
                        String depositStr = Serial.readStringUntil('\n');
                        depositStr.trim();
                        float depositAmount = depositStr.toFloat();
                        
                        ATM_Display_Processing();
                        processDeposit(depositAmount);
                        break;
                    }
                        
                    case 4: {  // Exit
                        ATM_Reset();
                        ATM_SetState(ATM_STATE_IDLE);  // Set state to IDLE to prevent menu from showing
                        lastDisplayedOption = 0;
                        cardDataRead = false;  // Reset card data read flag
                        enteredPIN = "";  // Clear PIN
                        pinEntryActive = false;  // Reset PIN entry
                        
                        ATM_Display_ThankYou();
                        Serial.println("Thank you for using ATM!");
                        delay(2000);
                        
                        // Show "Take your card" message
                        ATM_Display_TakeCard();
                        delay(3000);
                        
                        // Return to scan card screen
                        ATM_Display_ScanCard();
                        break;
                    }
                }
            } else {
                Serial.println("Invalid choice! Please enter 1-4");
                Serial.print("Choice: ");
            }
        } else if (atmState == ATM_STATE_BALANCE || atmState == ATM_STATE_TRANSACTION_COMPLETE) {
            // Return to menu on any input
            ATM_SetState(ATM_STATE_MENU);
            lastDisplayedOption = 0;
            ATM_Display_Menu();
            Serial.println("\n=== Enter Menu Choice (1-4) ===");
            Serial.print("Choice: ");
        }
    }
}

static void handleCardDetection(void) {
    if (_nfc == NULL) return;
    
    // Check for card periodically
    if (millis() - lastCardCheck >= CARD_CHECK_INTERVAL) {
        lastCardCheck = millis();
        
        if (_nfc->isCardPresent()) {
            if (!cardPresent) {
                // New card detected
                cardPresent = true;
                cardDataRead = false;
                
                // Get card UID
                uint8_t uid[NFC_MAX_UID_LENGTH];
                uint8_t uidLength = 0;
                
                if (_nfc->readCard(uid, &uidLength)) {
                    // Get UID as string
                    char uidString[32];
                    _nfc->getCardUIDString(uidString, sizeof(uidString));
                    
                    // Print to Serial
                    Serial.print("Card UID: ");
                    Serial.println(uidString);
                    Serial.print("Card Type: ");
                    uint8_t cardType = _nfc->getCardType();
                    if (cardType == NFC_CARD_TYPE_MIFARE) {
                        Serial.println("MIFARE");
                    } else if (cardType == NFC_CARD_TYPE_NTAG) {
                        Serial.println("NTAG");
                    } else {
                        Serial.println("Unknown");
                    }
                    
                    // Try to read NTAG data if it's an NTAG card
                    if (cardType == NFC_CARD_TYPE_NTAG && !cardDataRead) {
                        char cardData[128];
                        if (_nfc->readNTAGData(cardData, sizeof(cardData))) {
                            Serial.print("Card Data: ");
                            Serial.println(cardData);
                            
                            // Parse and load card data into ATM
                            if (ATM_ReadCardData(cardData)) {
                                cardDataRead = true;
                                CardData_t* card = ATM_GetCardData();
                                
                                Serial.println("====== Card Info ======");
                                Serial.print("Card ID: ");
                                Serial.println(card->cardID);
                                Serial.print("Balance: $");
                                Serial.println(card->balance);
                                Serial.print("Username: ");
                                Serial.println(card->username);
                                Serial.print("PIN: ");
                                Serial.println(card->pinCode);
                                Serial.println("=======================");
                                
                                // Display welcome message
                                ATM_Display_Welcome(card->username);
                                delay(2000);
                                
                                // Request PIN entry
                                ATM_SetState(ATM_STATE_PIN_ENTRY);
                                enteredPIN = "";
                                pinEntryActive = true;
                                
                                ATM_Display_PINEntry();
                                
                                Serial.println("\n=== PIN Entry Required ===");
                                Serial.println("Please enter your PIN via Serial Monitor and press Enter");
                                Serial.print("PIN: ");
                            }
                        }
                    }
                }
            }
        } else {
            if (cardPresent) {
                // Card removed
                cardPresent = false;
                cardDataRead = false;
                _nfc->reset();
                ATM_Reset();
                
                ATM_Display_ScanCard();
                
                Serial.println("Card removed");
            }
        }
    }
}

static void handleATMStates(void) {
    ATM_State_t atmState = ATM_GetState();
    
    // Handle ATM menu and operations
    if (atmState == ATM_STATE_MENU) {
        // Initialize menu when first entering menu state
        if (lastDisplayedOption == 0) {
            currentMenuOption = 1;
            ATM_Display_Menu();
            lastDisplayedOption = 1;  // Set to non-zero to indicate menu is displayed
        }
        // Menu stays on screen, no need to redraw constantly
    } else {
        // Reset menu state when leaving menu
        lastDisplayedOption = 0;
    }
    
    if (atmState == ATM_STATE_PIN_ENTRY) {
        // PIN entry is handled via Serial input
        // Nothing to do here
    } else if (atmState == ATM_STATE_BALANCE) {
        // Display balance - only once when entering this state
        static ATM_State_t lastState = ATM_STATE_IDLE;
        if (lastState != ATM_STATE_BALANCE) {
            ATM_Display_Balance();
        }
        lastState = atmState;
    } else if (atmState == ATM_STATE_TRANSACTION_COMPLETE) {
        ATM_Display_TransactionComplete();
    }
}

static void processWithdraw(float amount) {
    if (amount > 0) {
        CardData_t* card = ATM_GetCardData();
        if (amount <= card->balance) {
            ATM_ProcessWithdraw(amount);
            ATM_Display_TransactionComplete();
            Serial.print("Withdrawn: $");
            Serial.println(amount);
            delay(2000);
            ATM_Display_Balance();
            Serial.print("New Balance: $");
            Serial.println(card->balance);
            
            // Write updated balance back to card
            updateCardData();
        } else {
            Serial.println("Insufficient funds!");
            ATM_Display_InsufficientFunds();
            delay(2000);
        }
    }
    Serial.println("\nPress Enter to return to menu...");
    ATM_SetState(ATM_STATE_MENU);
    lastDisplayedOption = 0;
    ATM_Display_Menu();
    Serial.println("\n=== Enter Menu Choice (1-4) ===");
    Serial.print("Choice: ");
}

static void processDeposit(float amount) {
    if (amount > 0) {
        ATM_ProcessDeposit(amount);
        ATM_Display_TransactionComplete();
        Serial.print("Deposited: $");
        Serial.println(amount);
        delay(2000);
        ATM_Display_Balance();
        CardData_t* card = ATM_GetCardData();
        Serial.print("New Balance: $");
        Serial.println(card->balance);
        
        // Write updated balance back to card
        updateCardData();
    }
    Serial.println("\nPress Enter to return to menu...");
    ATM_SetState(ATM_STATE_MENU);
    lastDisplayedOption = 0;
    ATM_Display_Menu();
    Serial.println("\n=== Enter Menu Choice (1-4) ===");
    Serial.print("Choice: ");
}

static void updateCardData(void) {
    if (_nfc == NULL) return;
    
    if (_nfc->getCardType() == NFC_CARD_TYPE_NTAG && _nfc->isCardPresent()) {
        Serial.println("Writing updated balance to card...");
        ATM_Display_UpdatingCard();
        
        CardData_t* card = ATM_GetCardData();
        
        // Format card data: CardID,Balance,Username,PIN
        char updatedCardData[128];
        snprintf(updatedCardData, sizeof(updatedCardData), "%s,%.2f,%s,%s",
                 card->cardID, card->balance, card->username, card->pinCode);
        
        if (_nfc->writeNTAGData(updatedCardData)) {
            Serial.println("Card updated successfully!");
            ATM_Display_CardUpdated();
            delay(1000);
        } else {
            Serial.println("Warning: Failed to update card!");
            ATM_Display_CardUpdateFailed();
            delay(1000);
        }
    } else {
        Serial.println("Warning: Card not present! Cannot update card.");
    }
}

