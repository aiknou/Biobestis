#pragma once
#include <Wire.h>

#define TCA_ADDRESS 0x70

inline void tcaSelect(uint8_t channel) {
    Wire.beginTransmission(TCA_ADDRESS);
    Wire.write(1 << channel);
    Wire.endTransmission();
}
