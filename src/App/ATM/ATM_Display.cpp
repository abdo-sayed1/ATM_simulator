/*
 * ATM_Display.cpp - ATM Display Functions Implementation
 * Display functions for LCD screen
 */

#include "ATM_Display.h"
#include <string.h>
#include <stdlib.h>

static LCD* _lcd = NULL;

void ATM_Display_Init(LCD* lcd) {
    _lcd = lcd;
}

void ATM_Display_Menu(void) {
    if (_lcd == NULL) return;
    
    // Display full menu on LCD
    _lcd->clear();
    
    // Row 0: Title
    _lcd->setCursor(0, 0);
    _lcd->print("ATM Menu:");
    
    // Row 1: Option 1
    _lcd->setCursor(0, 1);
    _lcd->print("1.Balance");
    
    // Row 2: Option 2  
    _lcd->setCursor(0, 2);
    _lcd->print("2.Withdraw");
    
    // Row 3: Option 3
    _lcd->setCursor(0, 3);
    _lcd->print("3.Deposit");
    
    // Row 4: Option 4
    _lcd->setCursor(0, 4);
    _lcd->print("4.Exit");
    
    // Row 5: Input prompt
    _lcd->setCursor(0, 5);
    _lcd->print("Enter 1-4");
}

void ATM_Display_Balance(void) {
    if (_lcd == NULL) return;
    
    CardData_t* card = ATM_GetCardData();
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Account Balance");
    
    _lcd->setCursor(0, 1);
    _lcd->print("$");
    
    char balanceStr[20];
    dtostrf(card->balance, 0, 2, balanceStr);
    _lcd->print(balanceStr);
    
    _lcd->setCursor(0, 2);
    _lcd->print("Press Enter");
    _lcd->setCursor(0, 3);
    _lcd->print("to continue");
}

void ATM_Display_TransactionComplete(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Transaction");
    _lcd->setCursor(0, 1);
    _lcd->print("Complete!");
}

void ATM_Display_Welcome(const char* username) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Welcome");
    _lcd->setCursor(0, 1);
    
    // Display username (truncate if needed)
    if (strlen(username) <= 14) {
        _lcd->print(username);
    } else {
        char truncated[15];
        strncpy(truncated, username, 14);
        truncated[14] = '\0';
        _lcd->print(truncated);
    }
}

void ATM_Display_PINEntry(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Enter PIN:");
    _lcd->setCursor(0, 1);
    _lcd->print("Serial Monitor");
}

void ATM_Display_PINCorrect(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("PIN Correct");
    _lcd->setCursor(0, 1);
    _lcd->print("Access Granted");
}

void ATM_Display_PINIncorrect(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("PIN Incorrect");
    _lcd->setCursor(0, 1);
    _lcd->print("Access Denied");
}

void ATM_Display_WithdrawPrompt(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Withdraw");
    _lcd->setCursor(0, 1);
    _lcd->print("Enter Amount:");
    _lcd->setCursor(0, 2);
    _lcd->print("Serial Monitor");
}

void ATM_Display_DepositPrompt(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Deposit");
    _lcd->setCursor(0, 1);
    _lcd->print("Enter Amount:");
    _lcd->setCursor(0, 2);
    _lcd->print("Serial Monitor");
}

void ATM_Display_Processing(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Processing...");
}

void ATM_Display_UpdatingCard(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Updating Card");
    _lcd->setCursor(0, 1);
    _lcd->print("Keep Card On");
}

void ATM_Display_CardUpdated(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Card Updated!");
}

void ATM_Display_CardUpdateFailed(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Update Failed!");
}

void ATM_Display_InsufficientFunds(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Insufficient");
    _lcd->setCursor(0, 1);
    _lcd->print("Funds!");
}

void ATM_Display_ThankYou(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Thank You!");
    _lcd->setCursor(0, 1);
    _lcd->print("Goodbye");
}

void ATM_Display_TakeCard(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Take Your Card");
    _lcd->setCursor(0, 1);
    _lcd->print("Please Remove");
}

void ATM_Display_ScanCard(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("Scan Card");
}

void ATM_Display_NFCReady(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("NFC Ready");
    _lcd->setCursor(0, 1);
    _lcd->print("Scan Card");
}

void ATM_Display_NFCError(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("NFC Error!");
    _lcd->setCursor(0, 1);
    _lcd->print("Check Wiring");
}

void ATM_Display_InitScreen(void) {
    if (_lcd == NULL) return;
    
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("ATM Simulator");
    _lcd->setCursor(0, 1);
    _lcd->print("Scan Card...");
}

