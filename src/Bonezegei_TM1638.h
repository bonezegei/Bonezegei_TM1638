#ifndef BONEZEGEI_TM1638_H
#define BONEZEGEI_TM1638_H

#include <Arduino.h>

class Bonezegei_TM1638 {
public:
    Bonezegei_TM1638(uint8_t stbPin, uint8_t clkPin, uint8_t dioPin);
    
    void begin();
    void setBrightness(uint8_t brightness); // Brightness level 0 to 7
    void clear();
    
    // LED Control
    void setLED(uint8_t position, uint8_t state);
    void setLEDs(uint8_t mask);
    
    // Display Functions
    void displayRaw(uint8_t position, uint8_t segments);
    void displayDigit(uint8_t position, uint8_t digit, bool dot = false);
    void displayHex(uint8_t position, uint8_t hexVal, bool dot = false);
    void displayString(const char *str);
    void displayNumber(int32_t number);
    
    // Button Scanning
    uint8_t getButtons();

private:
    uint8_t _stbPin;
    uint8_t _clkPin;
    uint8_t _dioPin;
    uint8_t _brightness;
    
    void sendCommand(uint8_t cmd);
    void sendData(uint8_t address, uint8_t data);
    void writeByte(uint8_t data);
    uint8_t readByte();
};

#endif