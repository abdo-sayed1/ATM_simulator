/*
 * ATM_Display.h - ATM Display Functions Header
 * Display functions for LCD screen
 */

#ifndef ATM_DISPLAY_H
#define ATM_DISPLAY_H

#include <Arduino.h>
#include "src/Hal/LCD/LCD.h"
#include "ATM.h"

/* Function Prototypes */
void ATM_Display_Init(LCD* lcd);
void ATM_Display_Menu(void);
void ATM_Display_Balance(void);
void ATM_Display_TransactionComplete(void);
void ATM_Display_Welcome(const char* username);
void ATM_Display_PINEntry(void);
void ATM_Display_PINCorrect(void);
void ATM_Display_PINIncorrect(void);
void ATM_Display_WithdrawPrompt(void);
void ATM_Display_DepositPrompt(void);
void ATM_Display_Processing(void);
void ATM_Display_UpdatingCard(void);
void ATM_Display_CardUpdated(void);
void ATM_Display_CardUpdateFailed(void);
void ATM_Display_InsufficientFunds(void);
void ATM_Display_ThankYou(void);
void ATM_Display_TakeCard(void);
void ATM_Display_ScanCard(void);
void ATM_Display_NFCReady(void);
void ATM_Display_NFCError(void);
void ATM_Display_InitScreen(void);

#endif /* ATM_DISPLAY_H */

