
#pragma once

#include <Arduino.h>

inline void
printAddress(byte address)
{
    if (address < 16) {
        Serial.print("0");
    }
    Serial.println(address, HEX);
}

inline void
printHex(byte r)
{
    Serial.print("0x");
    if (r < 16) {
        Serial.print("0");
    }
    Serial.print(r, HEX);
}

inline void
printBinary(byte r)
{
    Serial.print("0b");
    if (r < 2) {
        Serial.print("0");
    }
    if (r < 4) {
        Serial.print("0");
    }
    if (r < 8) {
        Serial.print("0");
    }
    if (r < 16) {
        Serial.print("0");
    }
    if (r < 32) {
        Serial.print("0");
    }
    if (r < 64) {
        Serial.print("0");
    }
    if (r < 128) {
        Serial.print("0");
    }
    Serial.print(r, BIN);
}
