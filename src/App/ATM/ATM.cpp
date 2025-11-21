/*
 * ATM.cpp - ATM Simulator Application Implementation
 * Application layer for ATM operations
 */

#include "ATM.h"
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

/* Private Variables */
static CardData_t cardData;
static ATM_State_t currentState = ATM_STATE_IDLE;
static float transactionAmount = 0.0;

/* Private Function Prototypes */
static void parseCardData(const char* rawData);
static int findCharIndex(const char* str, char c, int startPos);
static void floatToString(float value, char* buffer, int bufferSize);

void ATM_Init(void) {
    memset(&cardData, 0, sizeof(CardData_t));
    cardData.isValid = false;
    currentState = ATM_STATE_IDLE;
    transactionAmount = 0.0;
}

void ATM_Process(void) {
    // State machine processing
    switch (currentState) {
        case ATM_STATE_IDLE:
            // Waiting for card
            break;
            
        case ATM_STATE_CARD_READ:
            // Card read, show menu
            currentState = ATM_STATE_MENU;
            ATM_DisplayMenu();
            break;
            
        case ATM_STATE_MENU:
            // Menu is displayed, waiting for user selection
            break;
            
        case ATM_STATE_BALANCE:
            // Display balance
            ATM_DisplayBalance();
            break;
            
        case ATM_STATE_WITHDRAW:
            // Process withdrawal
            break;
            
        case ATM_STATE_DEPOSIT:
            // Process deposit
            break;
            
        case ATM_STATE_TRANSACTION_COMPLETE:
            // Transaction complete, return to menu
            delay(2000);
            currentState = ATM_STATE_MENU;
            ATM_DisplayMenu();
            break;
            
        default:
            break;
    }
}

bool ATM_ReadCardData(const char* rawData) {
    if (rawData == NULL || strlen(rawData) == 0) {
        return false;
    }
    
    // Parse the card data
    parseCardData(rawData);
    
    if (cardData.isValid) {
        currentState = ATM_STATE_CARD_READ;
        return true;
    }
    
    return false;
}

void parseCardData(const char* rawData) {
    // Reset card data
    memset(&cardData, 0, sizeof(CardData_t));
    cardData.isValid = false;
    
    // Format: CardID,Balance,Username,PIN
    int len = strlen(rawData);
    if (len == 0) {
        return;
    }
    
    // Find comma positions
    int p1 = findCharIndex(rawData, ',', 0);
    int p2 = findCharIndex(rawData, ',', p1 + 1);
    int p3 = findCharIndex(rawData, ',', p2 + 1);
    
    if (p1 == -1 || p2 == -1 || p3 == -1) {
        // Invalid format
        return;
    }
    
    // Extract Card ID
    int cardIDLen = p1;
    if (cardIDLen > 0 && cardIDLen < MAX_CARD_ID_LENGTH) {
        strncpy(cardData.cardID, rawData, cardIDLen);
        cardData.cardID[cardIDLen] = '\0';
    }
    
    // Extract Balance
    int balanceLen = p2 - p1 - 1;
    if (balanceLen > 0 && balanceLen < MAX_BALANCE_LENGTH) {
        char balanceStr[MAX_BALANCE_LENGTH];
        strncpy(balanceStr, &rawData[p1 + 1], balanceLen);
        balanceStr[balanceLen] = '\0';
        cardData.balance = atof(balanceStr);
    }
    
    // Extract Username
    int usernameLen = p3 - p2 - 1;
    if (usernameLen > 0 && usernameLen < MAX_USERNAME_LENGTH) {
        strncpy(cardData.username, &rawData[p2 + 1], usernameLen);
        cardData.username[usernameLen] = '\0';
    }
    
    // Extract PIN
    int pinLen = len - p3 - 1;
    if (pinLen > 0 && pinLen < MAX_PIN_LENGTH) {
        strncpy(cardData.pinCode, &rawData[p3 + 1], pinLen);
        cardData.pinCode[pinLen] = '\0';
    }
    
    // Validate that we got all fields
    if (strlen(cardData.cardID) > 0 && 
        strlen(cardData.username) > 0 && 
        strlen(cardData.pinCode) > 0) {
        cardData.isValid = true;
    }
}

int findCharIndex(const char* str, char c, int startPos) {
    if (str == NULL) {
        return -1;
    }
    
    int len = strlen(str);
    for (int i = startPos; i < len; i++) {
        if (str[i] == c) {
            return i;
        }
    }
    return -1;
}

CardData_t* ATM_GetCardData(void) {
    return &cardData;
}

void ATM_DisplayWelcome(void) {
    // This will be called from main to display welcome screen
}

void ATM_DisplayMenu(void) {
    // This will be called from main to display menu on LCD
    // Menu options will be handled in main loop
}

void ATM_DisplayBalance(void) {
    // This will be called from main to display balance on LCD
}

void ATM_ProcessWithdraw(float amount) {
    if (amount > 0 && amount <= cardData.balance) {
        cardData.balance -= amount;
        transactionAmount = amount;
        currentState = ATM_STATE_TRANSACTION_COMPLETE;
    }
}

void ATM_ProcessDeposit(float amount) {
    if (amount > 0) {
        cardData.balance += amount;
        transactionAmount = amount;
        currentState = ATM_STATE_TRANSACTION_COMPLETE;
    }
}

void ATM_Reset(void) {
    memset(&cardData, 0, sizeof(CardData_t));
    cardData.isValid = false;
    currentState = ATM_STATE_IDLE;
    transactionAmount = 0.0;
}

ATM_State_t ATM_GetState(void) {
    return currentState;
}

void ATM_SetState(ATM_State_t state) {
    currentState = state;
}

float ATM_GetBalance(void) {
    return cardData.balance;
}

float ATM_GetTransactionAmount(void) {
    return transactionAmount;
}

void floatToString(float value, char* buffer, int bufferSize) {
    if (buffer == NULL || bufferSize == 0) {
        return;
    }
    
    // Convert float to string with 2 decimal places
    dtostrf(value, 0, 2, buffer);
}

