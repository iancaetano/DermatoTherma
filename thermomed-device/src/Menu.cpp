
#include "Menu.h"
#include "main.h"

#include "SoundSender.h"

#include "images/oled1.xbm"
#include "images/about.xbm"
#include "images/on.xbm"
#include "images/off.xbm"
#include "images/volume_1.xbm"
#include "images/volume_2.xbm"
#include "images/volume_3.xbm"
#include "images/volume_4.xbm"
#include "images/volume_mute.xbm"
#include "images/input_source.xbm"
#include "images/battery_0.xbm"
#include "images/battery_1.xbm"
#include "images/battery_2.xbm"
#include "images/battery_3.xbm"
#include "images/battery_4.xbm"
#include "images/battery_5.xbm"
#include "images/wheel_1.xbm"
#include "images/wheel_2.xbm"
#include "images/wheel_3.xbm"
#include "images/wheel_4.xbm"
#include "images/wheel_5.xbm"
#include "images/wheel_6.xbm"
#include "images/wheel_7.xbm"
#include "images/wheel_8.xbm"

static unsigned char *wheel[] {
    wheel_1_bits,
    wheel_2_bits,
    wheel_3_bits,
    wheel_4_bits,
    wheel_5_bits,
    wheel_6_bits,
    wheel_7_bits,
    wheel_8_bits
};

/*******************************************************************************
 * Selection
 */
/*
Selection::Selection() :
m_prevSelection(nullptr),
m_nextSelection(nullptr)
{

}
*/

Selection::Selection(Settings *settings, Selection *prevSelection, Selection *nextSelection) :
m_settings(settings),
m_prevSelection(prevSelection),
m_nextSelection(nextSelection)
{

}

/*
void
Selection::setPrevSelection(Selection *prevSelection)
{

}

void
Selection::setNextSelection(Selection *nextSelection)
{

}
*/

Selection *
Selection::getPrevSelection()
{
    return m_prevSelection;
}

Selection *
Selection::getNextSelection()
{
    return m_nextSelection;
}

void
Selection::setChange(int32_t change)
{

}

void
Selection::draw(bool pressed)
{
    char bufTempAct[8];
    char bufTempSet[8];
    char bufTimeHeat[8];
    char bufTimeTreatment[8];
    char bufPower[8];
    char bufFaultCount[8];

    u8g2.clearBuffer();

    snprintf(bufTempAct,        sizeof(bufTempAct),         "%2.1f",    m_settings->temperatureActual);
    snprintf(bufTempSet,        sizeof(bufTempSet),         "%2.1f",    m_settings->temperatureSetpoint);
    snprintf(bufTimeHeat,       sizeof(bufTimeHeat),        "%3d",      m_settings->timeHeating);
    snprintf(bufTimeTreatment,  sizeof(bufTimeTreatment),   "%3d",      m_settings->timeTreatment);
    snprintf(bufPower,          sizeof(bufPower),           "%2.1f W",  m_settings->power);
    snprintf(bufFaultCount,     sizeof(bufFaultCount),      "%3d",      m_settings->faultCount);

    u8g2.setDrawColor(1); 
    u8g2.drawXBM(0, 0, oled1_width, oled1_height, oled1_bits);
    
    //u8g2.setFont(u8g2_font_artosserif8_8n);
    //u8g2.setFont(u8g2_font_5x8_tn);
    //u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setFont(u8g2_font_profont11_tn);

    
    u8g2.drawStr( 46 - u8g2.getStrWidth(bufTempAct),       19, bufTempAct);
    u8g2.drawStr( 46 - u8g2.getStrWidth(bufTempSet),       29, bufTempSet);
    u8g2.drawStr(110 - u8g2.getStrWidth(bufTimeHeat),      19, bufTimeHeat);
    u8g2.drawStr(110 - u8g2.getStrWidth(bufTimeTreatment), 29, bufTimeTreatment);
    u8g2.drawStr(103 - u8g2.getStrWidth(bufPower),         50, bufPower);

    const static int volumeX = 50;
    const static int volumeY = 33;
    const static int volumeW = 11;
    const static int volumeH = 18;
    switch (m_settings->volume) {
        case 0:     u8g2.drawXBM(volumeX, volumeY, volumeW, volumeH, volume_mute_bits); break;
        case 1:     u8g2.drawXBM(volumeX, volumeY, volumeW, volumeH, volume_1_bits);    break;
        case 2:     u8g2.drawXBM(volumeX, volumeY, volumeW, volumeH, volume_2_bits);    break;
        case 3:     u8g2.drawXBM(volumeX, volumeY, volumeW, volumeH, volume_3_bits);    break;
        case 4:     u8g2.drawXBM(volumeX, volumeY, volumeW, volumeH, volume_4_bits);    break;
        default:                                                                        break;
    }

    if (m_settings->inputSourceAttached) {
        u8g2.drawXBM(98, 1, 10, 8, input_source_bits);
    }

    const static int batteryX = 109;
    const static int batteryY = 1;
    const static int batteryW = 18;
    const static int batteryH = 8;
    switch (m_settings->batteryLevel) {
        case 0:     u8g2.drawXBM(batteryX, batteryY, batteryW, batteryH, battery_0_bits);   break;
        case 1:     u8g2.drawXBM(batteryX, batteryY, batteryW, batteryH, battery_1_bits);   break;
        case 2:     u8g2.drawXBM(batteryX, batteryY, batteryW, batteryH, battery_2_bits);   break;
        case 3:     u8g2.drawXBM(batteryX, batteryY, batteryW, batteryH, battery_3_bits);   break;
        case 4:     u8g2.drawXBM(batteryX, batteryY, batteryW, batteryH, battery_4_bits);   break;
        case 5:     u8g2.drawXBM(batteryX, batteryY, batteryW, batteryH, battery_5_bits);   break;
        default:                                                                            break;
    }

    if (m_settings->rfPowerOn) {
        u8g2.drawXBM(106, 33, 15, 9, on_bits);
    } else {
        u8g2.drawXBM(106, 33, 15, 9, off_bits);
    }

    u8g2.drawStr(110 - u8g2.getStrWidth(bufFaultCount), 62, bufFaultCount);
    u8g2.drawXBM(118, 54, 9, 9, wheel[m_settings->faultWheelCounter]);
}

