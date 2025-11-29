/*
 * KeypadInput.h - 4x4 matrix keypad helper
 */

#ifndef KEYPAD_INPUT_H
#define KEYPAD_INPUT_H

#include <Arduino.h>
#include <Keypad.h>
#include "APP_Cfg.h"

class KeypadInput {
public:
    KeypadInput(void);
    void begin(void);
    char getKey(void);

private:
    Keypad _keypad;
};

#endif /* KEYPAD_INPUT_H */


