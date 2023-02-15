#pragma once

#include <stdint.h>

enum PushButtonEvent {
    PUSH_BUTTON_NONE = 0,
    PUSH_BUTTON_PRESS,
    PUSH_BUTTON_PRESS_EDGE,
    PUSH_BUTTON_RELEASE,
    PUSH_BUTTON_RELEASE_EDGE
};

typedef void (*PushButtonHandlerFunc)(int pin, PushButtonEvent event);

class PushButtonHandler {
    public:
        virtual void    handle(int pin, PushButtonEvent event) = 0;

};

class PushButton {
    private:

        enum PushButtonHandlerType {
            HANDLER_CLASS,
            HANDLER_FUNC
        };

        static const int        DEBOUNCE;

        int                     m_pin;
        uint32_t                m_lastCheck;
        PushButtonHandlerType   m_handlerType;
        PushButtonHandler&      m_handler;
        PushButtonHandlerFunc   m_handlerFunc;

        PushButtonEvent         m_lastEvent;

    public:
                        PushButton(int pin);
                        PushButton(int pin, PushButtonHandler& handler);
                        PushButton(int pin, PushButtonHandlerFunc handler);
        virtual         ~PushButton();

        int             getPin();
        PushButtonEvent checkPin();

};

class RotaryButton : public PushButton, public PushButtonHandler {
    private:

    public:
        RotaryButton();
        void handle(int pin, PushButtonEvent event);
};

class PowerButton : public PushButton, public PushButtonHandler {
    private:

    public:
        PowerButton();
        void handle(int pin, PushButtonEvent event);
};

class StartButton : public PushButton, public PushButtonHandler {
    private:

    public:
        StartButton();
        void handle(int pin, PushButtonEvent event);

};