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
static Buzzer* _buzzer = NULL;
static KeypadInput* _keypad = NULL;

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
static unsigned long pinEntryStartTime = 0;
static bool pinTimeoutAlertActive = false;
static const unsigned long PIN_ENTRY_TIMEOUT_MS = 10000;

// Keypad state
static String keypadInputBuffer = "";
static ATM_State_t keypadContextState = ATM_STATE_IDLE;

static const unsigned long TAKE_CARD_REMINDER_DURATION_MS = 8000;

/* Private Function Prototypes */
static void handleKeypadInput(void);
static void handleCardDetection(void);
static void handleATMStates(void);
static void processWithdraw(float amount);
static void processDeposit(float amount);
static void updateCardData(void);
static void delayWithBuzzer(unsigned long durationMs);
static void handleMenuSelection(int choice);
static void handlePinValidation(void);
static void cancelToMenu(void);
static void resetSessionToScan(void);

void ATM_Controller_Init(LCD* lcd, NFC* nfc, Buzzer* buzzer, KeypadInput* keypad) {
    _lcd = lcd;
    _nfc = nfc;
    _buzzer = buzzer;
    _keypad = keypad;
    ATM_Display_Init(lcd);
}

void ATM_Controller_Setup(void) {
    // Initialize Serial communication
    Serial.begin(115200);
    delayWithBuzzer(1000);
    
    if (_keypad != NULL) {
        _keypad->begin();
    }

    if (_buzzer != NULL) {
        _buzzer->begin();
        _buzzer->stop();
    }
    
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
    delayWithBuzzer(500);
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
        delayWithBuzzer(3000);
    }
    
    // Initialize ATM application
    ATM_Init();
    
    // Display welcome screen
    ATM_Display_InitScreen();
    
    delayWithBuzzer(1000);
}

void ATM_Controller_Loop(void) {
    ATM_Process();

    if (_buzzer != NULL) {
        _buzzer->update();
    }
    
    // Handle keypad input
    handleKeypadInput();
    
    // Check for card periodically
    handleCardDetection();
    
    // Handle ATM states
    handleATMStates();
}

static void handleKeypadInput(void) {
    if (_keypad == NULL) {
        return;
    }

    ATM_State_t atmState = ATM_GetState();

    if (atmState != keypadContextState) {
        keypadContextState = atmState;
        keypadInputBuffer = "";
    }

    char key = _keypad->getKey();
    if (key == 0) {
        return;
    }

    if (key == 'D') {
        if (keypadInputBuffer.length() > 0) {
            keypadInputBuffer.remove(keypadInputBuffer.length() - 1);
        }
        return;
    }

    if (key == 'C') {
        if (atmState == ATM_STATE_PIN_ENTRY) {
            Serial.println("PIN entry cancelled by user.");
            resetSessionToScan();
        } else if (atmState == ATM_STATE_WITHDRAW ||
                   atmState == ATM_STATE_DEPOSIT ||
                   atmState == ATM_STATE_BALANCE ||
                   atmState == ATM_STATE_TRANSACTION_COMPLETE ||
                   atmState == ATM_STATE_MENU) {
            Serial.println("Operation cancelled. Returning to menu.");
            cancelToMenu();
        }
        return;
    }

    if (key == 'A') {
        switch (atmState) {
            case ATM_STATE_PIN_ENTRY:
                if (pinEntryActive && keypadInputBuffer.length() > 0) {
                    enteredPIN = keypadInputBuffer;
                    keypadInputBuffer = "";
                    pinEntryActive = false;
                    pinEntryStartTime = 0;
                    if (_buzzer != NULL && pinTimeoutAlertActive &&
                        _buzzer->isPatternActive(BUZZER_PATTERN_PIN_TIMEOUT)) {
                        _buzzer->stop();
                        pinTimeoutAlertActive = false;
                    }
                    handlePinValidation();
                }
                break;

            case ATM_STATE_MENU:
                if (keypadInputBuffer.length() > 0) {
                    int choice = keypadInputBuffer.toInt();
                    keypadInputBuffer = "";
                    handleMenuSelection(choice);
                }
                break;

            case ATM_STATE_WITHDRAW:
            case ATM_STATE_DEPOSIT:
                if (keypadInputBuffer.length() == 0) {
                    Serial.println("Amount required before confirmation.");
                } else {
                    float amount = keypadInputBuffer.toFloat();
                    keypadInputBuffer = "";
                    ATM_Display_Processing();
                    if (atmState == ATM_STATE_WITHDRAW) {
                        processWithdraw(amount);
                        TelegramNotifier::sendMessage(
                            "Withdrawal: " + String(amount) + " EGP\nBalance: " + String(balance)
                        );
                        
                    } else {
                        processDeposit(amount);
                        TelegramNotifier::sendMessage(
                            "Deposit: " + String(amount) + " EGP\nBalance: " + String(balance)
                        );
                    }
                }
                break;

            case ATM_STATE_BALANCE:
            case ATM_STATE_TRANSACTION_COMPLETE:
                cancelToMenu();
                break;

            default:
                break;
        }
        return;
    }

    if (key >= '0' && key <= '9') {
        if (atmState == ATM_STATE_PIN_ENTRY &&
            keypadInputBuffer.length() >= (MAX_PIN_LENGTH - 1)) {
            return;
        }
        if (atmState == ATM_STATE_MENU && keypadInputBuffer.length() >= 1) {
            keypadInputBuffer = "";
        }
        keypadInputBuffer += key;
        return;
    }

    if ((key == '*') &&
        (atmState == ATM_STATE_WITHDRAW || atmState == ATM_STATE_DEPOSIT)) {
        if (keypadInputBuffer.indexOf('.') == -1) {
            keypadInputBuffer += '.';
        }
        return;
    }

    // Ignore other keys (B, #, etc.)
}

