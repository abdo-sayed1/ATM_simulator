/*
 * NFC.h - NFC Reader Driver Header
 * Hardware Abstraction Layer for PN532 NFC Reader
 * Uses Adafruit_PN532 library internally
 */

#ifndef NFC_H
#define NFC_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PN532.h>

/* NFC Constants */
#define NFC_MAX_UID_LENGTH  10

/* Card Types */
#define NFC_CARD_TYPE_MIFARE        0x01
#define NFC_CARD_TYPE_NTAG          0x02
#define NFC_CARD_TYPE_UNKNOWN       0x00

class NFC {
public:
    NFC(uint8_t sda, uint8_t scl);
    bool begin(void);
    bool isCardPresent(void);
    bool readCard(uint8_t* uid, uint8_t* uidLength);
    void getCardUIDString(char* buffer, uint8_t bufferSize);
    uint8_t getCardType(void);
    void reset(void);
    
    // Additional methods for NTAG reading
    bool readNTAGPage(uint8_t page, uint8_t* buffer);
    bool readNTAGData(char* data, uint16_t maxLength, uint8_t startPage = 4, uint8_t endPage = 39);
    
    // Methods for NTAG writing
    bool writeNTAGPage(uint8_t page, uint8_t* data);
    bool writeNTAGData(const char* data, uint8_t startPage = 4, uint8_t endPage = 39);

private:
    uint8_t _sda;
    uint8_t _scl;
    uint8_t _cardType;
    uint8_t _lastUID[NFC_MAX_UID_LENGTH];
    uint8_t _lastUIDLength;
    
    Adafruit_PN532* _pn532;
};

#endif /* NFC_H */

