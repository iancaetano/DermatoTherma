
#pragma once

class Rf_hardware {
    private:

        static const float GAIN_CONTROL_MIN;
        static const float GAIN_CONTROL_MAX;
        
    public:
        Rf_hardware();
        void begin();

        /* HW interface */
        void pke_enable();
        void pke_disable();
        static void set_debug_pin_state(bool state);
        float read_adc_voltage();
        void set_dac_voltage(float v);

    private:
        void beginTimer2();
        void beginADC1();
        void beginDAC1();
        void beginGpio();
    
};
