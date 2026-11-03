#ifndef GLOBAL_H
#define GLOBAL_H
#include "button.h"
#include "traffic_light.h"
#include "seg7_display.h"
typedef enum { MODE_AUTOMATIC = 1, MODE_MANUAL, MODE_SETTING } fsm_mode_t;
enum { TRAFFIC_TIMER = 0, BUTTON_TIMER = 3, BLINK_TIMER = 4 };
typedef struct {
    fsm_mode_t mode;
    traffic_light_t straight, cross;
    uint16_t green_s, yellow_s;
    button_handle_t buttons[3];
    uint8_t events[3];
    uint8_t digits[4];
    seg7_display_t display;
} fsm_t;
extern fsm_t *app;
#endif
