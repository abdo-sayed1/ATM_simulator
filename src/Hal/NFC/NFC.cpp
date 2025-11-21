/*
 * NFC.cpp - NFC Reader Driver Implementation
 * Hardware Abstraction Layer for PN532 NFC Reader
 * Uses Adafruit_PN532 library internally
 */

#include "NFC.h"
#include <string.h>
#include <Arduino.h>

NFC::NFC(uint8_t sda, uint8_t scl) {
    _sda = sda;
    _scl = scl;
    _cardType = NFC_CARD_TYPE_UNKNOWN;
    _lastUIDLength = 0;
    memset(_lastUID, 0, sizeof(_lastUID));
    _pn532 = new Adafruit_PN532(_sda, _scl);
}

bool NFC::begin(void) {
    // Initialize I2C
    Wire.begin(_sda, _scl);
    
    // Initialize PN532
    _pn532->begin();
    
    // Configure SAM (Secure Access Module)
    _pn532->SAMConfig();
    
    // Test communication by getting firmware version
    uint32_t versiondata = _pn532->getFirmwareVersion();
    if (!versiondata) {
        return false;
    }
    
    return true;
}

bool NFC::isCardPresent(void) {
    uint8_t uid[7];
    uint8_t uidLength;
    
    // Try to read a passive target (card)
    if (_pn532->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100)) {
        // Card found, store UID
        if (uidLength > 0 && uidLength <= NFC_MAX_UID_LENGTH) {
            _lastUIDLength = uidLength;
            memcpy(_lastUID, uid, uidLength);
            
            // Determine card type based on UID length and try to read as NTAG
            if (uidLength == 7) {
                // Try to read as NTAG (7-byte UID is typical for NTAG)
                uint8_t buffer[4];
                if (_pn532->ntag2xx_ReadPage(4, buffer)) {
                    _cardType = NFC_CARD_TYPE_NTAG;
                } else {
                    _cardType = NFC_CARD_TYPE_MIFARE;
                }
            } else {
                _cardType = NFC_CARD_TYPE_MIFARE;
            }
            
            return true;
        }
    }
    
    return false;
}

bool NFC::readCard(uint8_t* uid, uint8_t* uidLength) {
    if (isCardPresent()) {
        if (uid != NULL && uidLength != NULL) {
            memcpy(uid, _lastUID, _lastUIDLength);
            *uidLength = _lastUIDLength;
        }
        return true;
    }
    return false;
}

void NFC::getCardUIDString(char* buffer, uint8_t bufferSize) {
    if (buffer == NULL || bufferSize == 0) {
        return;
    }
    
    if (_lastUIDLength == 0) {
        strncpy(buffer, "No Card", bufferSize - 1);
        buffer[bufferSize - 1] = '\0';
        return;
    }
    
    // Convert UID to hex string
    uint8_t pos = 0;
    for (uint8_t i = 0; i < _lastUIDLength && pos < bufferSize - 3; i++) {
        if (i > 0 && pos < bufferSize - 1) {
            buffer[pos++] = ' ';
        }
        uint8_t high = (_lastUID[i] >> 4) & 0x0F;
        uint8_t low = _lastUID[i] & 0x0F;
        
        buffer[pos++] = (high < 10) ? ('0' + high) : ('A' + high - 10);
        if (pos < bufferSize - 1) {
            buffer[pos++] = (low < 10) ? ('0' + low) : ('A' + low - 10);
        }
    }
    buffer[pos] = '\0';
}

uint8_t NFC::getCardType(void) {
    return _cardType;
}

void NFC::reset(void) {
    _lastUIDLength = 0;
    _cardType = NFC_CARD_TYPE_UNKNOWN;
    memset(_lastUID, 0, sizeof(_lastUID));
}

bool NFC::readNTAGPage(uint8_t page, uint8_t* buffer) {
    if (buffer == NULL) {
        return false;
    }
    
    return _pn532->ntag2xx_ReadPage(page, buffer);
}

bool NFC::readNTAGData(char* data, uint16_t maxLength, uint8_t startPage, uint8_t endPage) {
    if (data == NULL || maxLength == 0) {
        return false;
    }
    
    String dataString = "";
    uint8_t buffer[4];
    
    for (uint8_t page = startPage; page <= endPage; page++) {
        if (_pn532->ntag2xx_ReadPage(page, buffer)) {
            for (uint8_t i = 0; i < 4; i++) {
                if (buffer[i] == 0x00) {
                    // Null terminator found, stop reading
                    goto done;
                }
                if (dataString.length() < maxLength - 1) {
                    dataString += (char)buffer[i];
                } else {
                    goto done;
                }
            }
        } else {
            // Failed to read page, stop
            break;
        }
    }
    
done:
    // Copy to output buffer
    uint16_t len = dataString.length();
    if (len > maxLength - 1) {
        len = maxLength - 1;
    }
    strncpy(data, dataString.c_str(), len);
    data[len] = '\0';
    
    return (len > 0);
}

bool NFC::writeNTAGPage(uint8_t page, uint8_t* data) {
    if (data == NULL) {
        return false;
    }
    
    return _pn532->ntag2xx_WritePage(page, data);
}

bool NFC::writeNTAGData(const char* data, uint8_t startPage, uint8_t endPage) {
    if (data == NULL) {
        return false;
    }
    
    uint16_t dataLen = strlen(data);
    uint8_t byteIndex = 0;
    
    // Write data to pages (4 bytes per page)
    for (uint8_t page = startPage; page <= endPage && byteIndex <= dataLen; page++) {
        uint8_t pageData[4];
        
        // Fill page with data
        for (uint8_t i = 0; i < 4; i++) {
            if (byteIndex < dataLen) {
                pageData[i] = data[byteIndex++];
            } else {
                // Fill remaining with null terminators
                pageData[i] = 0x00;
            }
        }
        
        // Write page to card
        if (!_pn532->ntag2xx_WritePage(page, pageData)) {
            return false;
        }
        
        delay(50);  // Small delay between writes
        
        // Stop if we've written all data including null terminator
        if (byteIndex > dataLen) {
            break;
        }
    }
    
    return true;
}
