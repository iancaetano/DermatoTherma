
#pragma once

#include "SoftwareTimer.h"
#include "main.h"

class TreatmentTimeHandler : public SoftwareTimerHandler {

    private:
        static const float OFFSET;

    private:
        bool m_isTreating;
        bool m_isThresholdExceeded;

    public:
        void reset();
        void loop(Rt_system::Rt_out &out);

    /* SoftwareTimerHandler in Interrupt mode */
    public:
        virtual void handle();

};

extern TreatmentTimeHandler treatmentTimeHandler;
