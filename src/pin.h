#ifndef PIN_H
#define PIN_H

#include "stm32f1xx_hal.h"

/* U1: STM32F103C8, LQFP48. D15 cathode on PA5 (package pin 15). */
#define INDICATOR_PORT GPIOA
#define INDICATOR GPIO_PIN_5
#define INDICATOR_ACTIVE_STATE GPIO_PIN_RESET

/* SEG[0..6] = A..G, common anode, active LOW.
 * PA8..PA14: package pins 29, 30, 31, 32, 33, 34, 37.
 * PA13..PA14 require debug disabled (SYS No Debug in CO3010.ioc).
 */
#define SEG_PORT GPIOA
static const uint32_t SEG[7] = {
    GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14};
#define SEG_ACTIVE_STATE GPIO_PIN_RESET

/* Traffic LEDs: array indices follow schematic labels, active HIGH.
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

/* Chosen GPIO assignments: EN0 = PA0 (pin 10), EN1 = PA1 (pin 11).
 * EN0 drives Q3/R39 and Q1/R13: DIGIT0 and DIGIT2.
 * EN1 drives Q4/R40 and Q2/R14: DIGIT1 and DIGIT3.
 * PNP drivers source +3V3 through 1-kohm base resistors, active LOW.
 * The paired digits share enables; they cannot be selected independently.
 */
#define EN_PORT GPIOA
static const uint32_t EN[2] = {
    GPIO_PIN_0, GPIO_PIN_1};
static const uint32_t DIGIT_EN[4] = {0U, 1U, 0U, 1U};
#define EN_ACTIVE_STATE GPIO_PIN_RESET

#endif /* PIN_H */
