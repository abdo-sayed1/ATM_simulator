/*
 * KeypadInput.cpp - 4x4 matrix keypad helper
 */

#include "KeypadInput.h"

static char _keyMap[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

static byte _rowPins[KEYPAD_ROWS] = {
    KEYPAD_ROW0_PIN,
    KEYPAD_ROW1_PIN,
    KEYPAD_ROW2_PIN,
    KEYPAD_ROW3_PIN
};

static byte _colPins[KEYPAD_COLS] = {
    KEYPAD_COL0_PIN,
    KEYPAD_COL1_PIN,
    KEYPAD_COL2_PIN,
    KEYPAD_COL3_PIN
};

KeypadInput::KeypadInput(void)
    : _keypad(makeKeymap(_keyMap), _rowPins, _colPins, KEYPAD_ROWS, KEYPAD_COLS) {}

void KeypadInput::begin(void) {
    // Keypad library handles pin initialization internally.
}

char KeypadInput::getKey(void) {
    return _keypad.getKey();
}


