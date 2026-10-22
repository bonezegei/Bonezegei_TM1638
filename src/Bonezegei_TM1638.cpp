#include "Bonezegei_TM1638.h"

// 7-Segment font table (0-9, A-F)
static const uint8_t PROGMEM digitFont[] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // b
    0x39, // C
    0x5E, // d
    0x79, // E
    0x71  // F
};

Bonezegei_TM1638::Bonezegei_TM1638(uint8_t stbPin, uint8_t clkPin, uint8_t dioPin) {
    _stbPin = stbPin;
    _clkPin = clkPin;
    _dioPin = dioPin;
    _brightness = 7;
}

void Bonezegei_TM1638::begin() {
    pinMode(_stbPin, OUTPUT);
    pinMode(_clkPin, OUTPUT);
    pinMode(_dioPin, OUTPUT);
    
    digitalWrite(_stbPin, HIGH);
    digitalWrite(_clkPin, HIGH);
    
    setBrightness(_brightness);
    clear();
}

void Bonezegei_TM1638::writeByte(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(_clkPin, LOW);
        digitalWrite(_dioPin, (data >> i) & 0x01);
        digitalWrite(_clkPin, HIGH);
    }
}

uint8_t Bonezegei_TM1638::readByte() {
    uint8_t data = 0;
    pinMode(_dioPin, INPUT);
    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(_clkPin, LOW);
        if (digitalRead(_dioPin)) {
            data |= (1 << i);
        }
        digitalWrite(_clkPin, HIGH);
    }
    pinMode(_dioPin, OUTPUT);
    return data;
}

void Bonezegei_TM1638::sendCommand(uint8_t cmd) {
    digitalWrite(_stbPin, LOW);
    writeByte(cmd);
    digitalWrite(_stbPin, HIGH);
}

void Bonezegei_TM1638::sendData(uint8_t address, uint8_t data) {
    sendCommand(0x44); // Fixed address command
    digitalWrite(_stbPin, LOW);
    writeByte(address);
    writeByte(data);
    digitalWrite(_stbPin, HIGH);
}

void Bonezegei_TM1638::setBrightness(uint8_t brightness) {
    if (brightness > 7) brightness = 7;
    _brightness = brightness;
    sendCommand(0x88 | brightness);
}

void Bonezegei_TM1638::clear() {
    for (uint8_t i = 0; i < 16; i++) {
        sendData(0xC0 + i, 0x00);
    }
}

void Bonezegei_TM1638::setLED(uint8_t position, uint8_t state) {
    if (position >= 8) return;
    uint8_t address = 0xC1 + (position * 2);
    sendData(address, state ? 0x01 : 0x00);
}

void Bonezegei_TM1638::setLEDs(uint8_t mask) {
    for (uint8_t i = 0; i < 8; i++) {
        setLED(i, (mask >> i) & 0x01);
    }
}

void Bonezegei_TM1638::displayRaw(uint8_t position, uint8_t segments) {
    if (position >= 8) return;
    uint8_t address = 0xC0 + (position * 2);
    sendData(address, segments);
}

void Bonezegei_TM1638::displayDigit(uint8_t position, uint8_t digit, bool dot) {
    if (digit > 9) return;
    uint8_t segments = pgm_read_byte(&digitFont[digit]);
    if (dot) segments |= 0x80;
    displayRaw(position, segments);
}

void Bonezegei_TM1638::displayHex(uint8_t position, uint8_t hexVal, bool dot) {
    if (hexVal > 15) return;
    uint8_t segments = pgm_read_byte(&digitFont[hexVal]);
    if (dot) segments |= 0x80;
    displayRaw(position, segments);
}

void Bonezegei_TM1638::displayString(const char *str) {
    uint8_t pos = 0;
    while (*str && pos < 8) {
        char c = *str++;
        bool dot = false;
        if (*str == '.') {
            dot = true;
            str++;
        }
        
        uint8_t seg = 0x00;
        if (c >= '0' && c <= '9') seg = pgm_read_byte(&digitFont[c - '0']);
        else if (c >= 'A' && c <= 'F') seg = pgm_read_byte(&digitFont[c - 'A' + 10]);
        else if (c >= 'a' && c <= 'f') seg = pgm_read_byte(&digitFont[c - 'a' + 10]);
        else if (c == '-') seg = 0x40;
        else if (c == ' ') seg = 0x00;
        
        if (dot) seg |= 0x80;
        displayRaw(pos++, seg);
    }
    while (pos < 8) {
        displayRaw(pos++, 0x00);
    }
}

void Bonezegei_TM1638::displayNumber(int32_t number) {
    char buf[9];
    snprintf(buf, sizeof(buf), "%8ld", (long)number);
    displayString(buf);
}

uint8_t Bonezegei_TM1638::getButtons() {
    uint8_t keys = 0;
    digitalWrite(_stbPin, LOW);
    writeByte(0x42); // Read key data
    
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t val = readByte();
        if (val & 0x01) keys |= (1 << i);
        if (val & 0x10) keys |= (1 << (i + 4));
    }
    
    digitalWrite(_stbPin, HIGH);
    return keys;
}