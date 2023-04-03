#include "Arduino.h"
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
        float read_adc_VDC();
        float read_adc_IDC();
        float read_adc_phi();
        void set_DCDC_output_hw(byte v);
        byte readStat();
        void writeToWireOne(byte ADDR,byte REG, byte value);
        void writeDCDCOutToWireOne(byte ADDR,byte REG, byte value);
        byte readfromWireOne(byte ADDR,byte REG);

    private:
        void beginDCDC();
        void MX_GPIO_Init(void);
        void MX_DMA_Init(void);
        void MX_ADC1_Init(void);
        void MX_ADC2_Init(void);
        void MX_TIM2_Init(void);
        void DMA1_Channel1_IRQHandler(void);
        void DMA1_Channel2_IRQHandler(void);

        
    
};
