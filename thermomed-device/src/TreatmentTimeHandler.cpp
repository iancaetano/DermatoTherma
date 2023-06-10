
#include "TreatmentTimeHandler.h"

const float TreatmentTimeHandler::OFFSET = 0.5f;

void
TreatmentTimeHandler::reset()
{
    m_isTreating = false;
    m_isThresholdExceeded = false;

    settings.timeHeating = 0;
    settings.timeTreatment = 0;
}

void
TreatmentTimeHandler::loop(Rt_system::Rt_out &out)
{
    m_isTreating = (out.rt_status == Rt_system::Rt_status::treating);
}

void
TreatmentTimeHandler::handle()
{
    

    if (m_isTreating) {
        if (!m_isThresholdExceeded && settings.temperatureActual >= (settings.temperatureSetpoint - OFFSET)) {
            m_isThresholdExceeded = true;
        }

        if (m_isThresholdExceeded) {
            settings.timeTreatment++;
        } else {
            settings.timeHeating++;
        }
    }
}

TreatmentTimeHandler treatmentTimeHandler;