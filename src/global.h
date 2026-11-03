#ifndef GLOBAL_H
#define GLOBAL_H
#include "button.h"
#include "traffic_light.h"
#include "seg7_display.h"
typedef enum {
    AUTO_GREEN, AUTO_YELLOW, AUTO_CROSS_GREEN, AUTO_CROSS_YELLOW,
    MAN_GREEN, MAN_YELLOW, MAN_CROSS_GREEN, MAN_CROSS_YELLOW,
    SET_RED, SET_YELLOW, SET_GREEN
} fsm_state_t;
enum { TRAFFIC_TIMER = 0, DISPLAY_TIMER = 1, INDICATOR_TIMER = 2, BLINK_TIMER = 3, COUNTDOWN_TIMER = 4 };
typedef struct {
    fsm_state_t state;
    traffic_light_t straight, cross;
    uint16_t green_s, yellow_s;
    button_handle_t buttons[3];
    uint8_t digits[4];
    seg7_display_t display;
} fsm_t;
extern fsm_t *app;
#endif
