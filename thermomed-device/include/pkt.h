#pragma once

#include <stdint.h>

typedef enum {
    PKT_TYPE_ERROR = 0,
    PKT_TYPE_TEMPERATURE,
    PKT_TYPE_KEYPRESS,
    PKT_TYPE_LED
} pkt_type_t;

#pragma pack(push)
#pragma pack(1)

typedef struct {
    uint32_t type;
} pkt_generic_t;

typedef struct {
    uint32_t type; // = PKT_TYPE_TEMPERATURE
    double temperature;
} pkt_temperature_t;

typedef struct {
    uint32_t type; // = PKT_TYPE_KEYPRESS
    bool     isPressed;
} pkt_keypress_t;

typedef struct {
    uint32_t type; // = PKT_TYPE_LED
    bool     isOn;
} pkt_led_t;

#pragma pack(pop)
