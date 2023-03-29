
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
        float read_adc_VDC();
        float read_adc_IDC();
        float read_adc_phi();
        void set_DCDC_output(byte v);
        byte readStat();

    private:
        void beginDCDC();
        void beginTimer2();
        void beginADC1();
        void beginGpio();
        void ADC_Select_CH4();
        void ADC_Select_CH8();
        void ADC_Select_CH9();
        
    
};
