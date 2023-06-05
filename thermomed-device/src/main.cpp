#include "main.h"

// framework-arduinoststm32/variants/STM32G4xx/G473R(B-C-E)T_G474R(B-C-E)T_G483RET_G484RET
#include <Arduino.h>
#include <SPI.h>

#include "Settings.h"
#include "Console.h"
#include "RotaryEncoder.h"
#include "PushButton.h"
#include "BQ25792.h"
#include "SoftwareTimer.h"
#include "Sound.h"
#include "Menu.h"
#include "TreatmentTimeHandler.h"
#include "MLX90614.h"
#include "Handset.h"
#include "rt/callbackFlag.h"

/* OLED Driver */
U8G2_SSD1322_NHD_128X64_CUSTOM u8g2(U8G2_R0, /* cs=*/ PC15, /* dc=*/ PB0, /* reset=*/ PB1);

Settings            settings;
RotaryEncoder       encoder;
RotaryButton        rotaryButton;
PowerButton         powerButton;
SoftwareTimer       swTimer;

Rt_system           rtsys;
MLX90614            TempSensor;
HandsetClass        Handset;
SoundSender         Sound;



const static int    baudRate = 2000000;
int RTcallbackFlag = 0;


/**
 * 
 */
void
setup() {
    Serial.begin(baudRate);
    Serial3.begin(baudRate);

    Serial.println("setup start");

    // comm.begin();
    // Serial.println("comm done");
    
    Wire.begin();
    Serial.println("wire done");
    
    bq25792.begin();
    Serial.println("battery done");

    u8g2.begin();
    Serial.println("display done");

    encoder.begin();
    encoder.start();
    Serial.println("encoder done");

    Handset.init();
    Serial.println("Handset done");

    rtsys.init();
    rtsys.set_temp_sp(settings.temperatureSetpoint);
    Serial.println("rtsys done");

 

    treatmentTimeHandler.reset();

    swTimer.addHandler(250, SoftwareTimer::AUTO_RELOAD, bq25792);
    swTimer.addHandler(20, SoftwareTimer::AUTO_RELOAD, Handset);
    swTimer.addHandler(1000, SoftwareTimer::AUTO_RELOAD, treatmentTimeHandler);
    swTimer.begin();
    Serial.println("software timer done");

    pinMode(PB7, OUTPUT);
    Serial.println("setup done");

    Sound.begin(); //call after rf Hardware
}


/**
 * 
 */
void
loop()
{
    if (RTcallbackFlag){
        rtsys.rt_callback();
        RTcallbackFlag =0;
    }
    struct Rt_system::Rt_out out;
    rtsys.get_status(out);
    settings.power = out.power_estimate;
    console.loop();
    bq25792.loop();
    treatmentTimeHandler.loop(out);
    Handset.loop();
    rotaryButton.checkPin();
    powerButton.checkPin();
    nav.setChange(encoder.difference());
    nav.draw();
    

}