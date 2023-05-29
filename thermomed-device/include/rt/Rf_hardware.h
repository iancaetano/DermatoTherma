
#pragma once

class Rf_hardware {
    private:

        static const float GAIN_CONTROL_MIN;
        static const float GAIN_CONTROL_MAX;
        
    public:
        float value;
        Rf_hardware();
        void begin();

        /* HW interface */
        void pke_enable();
        void pke_disable();
        float read_RFVDC_voltage();
        void set_dac_voltage(float v);
        float read_adc_VDC();
        float read_adc_IDC();
        float read_adc_phi();

    private:
        void beginTimer2();
        void beginADC1();
        void beginDAC1();
        void beginGpio();
        void beginDMA();



    
};
