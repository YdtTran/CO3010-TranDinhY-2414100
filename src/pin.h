#ifndef PIN_H
#define PIN_H

#include "stm32f1xx_hal.h"

/* U1: STM32F103C8, LQFP48. D15 cathode on PA5 (package pin 15). */
#define INDICATOR_PORT GPIOA
#define INDICATOR GPIO_PIN_5
#define INDICATOR_ACTIVE_STATE GPIO_PIN_RESET

/* SEG[0..6] = A..G, common anode, active LOW.
 * PA9..PA15: package pins 30, 31, 32, 33, 34, 37, 38.
 * PA13..PA15 require debug disabled (SYS No Debug in CO3010.ioc).
 */
#define SEG_PORT GPIOA
static const uint32_t SEG[7] = {
    GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12,
    GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};
#define SEG_ACTIVE_STATE GPIO_PIN_RESET

/* Traffic LEDs: indices 0/1/2 = green/yellow/red, active HIGH.
 * L drives left/right groups: pins 18, 19, 20.
 * T drives top/bottom groups: pins 39, 40, 41.
 */
#define L_PORT GPIOB
static const uint32_t L[3] = {
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2};
#define T_PORT GPIOB
static const uint32_t T[3] = {
    GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5};
#define TRAFFIC_LED_ACTIVE_STATE GPIO_PIN_SET

/* BTN[0..2]: package pins 26, 27, 28; pull-up inputs, switches to GND. */
#define BTN_PORT GPIOB
static const uint32_t BTN[3] = {
    GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};
#define BTN_ACTIVE_STATE GPIO_PIN_RESET
#define BTN_PULL GPIO_PULLUP

/* EN0..EN3 = PB6..PB9 (package pins 42, 43, 45, 46).
 * Each PNP driver independently enables one digit, active LOW.
 */
#define EN_PORT GPIOB
static const uint32_t EN[4] = {
    GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_9};
#define EN_ACTIVE_STATE GPIO_PIN_RESET

#endif /* PIN_H */
