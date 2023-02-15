#include "u8g2custom.h"

#include <Arduino.h>
#include <SPI.h>

extern "C" uint8_t custom_byte_arduino_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    uint8_t *data;
    uint8_t internal_spi_mode;

    switch(msg) {

        case U8X8_MSG_BYTE_SEND:
        
            // 1.6.5 offers a block transfer, but the problem is, that the
            // buffer is overwritten with the incoming data
            // so it can not be used...
            // SPI.transfer((uint8_t *)arg_ptr, arg_int);
            
            data = (uint8_t *)arg_ptr;
            while( arg_int > 0 ) {
                SPI.transfer((uint8_t)*data);
                data++;
                arg_int--;
            }

            break;
        case U8X8_MSG_BYTE_INIT:
            if ( u8x8->bus_clock == 0 ) 	/* issue 769 */
                u8x8->bus_clock = u8x8->display_info->sck_clock_hz;

            /* disable chipselect */
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);

            /* no wait required here */

            /* for SPI: setup correct level of the clock signal */
            // removed, use SPI.begin() instead: pinMode(11, OUTPUT);
            // removed, use SPI.begin() instead: pinMode(13, OUTPUT);
            // removed, use SPI.begin() instead: digitalWrite(13, u8x8_GetSPIClockPhase(u8x8));

            /* setup hardware with SPI.begin() instead of previous digitalWrite() and pinMode() calls */


            /* issue #377 */
            /* issue #378: removed ESP8266 support, which is implemented differently */
            SPI.begin();

            break;
        
        case U8X8_MSG_BYTE_SET_DC:
            u8x8_gpio_SetDC(u8x8, arg_int);
            break;

        case U8X8_MSG_BYTE_START_TRANSFER:
            /* SPI mode has to be mapped to the mode of the current controller, at least Uno, Due, 101 have different SPI_MODEx values */
            internal_spi_mode =  0;
            switch(u8x8->display_info->spi_mode)
            {
            case 0: internal_spi_mode = SPI_MODE0; break;
            case 1: internal_spi_mode = SPI_MODE1; break;
            case 2: internal_spi_mode = SPI_MODE2; break;
            case 3: internal_spi_mode = SPI_MODE3; break;
            }

            SPI.beginTransaction(SPISettings(u8x8->bus_clock, MSBFIRST, internal_spi_mode));

            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);  
            u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
            break;

        case U8X8_MSG_BYTE_END_TRANSFER:      
            u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->pre_chip_disable_wait_ns, NULL);
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);

            SPI.endTransaction();

            break;
        default:
            return 0;
    }
    
    return 0;
}
