#pragma once

#include "Arduino.h"
#include "SoftwareTimer.h"
#include "PushButton.h"
#include <Wire.h>

#define numReadings                 20      // define number of readings for average


class HandsetClass : public SoftwareTimerHandler{
    public:
    

        TwoWire wireOne = TwoWire();

        HandsetClass();
        
        void init();
        void loop();
        
        double Temp;
        double AmbTemp;

        double readObjTemp();
        double readAmbTemp();


        void handle();

        StartButton startButton;

        void writeToWireOne(byte ADDR,byte REG, byte value);
        void writeDCDCOutToWireOne(byte ADDR,byte REG, byte value);
        byte readfromWireOne(byte ADDR,byte REG);
    
    private:

        double total_obj = 0;
        double val_obj;                      // object temperature
        double avg_val_obj[numReadings] = {};
        double inverse;
        double inverse_2;

};


