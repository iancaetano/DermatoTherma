#include "HandsetCommunication.h"
#include "TreatmentTimeHandler.h"

#include <Arduino.h>

/* Framer */
#include "pkt.h"
#include "frm.h"

static byte         rxbuf[64];
frm_contex_t        frameContext;
//uint32_t            miliPktTemperature_notActive = 0;
//uint32_t            miliPktTemperatureThreshold_notActive = 175;

/**
 * 
 */
static void
txfun(void *par, byte b)
{
    HardwareSerial *serial = static_cast<HardwareSerial *>(par);
    serial->write(b);
}

void
HandsetCommunication::begin()
{
    frm_init_encode_context(&frameContext, txfun, &Serial3);
    frm_init_decode_context(&frameContext);
}

void
HandsetCommunication::loop(Rt_system::Rt_out &out)
{
    int ch;

    while ((ch = Serial3.read()) >= 0) {
        int n_dec;
        n_dec = frm_decode(&frameContext, rxbuf, sizeof(rxbuf), static_cast<byte>(ch));

        /* received full frame */
        if (n_dec > 0) {
            
            pkt_generic_t *pkt = reinterpret_cast<pkt_generic_t *>(rxbuf);
            pkt_temperature_t *pkt_temp;
            pkt_keypress_t *pkt_keypress;
            pkt_led_t pkt_led;

            wheel_increment();

            switch (pkt->type) {
                case PKT_TYPE_TEMPERATURE:
                    pkt_temp = reinterpret_cast<pkt_temperature_t *>(rxbuf);

                    /* update settings */
                    settings.temperatureActual = pkt_temp->temperature;

                    /* save timestamp */
                    //miliPktTemperature_notActive = millis();

                    /* update RT system */
                    rtsys.set_temp_pv(pkt_temp->temperature);

                    break;

                case PKT_TYPE_KEYPRESS:
                    pkt_keypress = reinterpret_cast<pkt_keypress_t *>(rxbuf);

                    /* update settings */
                    settings.rfPowerOn = pkt_keypress->isPressed;

                    /* acknowledge key press */ 
                    pkt_led.type = PKT_TYPE_LED;
                    pkt_led.isOn = pkt_keypress->isPressed;
                    frm_encode_message(&frameContext, reinterpret_cast<const byte *>(&pkt_led), sizeof(pkt_led));
                    Serial3.flush();

                    /* update RT system */
                    if (pkt_keypress->isPressed) {
                        treatmentTimeHandler.reset();
                        rtsys.start_treatment();
                    } else {
                        rtsys.stop_treatment();
                    }
                    
                    break;

                default:
                    break;
            }
            
        }
    }

    if (out.rt_status == Rt_system::Rt_status::error) {
        pkt_led_t pkt_led;

       /* update settings */
        settings.rfPowerOn = false;

       /* acknowledge error */ 
        pkt_led.type = PKT_TYPE_LED;
        pkt_led.isOn = false;
        frm_encode_message(&frameContext, reinterpret_cast<const byte *>(&pkt_led), sizeof(pkt_led));
        Serial3.flush();

        /* update RT system */
        rtsys.stop_treatment();
    }
}

void 
HandsetCommunication::wheel_increment()
{
    settings.faultWheelCounter++;
    if (settings.faultWheelCounter >= 8) {
        settings.faultWheelCounter = 0;
    } 

}

void
HandsetCommunication::handle()
{
    //uint32_t now = millis();

    /* count 1 sec. after boot 
    if (now > 1000) {

         count up only if there is no recent packet received 
        if (now > miliPktTemperature_notActive + miliPktTemperatureThreshold_notActive) {
            settings.faultCount++;
        }
    }
*/
}
HandsetCommunication comm;
