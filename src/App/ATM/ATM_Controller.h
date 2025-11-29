/*
 * ATM_Controller.h - ATM Controller Header
 * Main ATM logic and state management
 */

#ifndef ATM_CONTROLLER_H
#define ATM_CONTROLLER_H

#include <Arduino.h>
#include "src/Hal/LCD/LCD.h"
#include "src/Hal/NFC/NFC.h"
#include "src/Hal/Buzzer/Buzzer.h"
#include "src/Hal/Keypad/KeypadInput.h"
#include "ATM.h"

/* Function Prototypes */
void ATM_Controller_Init(LCD* lcd, NFC* nfc, Buzzer* buzzer, KeypadInput* keypad);
void ATM_Controller_Setup(void);
void ATM_Controller_Loop(void);

#endif /* ATM_CONTROLLER_H */

