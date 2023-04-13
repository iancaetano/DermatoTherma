#include "main.h"

// framework-arduinoststm32/variants/STM32G4xx/G473R(B-C-E)T_G474R(B-C-E)T_G483RET_G484RET
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "Settings.h"
#include "Console.h"
//#include "HandsetCommunication.h"
#include "RotaryEncoder.h"
#include "PushButton.h"
#include "BQ25792.h"
#include "SoftwareTimer.h"
#include "SoundSender.h"
#include "Menu.h"
#include "TreatmentTimeHandler.h"
#include "MLX90614.h"
#include "Handset.h"


#define DCDC_ADDR                   0x74

#define SCL_wireone                 PA9
#define SDA_wireone                 PA8

/* OLED Driver */
U8G2_SSD1322_NHD_128X64_CUSTOM u8g2(U8G2_R0, /* cs=*/ PC15, /* dc=*/ PB0, /* reset=*/ PB1);

Settings            settings;
RotaryEncoder       encoder;
RotaryButton        rotaryButton;
PowerButton         powerButton;
SoftwareTimer       swTimer;

Rt_system           rtsys;
MLX90614            TempSensor;
Rf_hardware         DCDC;
HandsetClass        Handset;
TPS                 DCDC_TPS;

TwoWire wireOne = TwoWire();






const static int    baudRate = 2000000;



/**
 * 
 */
void
setup() {


    Serial.begin(baudRate);
    //Serial3.begin(baudRate);

    Serial.println("setup start");

    // comm.begin();
    // Serial.println("comm done");
    
    Wire.begin();
    Serial.println("wire done");

    wireOne.setSDA(SDA_wireone);
    wireOne.setSCL(SCL_wireone);
    wireOne.begin();
    Serial.println("wireOne done");
    
    bq25792.begin();
    Serial.println("battery done");

    u8g2.begin();
    Serial.println("display done");

    encoder.begin();
    encoder.start();
    Serial.println("encoder done");

    Handset.init();
    Serial.println("Handset done");

    /*
    #define MCO_Pin GPIO_PIN_8
    #define MCO_GPIO_Port GPIOA

    GPIO_InitTypeDef        GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin                     = MCO_Pin;
    GPIO_InitStruct.Mode                    = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull                    = GPIO_NOPULL;
    GPIO_InitStruct.Speed                   = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate               = GPIO_AF0_MCO;

    HAL_GPIO_Init(MCO_GPIO_Port, &GPIO_InitStruct);
    HAL_RCC_MCOConfig(RCC_MCO1,RCC_MCO1SOURCE_PLLCLK,RCC_MCODIV_2);

*/
    DCDC_TPS.begin(DCDC_ADDR, &wireOne);
    rtsys.init();
    rtsys.set_temp_sp(settings.temperatureSetpoint);
    Serial.println("rtsys done");

    treatmentTimeHandler.reset();

    swTimer.addHandler(250, SoftwareTimer::AUTO_RELOAD, bq25792);
    //swTimer.addHandler(50, SoftwareTimer::AUTO_RELOAD, comm);
    swTimer.addHandler(20, SoftwareTimer::AUTO_RELOAD, Handset);
    swTimer.addHandler(100, SoftwareTimer::AUTO_RELOAD, soundSender);
    swTimer.addHandler(1000, SoftwareTimer::AUTO_RELOAD, treatmentTimeHandler);
    swTimer.begin();
    Serial.println("software timer done");

    pinMode(PB7, OUTPUT);

    Serial.println("setup done");

}


/**
 * 
 */
void
loop()
{
    struct Rt_system::Rt_out out;
    rtsys.get_status(out);


    if(rtsys.dcdcStartFlag){
        DCDC_TPS.beginDCDC();
        DCDC_TPS.set_DCDC_output_hw(0);
        rtsys.dcdcStartFlag = 0;
    }
    if(){

    }
    if(){

    }
    if(){
        
    }



    settings.power = out.power_estimate;
    console.loop();
    bq25792.loop();
    treatmentTimeHandler.loop(out);
    soundSender.loop(out);
    Handset.loop();
    // comm.loop(out);
    rotaryButton.checkPin();
    powerButton.checkPin();
    nav.setChange(encoder.difference());
    
    nav.draw();
    

}