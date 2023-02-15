#pragma once

#include <stdint.h>

class Settings {
    public:
        double      temperatureActual;
        double      temperatureSetpoint;
        uint8_t     volume;
        bool        rfPowerOn;
        uint16_t    timeHeating;
        uint16_t    timeTreatment;
        float       power;
        uint8_t     batteryLevel;
        bool        inputSourceAttached;
        uint8_t     faultCount;
        uint8_t     faultWheelCounter;

    public:
        Settings();
};
