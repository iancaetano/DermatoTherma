#include "SoundSender.h"
#include "SerialUtils.h"

#include <Arduino.h>
#include <SPI.h>

/*              MOSI, MISO, SCLK, NSS */
SPIClass ddsSPI(PB15, PB14, PB13, PB12);

const float SoundSender::OFFSET = 1.f;

SoundSender::SoundSender()
{
    m_timeout = false;
}

void
SoundSender::begin()
{
    ddsSPI.begin();
}

/**
 * 
 * @param enable
 * @param volume 0 = mute, 4 = full amplitude
 * @param temperature in centigrad
 */
void
SoundSender::send(bool enable, uint8_t volume, uint16_t temperature)
{
    byte buf[4];

    temperature *= 10;

    buf[0] = (byte) enable;
    buf[1] = (byte) volume;
    buf[2] = (byte) ((temperature & 0xff00) >> 8);
    buf[3] = (byte) ((temperature & 0x00ff) >> 0);

    ddsSPI.beginTransaction(SPISettings(4000000L, MSBFIRST, SPI_MODE0));
    ddsSPI.transfer(buf, sizeof(buf));
    ddsSPI.endTransaction();
}


void
SoundSender::loop(Rt_system::Rt_out &out)
{
    bool enable = false;

    if (m_timeout) {
        m_timeout = false;
        if (out.rt_status == Rt_system::Rt_status::treating &&
            settings.temperatureActual < (settings.temperatureSetpoint - OFFSET)) {
                enable = true;
        }
        soundSender.send(enable, settings.volume, settings.temperatureActual);
    }
}

/* SoftwareTimerHandler in Interrupt mode */
void
SoundSender::handle()
{
    m_timeout = true;
}

SoundSender soundSender;