static void handlePinValidation(void) {
    CardData_t* card = ATM_GetCardData();

    if (enteredPIN.equals(card->pinCode)) {
        Serial.println("PIN Correct! Access granted.");
        ATM_Display_PINCorrect();
        delayWithBuzzer(2000);

        enteredPIN = "";
        cancelToMenu();
        Serial.println("Select menu option 1-4 on keypad, then press A.");
    } else {
        Serial.println("PIN Incorrect! Access denied.");
        ATM_Display_PINIncorrect();
        delayWithBuzzer(2000);

        resetSessionToScan();
    }
}

static void handleMenuSelection(int choice) {
    if (choice < 1 || choice > 4) {
        Serial.println("Invalid choice! Enter 1-4 then press A.");
        return;
    }

    currentMenuOption = choice;

    switch (currentMenuOption) {
        case 1: {  // Balance
            ATM_SetState(ATM_STATE_BALANCE);
            lastDisplayedOption = 0;
            keypadInputBuffer = "";
            ATM_Display_Balance();
            CardData_t* card = ATM_GetCardData();
            Serial.print("Current Balance: $");
            Serial.println(card->balance);
            Serial.println("Press A or C to return to menu.");
            break;
        }

        case 2: {  // Withdraw
            ATM_SetState(ATM_STATE_WITHDRAW);
            lastDisplayedOption = 0;
            keypadInputBuffer = "";
            ATM_Display_WithdrawPrompt();
            Serial.println("Enter withdraw amount using keypad. Use * for decimal. Press A to confirm, D to delete, C to cancel.");
            break;
        }

        case 3: {  // Deposit
            ATM_SetState(ATM_STATE_DEPOSIT);
            lastDisplayedOption = 0;
            keypadInputBuffer = "";
            ATM_Display_DepositPrompt();
            Serial.println("Enter deposit amount using keypad. Use * for decimal. Press A to confirm, D to delete, C to cancel.");
            break;
        }

        case 4: {  // Exit
            ATM_Reset();
            ATM_SetState(ATM_STATE_IDLE);
            lastDisplayedOption = 0;
            cardDataRead = false;
            enteredPIN = "";
            pinEntryActive = false;

            ATM_Display_ThankYou();
            Serial.println("Thank you for using ATM!");
            delayWithBuzzer(2000);

            ATM_Display_TakeCard();
            if (_buzzer != NULL) {
                _buzzer->startPattern(BUZZER_PATTERN_TAKE_CARD);
            }
            delayWithBuzzer(TAKE_CARD_REMINDER_DURATION_MS);
            if (_buzzer != NULL) {
                _buzzer->stop();
            }

            ATM_Display_ScanCard();
            keypadInputBuffer = "";
            keypadContextState = ATM_STATE_IDLE;
            break;
        }
    }
}

static void cancelToMenu(void) {
    keypadInputBuffer = "";
    ATM_SetState(ATM_STATE_MENU);
    currentMenuOption = 1;
    lastDisplayedOption = 0;
    keypadContextState = ATM_STATE_MENU;
    ATM_Display_Menu();
}

