/*
 * Buzzer.cpp - Simple buzzer HAL for audible alerts
 */

#include "Buzzer.h"

Buzzer::Buzzer(uint8_t pin)
    : _pin(pin),
      _pattern(BUZZER_PATTERN_NONE),
      _frequency(0),
      _onDuration(0),
      _offDuration(0),
      _repeatCount(0),
      _completedCycles(0),
      _isOnPhase(false),
      _lastToggle(0) {}

void Buzzer::begin(void) {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void Buzzer::startPattern(BuzzerPattern_t pattern) {
    applyPatternSettings(pattern);
    _pattern = pattern;
    _completedCycles = 0;
    _isOnPhase = false;
    _lastToggle = millis();
    toggleOutput(false);
}

void Buzzer::stop(void) {
    toggleOutput(false);
    _pattern = BUZZER_PATTERN_NONE;
    _isOnPhase = false;
}

bool Buzzer::isActive(void) const {
    return _pattern != BUZZER_PATTERN_NONE;
}

BuzzerPattern_t Buzzer::getPattern(void) const {
    return _pattern;
}

bool Buzzer::isPatternActive(BuzzerPattern_t pattern) const {
    return _pattern == pattern;
}

void Buzzer::update(void) {
    if (_pattern == BUZZER_PATTERN_NONE) {
        return;
    }

    const unsigned long now = millis();
    const uint16_t window = _isOnPhase ? _onDuration : _offDuration;

    if (window == 0 || now - _lastToggle < window) {
        return;
    }

    _lastToggle = now;
    _isOnPhase = !_isOnPhase;

    toggleOutput(_isOnPhase);

    if (!_isOnPhase && _repeatCount > 0) {
        _completedCycles++;
        if (_completedCycles >= _repeatCount) {
            stop();
        }
    }
}

void Buzzer::applyPatternSettings(BuzzerPattern_t pattern) {
    switch (pattern) {
        case BUZZER_PATTERN_PIN_TIMEOUT:
            _frequency = 1800;
            _onDuration = 180;
            _offDuration = 120;
            _repeatCount = 10;
            break;

        case BUZZER_PATTERN_TAKE_CARD:
            _frequency = 1400;
            _onDuration = 400;
            _offDuration = 400;
            _repeatCount = 0;  // continuous until stopped
            break;

        case BUZZER_PATTERN_CARD_DETECTED:
            _frequency = 2000;
            _onDuration = 120;
            _offDuration = 80;
            _repeatCount = 3;
            break;

        default:
            _frequency = 0;
            _onDuration = 0;
            _offDuration = 0;
            _repeatCount = 0;
            break;
    }
}

void Buzzer::toggleOutput(bool enable) {
    if (enable && _frequency > 0) {
        tone(_pin, _frequency);
    } else {
        noTone(_pin);
        digitalWrite(_pin, LOW);
    }
}


