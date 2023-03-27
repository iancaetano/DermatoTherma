
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
        void set_opamp_on_input_float();
        void set_opamp_on_input_pull_low();
        static void set_debug_pin_state(bool state);
        float read_adc_VDC();
        void set_DCDC_output(byte v);

    private:
        void beginDCDC();
        void beginTimer2();
        void beginADC1();
        void beginGpio();
    
};
