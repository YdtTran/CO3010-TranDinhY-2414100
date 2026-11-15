#ifndef GLOBAL_H
#define GLOBAL_H
#include "pin.h"
#include "button.h"
#include "traffic_light.h"
#include "seg7_display.h"
typedef enum
{
    AUTO_GREEN,
    AUTO_YELLOW,
    AUTO_CROSS_GREEN,
    AUTO_CROSS_YELLOW,
    MAN_GREEN,
    MAN_YELLOW,
    MAN_CROSS_GREEN,
    MAN_CROSS_YELLOW,
    SET_RED,
    SET_YELLOW,
    SET_GREEN
} fsm_state_t;
enum
{
    TRAFFIC_TIMER = 0,
    DISPLAY_TIMER = 2,
    INDICATOR_TIMER = 1,
    BLINK_TIMER = 3,
    COUNTDOWN_TIMER = 4
};
extern fsm_state_t state;
extern traffic_light_t light_system;
extern uint16_t green_s, yellow_s;
extern button_handle_t buttons[3];
extern uint8_t draft, blink_on;
extern uint8_t digits[4];
extern seg7_display_t seg7_display;
#endif
