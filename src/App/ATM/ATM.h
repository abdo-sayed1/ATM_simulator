/*
 * ATM.h - ATM Simulator Application Header
 * Application layer for ATM operations
 */

#ifndef ATM_H
#define ATM_H

#include <Arduino.h>

/* Card Data Structure */
#define MAX_CARD_ID_LENGTH    20
#define MAX_USERNAME_LENGTH   30
#define MAX_PIN_LENGTH        10
#define MAX_BALANCE_LENGTH    20

typedef struct {
    char cardID[MAX_CARD_ID_LENGTH];
    float balance;
    char username[MAX_USERNAME_LENGTH];
    char pinCode[MAX_PIN_LENGTH];
    bool isValid;
} CardData_t;

/* ATM States */
typedef enum {
    ATM_STATE_IDLE,
    ATM_STATE_CARD_READ,
    ATM_STATE_PIN_ENTRY,
    ATM_STATE_MENU,
    ATM_STATE_BALANCE,
    ATM_STATE_WITHDRAW,
    ATM_STATE_DEPOSIT,
    ATM_STATE_TRANSACTION_COMPLETE
} ATM_State_t;

/* ATM Menu Options */
typedef enum {
    ATM_MENU_BALANCE = 1,
    ATM_MENU_WITHDRAW = 2,
    ATM_MENU_DEPOSIT = 3,
    ATM_MENU_EXIT = 4
} ATM_MenuOption_t;

/* Function Prototypes */
void ATM_Init(void);
void ATM_Process(void);
bool ATM_ReadCardData(const char* rawData);
CardData_t* ATM_GetCardData(void);
void ATM_DisplayWelcome(void);
void ATM_DisplayMenu(void);
void ATM_DisplayBalance(void);
void ATM_ProcessWithdraw(float amount);
void ATM_ProcessDeposit(float amount);
void ATM_Reset(void);
ATM_State_t ATM_GetState(void);
void ATM_SetState(ATM_State_t state);
float ATM_GetBalance(void);
float ATM_GetTransactionAmount(void);

#endif /* ATM_H */

