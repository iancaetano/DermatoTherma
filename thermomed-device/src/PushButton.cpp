#include "PushButton.h"
#include "Menu.h"
#include "BQ25792.h"
#include  "main.h"
#include "TreatmentTimeHandler.h"

#include <Arduino.h>

/*******************************************************************************
 * PushButtonHandler
 */



class PushButtonHandlerDefault : public PushButtonHandler {
    public:
        PushButtonHandlerDefault()
        {

        }

        virtual ~PushButtonHandlerDefault()
        {

        }

        void handle(int pin, PushButtonEvent event)
        {
            //
        }
};

static PushButtonHandlerDefault defaultHandler;


/*******************************************************************************
 * PushButton
 */

const int PushButton::DEBOUNCE = 100;

PushButton::PushButton(int pin) :
m_pin(pin), m_lastCheck(0), m_handlerType(HANDLER_CLASS), m_handler(defaultHandler),
m_handlerFunc(nullptr), m_lastEvent(PUSH_BUTTON_RELEASE)
{
        pinMode(pin, INPUT);
}

PushButton::PushButton(int pin, PushButtonHandler& handler) :
m_pin(pin), m_lastCheck(0), m_handlerType(HANDLER_CLASS), m_handler(handler),
m_handlerFunc(nullptr), m_lastEvent(PUSH_BUTTON_RELEASE)
{
    pinMode(pin, INPUT);
}

PushButton::PushButton(int pin, PushButtonHandlerFunc handler) :
m_pin(pin), m_lastCheck(0), m_handlerType(HANDLER_FUNC), m_handler(defaultHandler),
m_handlerFunc(handler), m_lastEvent(PUSH_BUTTON_RELEASE)
{
    pinMode(pin, INPUT);
}

PushButton::~PushButton()
{
    //
}

int
PushButton::getPin()
{
    return m_pin;
}

PushButtonEvent
PushButton::checkPin()
{
    uint32_t now = millis();

    /* */
    if (now - m_lastCheck > DEBOUNCE) {
        m_lastCheck = now;
        int read = digitalRead(m_pin);
        if (read == LOW) {
            if (m_lastEvent == PUSH_BUTTON_RELEASE || m_lastEvent == PUSH_BUTTON_RELEASE_EDGE) {
                m_lastEvent = PUSH_BUTTON_PRESS_EDGE;
            } else {
                m_lastEvent = PUSH_BUTTON_PRESS;
            }
        } else {
            if (m_lastEvent == PUSH_BUTTON_PRESS || m_lastEvent == PUSH_BUTTON_PRESS_EDGE) {
                m_lastEvent = PUSH_BUTTON_RELEASE_EDGE;
            } else {
                m_lastEvent = PUSH_BUTTON_RELEASE;
            }
        }
    /* */
    } else {
        if (m_lastEvent == PUSH_BUTTON_PRESS_EDGE) {
            m_lastEvent = PUSH_BUTTON_PRESS;
        } else if (m_lastEvent == PUSH_BUTTON_RELEASE_EDGE) {
            m_lastEvent = PUSH_BUTTON_RELEASE;
        }
    }

    if (m_handlerType == HANDLER_CLASS) {
        m_handler.handle(m_pin, m_lastEvent);
    } else if (m_handlerType == HANDLER_FUNC && m_handlerFunc != nullptr) {
        m_handlerFunc(m_pin, m_lastEvent);
    }

    return m_lastEvent;
}


/*******************************************************************************
 * RotaryButton
 */

RotaryButton::RotaryButton() :
PushButton(PA0, *this)
{
    //
}


void
RotaryButton::handle(int pin, PushButtonEvent event)
{
    static bool onOff = false;

    if (event == PUSH_BUTTON_PRESS_EDGE) {
        onOff = !onOff;

        if (onOff) {
            Serial.println("enterMenu");
            nav.enterMenu();
        } else {
            Serial.println("exitMenu");
            nav.exitMenu();
        }
    }
}


/*******************************************************************************
 * PowerButton
 */

PowerButton::PowerButton() :
PushButton(PC14, *this)
{
    //
}


void
PowerButton::handle(int pin, PushButtonEvent event)
{
    if (millis() > 2000) {

        switch (event) {

            case PUSH_BUTTON_RELEASE_EDGE: {
                Serial.println("shutdown!");
                bq25792.SDRV_CTRL(0x02);
                break;
            }

            default: {
                break;
            }

        }
    }
}

/*******************************************************************************
 * StartButton
 */

StartButton::StartButton() :
PushButton(PA10, *this)
{
    //
}

void StartButton::handle(int pin, PushButtonEvent event){

    static bool onOff = false;

    if (event == PUSH_BUTTON_PRESS_EDGE) {
        onOff = !onOff;
        if (onOff) {

            treatmentTimeHandler.reset();
            rtsys.start_treatment();
            settings.rfPowerOn = 1;
        } else {
            rtsys.stop_treatment();
            settings.rfPowerOn = 0;
        }
    }
    
}