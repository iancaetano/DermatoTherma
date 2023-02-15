
#include "Console.h"
#include "BQ25792.h"
#include <Arduino.h>


void
Console::loop()
{
    int ch;

    if ((ch = Serial.read()) >= 0) {
        switch (ch) {

            case 'r': {
                bq25792.printRegister();
                break;
            }

            case '1': {
                bq25792.WATCHDOG(0x00);
                bq25792.ADC_EN(0x01);
                bq25792.EN_IBAT(0x01);
                bq25792.EN_EXTILIM(0x00); // disable ILIM_HIZ
                bq25792.SFET_PRESENT(0x01);
                bq25792.SDRV_DLY(0x01);
                bq25792.WKUP_DLY(0x01);
                break;
            }

            case 's': {
                bq25792.scanDevices();
                break;
            }

            case 'x': {
                bq25792.SDRV_CTRL(0x02);
                break;
            }

            case 'b': {
                uint16_t vbat_adc;
                int result = bq25792.get_VBAT_ADC(&vbat_adc);
                Serial.print("VBAT=");
                if (result) {
                    Serial.println(vbat_adc);
                } else {
                    Serial.println("<error>");
                }
                break;
            }

            default:
                break;
        }
    }
}

Console console;