/*******************************************************************************
 * NullSelection
 */
NullSelection::NullSelection(Settings *settings, Selection *prevSelection, Selection *nextSelection) :
Selection(settings, prevSelection, nextSelection)
{

}

void
NullSelection::draw(bool pressed)
{
    Selection::draw(pressed);

    u8g2.sendBuffer();
}

/*******************************************************************************
 * TemperatureSelection
 */

const float TemperatureSelection::TEMP_MIN = 35.0f;
const float TemperatureSelection::TEMP_MAX = 55.0f;

TemperatureSelection::TemperatureSelection(Settings *settings, Selection *prevSelection, Selection *nextSelection) :
Selection(settings, prevSelection, nextSelection)
{

}

void
TemperatureSelection::draw(bool pressed)
{
    Selection::draw(pressed);

    u8g2.setDrawColor(2); 
    u8g2.drawBox(22, 21, 60 - 22, 30 - 21);
    
    u8g2.sendBuffer();
}

void
TemperatureSelection::setChange(int32_t change)
{
    if (change > 0 && m_settings->temperatureSetpoint < TEMP_MAX) {
        m_settings->temperatureSetpoint += 1.0f;
    } else if (change < 0 && m_settings->temperatureSetpoint > TEMP_MIN) {
        m_settings->temperatureSetpoint -= 1.0f;
    }
    rtsys.set_temp_sp(m_settings->temperatureSetpoint);
}

/*******************************************************************************
 * VolumeSelection
 */

VolumeSelection::VolumeSelection(Settings *settings, Selection *prevSelection, Selection *nextSelection) :
Selection(settings, prevSelection, nextSelection)
{

}

void
VolumeSelection::draw(bool pressed)
{
    Selection::draw(pressed);

    u8g2.setDrawColor(2); 
    u8g2.drawBox(0, 32, 63 - 0, 52 - 32);
    u8g2.sendBuffer();
}

void 
VolumeSelection::setChange(int32_t change)
{
    if (change > 0 && m_settings->volume < 4) {
        m_settings->volume++;
    } else if (change < 0 && m_settings->volume > 0) {
        m_settings->volume--;
    }
}

/*******************************************************************************
 * AboutSelection
 */

AboutSelection::AboutSelection(Settings *settings, Selection *prevSelection, Selection *nextSelection) :
Selection(settings, prevSelection, nextSelection)
{

}

void
AboutSelection::draw(bool pressed)
{
    if (pressed) {
        u8g2.clearBuffer();
        u8g2.drawXBM(0, 0, about_width, about_height, about_bits);

    } else {
        Selection::draw(pressed);

        u8g2.setDrawColor(2); 
        u8g2.drawBox(0, 53, 63 - 0, 64 - 53);
    }

    u8g2.sendBuffer();
}

void
AboutSelection::setChange(int32_t change)
{

}

/*******************************************************************************
 * Navigation
 */

Navigation::Navigation(Settings *settings) :
m_pressed(false),
m_currentSelection(&m_nullSelection),
m_nullSelection(settings, nullptr, &m_temperatureSelection),
m_temperatureSelection(settings, nullptr, &m_volumeSelection),
m_volumeSelection(settings, &m_temperatureSelection, &m_aboutSelection),
m_aboutSelection(settings, &m_volumeSelection, nullptr)
{

}

void
Navigation::enterMenu()
{
    m_pressed = true;
}

void
Navigation::exitMenu()
{
    m_pressed = false;
}

void
Navigation::setChange(int32_t change)
{
    Selection *selection = nullptr;
    if (m_pressed) {
        /* only when changed */
        if (change < 0 || change > 0) {
            m_currentSelection->setChange(change);
        }
    } else {
        if (change < 0) {
            selection = m_currentSelection->getPrevSelection();
        } else if (change > 0) {
            selection = m_currentSelection->getNextSelection();
        }

        if (selection != nullptr) {
            m_currentSelection = selection;
        }
    }
}

void
Navigation::draw()
{
    m_currentSelection->draw(m_pressed);
}

Navigation nav(&settings);