
#pragma once

#include <HardwareTimer.h>
#include <vector>

using namespace std;

class SoftwareTimerHandler {

    public:
        virtual void handle() = 0;
};

class SoftwareTimer {

    public:
        typedef uint32_t Period;

        typedef enum {
            ONE_SHOT,
            AUTO_RELOAD
        } Mode;

        typedef function<void(void)> SoftwareTimerHandlerFunc;

    private:
    
        enum SoftwareTimerHandlerType {
            HANDLER_CLASS,
            HANDLER_FUNC
        };

        class Element {
            public:
                Period                      m_period;
                Mode                        m_mode;
                SoftwareTimerHandlerType    m_handlerType;
                SoftwareTimerHandler&       m_handler;
                SoftwareTimerHandlerFunc    m_handlerFunc;
                uint32_t                    m_timeout;
                bool                        m_markedForDeletion;
            
            public:
                Element(Period period, Mode mode, SoftwareTimerHandler& handler);
                Element(Period period, Mode mode, SoftwareTimerHandlerFunc& handlerFunc);
        };

    private:
        HardwareTimer   m_timer;
        vector<Element> m_container;

    public:
        SoftwareTimer();
        void begin();
        void addHandler(Period period, Mode mode, SoftwareTimerHandler& handlerKlass);
        void addHandler(Period period, Mode mode, SoftwareTimerHandlerFunc handlerFunc);

    private:
        void interrupt();
};
