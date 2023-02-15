
#pragma once

#include "SoftwareTimer.h"
#include "main.h"

class HandsetCommunication : public SoftwareTimerHandler {
    private:

    public:
        void begin();
        void loop(Rt_system::Rt_out &out);
        void wheel_increment();
        
    /* SoftwareTimerHandler in Interrupt mode */
    public:
        virtual void handle();
    
};

extern HandsetCommunication comm;
