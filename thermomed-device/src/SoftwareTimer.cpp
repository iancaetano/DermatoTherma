#include "SoftwareTimer.h"
#include <Arduino.h>

/*******************************************************************************
 * SoftwareTimerHandler
 */

class DefaultSoftwareTimerHandler : public SoftwareTimerHandler {
    public:
        virtual void handle() {
            //
        }
};

static DefaultSoftwareTimerHandler defaultHandler;


/*******************************************************************************
 * SoftwareTimer::Handler
 */
SoftwareTimer::Element::Element(Period period, Mode mode, SoftwareTimerHandler& handler) :
m_period(period), m_mode(mode), m_handlerType(HANDLER_CLASS), m_handler(handler),
m_handlerFunc(nullptr), m_timeout(millis() + period), m_markedForDeletion(false)
{
    //
}


SoftwareTimer::Element::Element(Period period, Mode mode, SoftwareTimerHandlerFunc& handlerFunc) :
m_period(period), m_mode(mode), m_handlerType(HANDLER_FUNC), m_handler(defaultHandler),
m_handlerFunc(handlerFunc), m_timeout(millis() + period), m_markedForDeletion(false)
{
    //
}


/*******************************************************************************
 * SoftwareTimer
 */

SoftwareTimer::SoftwareTimer() :
m_timer(TIM6)
{
    m_timer.pause();

    /* 170MHz / 1'700 = 100kHz */
    /* 100kHz / 100 = 1kHz = 1 ms */
    m_timer.setPrescaleFactor(1700);
    m_timer.setOverflow(100);
    m_timer.setCount(0);
    m_timer.setPreloadEnable(true);
    m_timer.refresh();

    m_timer.attachInterrupt(std::bind(&SoftwareTimer::interrupt, this));
}

void
SoftwareTimer::begin()
{
    m_timer.resume();
}

/**
 * @brief 
 * 
 * @see https://stackoverflow.com/questions/7713266/how-can-i-change-the-variable-to-which-a-c-reference-refers
 * @see https://stackoverflow.com/questions/728233/why-are-references-not-reseatable-in-c
 * 
 * @param period 
 * @param mode 
 * @param handlerKlass 
 */
void
SoftwareTimer::addHandler(Period period, Mode mode, SoftwareTimerHandler& handlerKlass)
{
    Element element(period, mode, handlerKlass);

    m_container.push_back(element);
}

void
SoftwareTimer::addHandler(Period period, Mode mode, SoftwareTimerHandlerFunc handlerFunc)
{
    Element element(period, mode, handlerFunc);

    m_container.push_back(element);
}

void
SoftwareTimer::interrupt()
{
    uint32_t now = millis();
    
    for (Element& element : m_container) {
        if (now > element.m_timeout) {
            element.m_timeout = now + element.m_period;
            if (element.m_handlerType == HANDLER_CLASS) {
                element.m_handler.handle();
            } else if (element.m_handlerType == HANDLER_FUNC && element.m_handlerFunc != nullptr) {
                element.m_handlerFunc();
            }
        }
    }
}
