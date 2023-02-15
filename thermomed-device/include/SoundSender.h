
#pragma once

#include <inttypes.h>
#include "main.h"
#include "SoftwareTimer.h"

class SoundSender : public SoftwareTimerHandler {

    private:
        static const float OFFSET;

    public:
        SoundSender();
        void begin();
        void loop(Rt_system::Rt_out &out);

        void send(bool enable, uint8_t volume, uint16_t temperature);

        
    /* SoftwareTimerHandler in Interrupt mode */
    private:
        bool m_timeout;

    public:
        virtual void handle();
    
};

extern SoundSender soundSender;
