

#ifndef __ROTARY_ENCODER_H__
#define __ROTARY_ENCODER_H__

#include "Arduino.h"
#include <stm32g4xx_hal.h>

class RotaryEncoder {
    private:
        TIM_HandleTypeDef m_htim3;
        int32_t           m_counter;
        int32_t           m_counterOld;

    public:
        RotaryEncoder();

        void begin();
        void start();
        void stop();
        int32_t count();
        int32_t difference();

};

#endif // __ROTARY_ENCODER_H__