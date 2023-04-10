#include "Arduino.h"
#include "MLX90614.h"
#include <Adafruit_I2CDevice.h>
#include "SoftwareTimer.h"


#pragma once


class TPS:MLX90614{
    private:

        static const float GAIN_CONTROL_MIN;
        static const float GAIN_CONTROL_MAX;
    public:
        bool begin(uint8_t addr, TwoWire *wire = &Wire);
        Adafruit_I2CDevice *DCDC_i2c_dev = NULL;
        uint8_t read8(uint8_t a);
        uint16_t read16(uint8_t a);
        void write8(uint8_t a, uint8_t v);
        void write16(uint8_t a, uint8_t v);
        void beginDCDC();
        void set_DCDC_output_hw(byte v);
        byte readStat();
    private:

};

class Rf_hardware {

        HAL_StatusTypeDef test;
        
        
    public:

        Rf_hardware();
        void begin();

        /* HW interface */
        void pke_enable();
        void pke_disable();
        float read_adc_VDC();
        float read_adc_IDC();
        float read_adc_phi();
        void writeToWireOne(byte ADDR,byte REG, byte value);
        void writeDCDCOutToWireOne(byte ADDR,byte REG, byte value);
        byte readfromWireOne(byte ADDR,byte REG);

    private:

        bool beginDCDC_I2C(uint8_t addr, TwoWire *wire);
        void MX_GPIO_Init(void);
        void MX_DMA_Init(void);
        void MX_ADC1_Init(void);
        void MX_ADC2_Init(void);
        void MX_TIM2_Init(void);
        void DMA1_Channel1_IRQHandler(void);
        void DMA1_Channel2_IRQHandler(void);
   
    
};


