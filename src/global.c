#include "global.h"
fsm_state_t state = AUTO_GREEN;

traffic_light_t light_system = {
    .port = GPIOB,
    .gpio = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2,
             GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5},
    .straight_color = LED_OFF,
    .cross_color = LED_OFF};

uint16_t green_s = 2, yellow_s = 3;

button_handle_t buttons[3] = {
    {.port = BTN_PORT, .gpio = GPIO_PIN_13, .active_level = BTN_ACTIVE_STATE,
     .buffer = {GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET}},
    {.port = BTN_PORT, .gpio = GPIO_PIN_14, .active_level = BTN_ACTIVE_STATE,
     .buffer = {GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET}},
    {.port = BTN_PORT, .gpio = GPIO_PIN_15, .active_level = BTN_ACTIVE_STATE,
     .buffer = {GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET}}};

uint8_t draft = 0, blink_on = 0;
uint8_t digits[4] = {0, 0, 0, 0};
seg7_display_t seg7_display = {
    .segs = {GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12,
             GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, 0},
    .en_pin = EN,
    .seg_port = SEG_PORT,
    .en_port = EN_PORT,
    .buffer = digits,
    .num = 4,
    .com = SEG7_COMMON_ANODE,
    .current_digit = 0};
