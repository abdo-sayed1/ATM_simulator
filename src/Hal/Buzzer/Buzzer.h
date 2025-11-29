/*
 * Buzzer.h - Simple buzzer HAL for audible alerts
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

typedef enum {
    BUZZER_PATTERN_NONE = 0,
    BUZZER_PATTERN_PIN_TIMEOUT,
    BUZZER_PATTERN_TAKE_CARD,
    BUZZER_PATTERN_CARD_DETECTED
} BuzzerPattern_t;

class Buzzer {
public:
    explicit Buzzer(uint8_t pin);

    void begin(void);
    void startPattern(BuzzerPattern_t pattern);
    void stop(void);
    void update(void);
    bool isActive(void) const;
    BuzzerPattern_t getPattern(void) const;
    bool isPatternActive(BuzzerPattern_t pattern) const;

private:
    void applyPatternSettings(BuzzerPattern_t pattern);
    void toggleOutput(bool enable);

    uint8_t _pin;
    BuzzerPattern_t _pattern;
    uint16_t _frequency;
    uint16_t _onDuration;
    uint16_t _offDuration;
    uint8_t _repeatCount;   // 0 = continuous
    uint8_t _completedCycles;
    bool _isOnPhase;
    unsigned long _lastToggle;
};

#endif /* BUZZER_H */


