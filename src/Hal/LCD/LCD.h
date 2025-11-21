/*
 * LCD.h - Nokia 5110 LCD Display Driver Header
 * Hardware Abstraction Layer for PCD8544 Controller
 */

#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <SPI.h>

/* LCD Dimensions */
#define LCD_WIDTH  84
#define LCD_HEIGHT 48

/* LCD Commands */
#define LCD_CMD_MODE     0x20
#define LCD_CMD_DISPLAY  0x08
#define LCD_CMD_Y_ADDR   0x40
#define LCD_CMD_X_ADDR   0x80
#define LCD_CMD_TEMP     0x04
#define LCD_CMD_BIAS     0x10
#define LCD_CMD_VOP      0x80

/* Display Control */
#define LCD_DISPLAY_BLANK    0x00
#define LCD_DISPLAY_NORMAL   0x04
#define LCD_DISPLAY_ALL_ON   0x01
#define LCD_DISPLAY_INVERT   0x05

class LCD {
public:
    LCD(uint8_t rst, uint8_t ce, uint8_t dc, uint8_t din, uint8_t clk);
    void begin(void);
    void clear(void);
    void setContrast(uint8_t contrast);
    void setCursor(uint8_t x, uint8_t y);
    void print(const char* str);
    void printChar(char c);
    void drawPixel(uint8_t x, uint8_t y, uint8_t color);
    void updateDisplay(void);
    void setInverse(bool inverse);
    void setPower(bool on);

private:
    uint8_t _rst;
    uint8_t _ce;
    uint8_t _dc;
    uint8_t _din;
    uint8_t _clk;
    
    uint8_t _cursorX;
    uint8_t _cursorY;
    uint8_t _contrast;
    bool _inverse;
    
    uint8_t _displayBuffer[LCD_WIDTH * LCD_HEIGHT / 8];
    
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void setAddress(uint8_t x, uint8_t y);
    void initDisplay(void);
};

#endif /* LCD_H */

