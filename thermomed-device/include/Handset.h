#pragma once

#include "Arduino.h"
#include "SoftwareTimer.h"
#include "main.h"
#include "PushButton.h"

#define numReadings                 20      // define number of readings for average


class HandsetClass : public SoftwareTimerHandler{
    public:
    
        HandsetClass();
        
        void init();
        void loop();
        
        double Temp;
        double AmbTemp;

        double readObjTemp();
        double readAmbTemp();


        void handle();

        StartButton startButton;
    
    private:

        double total_obj = 0;
        double val_obj;                      // object temperature
        double avg_val_obj[numReadings] = {};
        double inverse;
        double inverse_2;

};


