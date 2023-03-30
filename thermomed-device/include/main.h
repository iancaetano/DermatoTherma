#pragma once

/* u8g2 OLED library */
#include <U8g2lib.h>
#include "u8g2custom.h"

/* Real-time System */
#include "rt/Rt_system.h"

/* Settings */
#include "Settings.h"

/*Sensor*/
#include  "MLX90614.h"

//Button
#include "PushButton.h"

#include "Handset.h"

extern U8G2_SSD1322_NHD_128X64_CUSTOM   u8g2;
extern Rt_system                        rtsys;
extern Settings                         settings;
extern MLX90614                         TempSensor;

extern HandsetClass                     Handset;

