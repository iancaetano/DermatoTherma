#include "Settings.h"

Settings::Settings()
{
    temperatureActual      = 0.0f;
    temperatureSetpoint    = 50.0f;
    volume                 = 1;
    rfPowerOn              = false;
    timeHeating            = 0;
    timeTreatment          = 0;
    power                  = 0.0f;
    inputSourceAttached    = false;
    batteryLevel           = 0;
}