static void resetSessionToScan(void) {
    ATM_Reset();
    ATM_SetState(ATM_STATE_IDLE);
    enteredPIN = "";
    pinEntryActive = false;
    pinEntryStartTime = 0;
    pinTimeoutAlertActive = false;
    currentMenuOption = 1;
    lastDisplayedOption = 0;
    cardDataRead = false;
    keypadInputBuffer = "";
    keypadContextState = ATM_STATE_IDLE;
    if (_buzzer != NULL) {
        _buzzer->stop();
    }
    ATM_Display_TakeCard();
    delayWithBuzzer(1500);
    ATM_Display_ScanCard();
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
                                
                                if (_buzzer != NULL) {
                                    _buzzer->startPattern(BUZZER_PATTERN_CARD_DETECTED);
                                }

                                // Display welcome message
                                ATM_Display_Welcome(card->username);
                                delayWithBuzzer(2000);
                                
                                // Request PIN entry
                                ATM_SetState(ATM_STATE_PIN_ENTRY);
                                enteredPIN = "";
                                pinEntryActive = true;
                                pinEntryStartTime = millis();
                                pinTimeoutAlertActive = false;
                                if (_buzzer != NULL &&
                                    _buzzer->isPatternActive(BUZZER_PATTERN_PIN_TIMEOUT)) {
                                    _buzzer->stop();
                                }
                                keypadInputBuffer = "";
                                keypadContextState = ATM_STATE_PIN_ENTRY;
                                
                                ATM_Display_PINEntry();
                                
                                Serial.println("\n=== PIN Entry Required ===");
                                Serial.println("Use keypad to enter your PIN, then press A.");
                                Serial.println("Press D to delete a digit or C to cancel.");
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
                pinEntryActive = false;
                pinEntryStartTime = 0;
                pinTimeoutAlertActive = false;
                if (_buzzer != NULL) {
                    _buzzer->stop();
                }
                
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
    
    if (atmState == ATM_STATE_PIN_ENTRY && pinEntryActive) {
        if (pinEntryStartTime == 0) {
            pinEntryStartTime = millis();
        } else if (!pinTimeoutAlertActive &&
                   millis() - pinEntryStartTime >= PIN_ENTRY_TIMEOUT_MS) {
            pinTimeoutAlertActive = true;
            if (_buzzer != NULL) {
                _buzzer->startPattern(BUZZER_PATTERN_PIN_TIMEOUT);
            }
        }
    } else if (atmState == ATM_STATE_BALANCE) {
        // Display balance - only once when entering this state
        static ATM_State_t lastState = ATM_STATE_IDLE;
        if (lastState != ATM_STATE_BALANCE) {
            ATM_Display_Balance();
        }
        lastState = atmState;
    } else if (atmState == ATM_STATE_TRANSACTION_COMPLETE) {
        ATM_Display_TransactionComplete();
    } else {
        pinEntryStartTime = 0;
        if (pinTimeoutAlertActive) {
            pinTimeoutAlertActive = false;
            if (_buzzer != NULL &&
                _buzzer->isPatternActive(BUZZER_PATTERN_PIN_TIMEOUT)) {
                _buzzer->stop();
            }
        }
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
            delayWithBuzzer(2000);
            ATM_Display_Balance();
            Serial.print("New Balance: $");
            Serial.println(card->balance);
            
            // Write updated balance back to card
            updateCardData();
        } else {
            Serial.println("Insufficient funds!");
            ATM_Display_InsufficientFunds();
            delayWithBuzzer(2000);
        }
    }
    Serial.println("\nPress A or C to return to menu...");
    ATM_SetState(ATM_STATE_MENU);
    lastDisplayedOption = 0;
    ATM_Display_Menu();
}

static void processDeposit(float amount) {
    if (amount > 0) {
        ATM_ProcessDeposit(amount);
        ATM_Display_TransactionComplete();
        Serial.print("Deposited: $");
        Serial.println(amount);
        delayWithBuzzer(2000);
        ATM_Display_Balance();
        CardData_t* card = ATM_GetCardData();
        Serial.print("New Balance: $");
        Serial.println(card->balance);
        
        // Write updated balance back to card
        updateCardData();
    }
    Serial.println("\nPress A or C to return to menu...");
    ATM_SetState(ATM_STATE_MENU);
    lastDisplayedOption = 0;
    ATM_Display_Menu();
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
            delayWithBuzzer(1000);
        } else {
            Serial.println("Warning: Failed to update card!");
            ATM_Display_CardUpdateFailed();
            delayWithBuzzer(1000);
        }
    } else {
        Serial.println("Warning: Card not present! Cannot update card.");
    }
}

static void delayWithBuzzer(unsigned long durationMs) {
    const unsigned long start = millis();
    while (millis() - start < durationMs) {
        if (_buzzer != NULL) {
            _buzzer->update();
        }
        delay(5);
    }
}

