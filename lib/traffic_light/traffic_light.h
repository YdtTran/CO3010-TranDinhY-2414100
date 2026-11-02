#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include "stm32f1xx_hal.h"

typedef enum
{
    LED_GREEN,
    LED_YELLOW,
    LED_RED
} light_state_t;

typedef struct
{
    GPIO_TypeDef *port;
    uint32_t gpio[3]; /* Indexed by light_state_t; active-high outputs. */
    light_state_t state;
    uint8_t timer_index;
    uint16_t remaining_s;
    uint8_t timeout_flag; /* Set when remaining_s reaches zero. */
} traffic_light_t;

typedef struct
{
    traffic_light_t straight;
    traffic_light_t cross;
    uint16_t green_s;
    uint16_t yellow_s;
    uint8_t running;
} traffic_system_t;

/* Populate ports, pins, distinct timer indices and positive durations first.
 * Returns 0 without changing outputs on invalid configuration.
 * Initialize before starting the software-timer interrupt.
 */
uint8_t TRAFFIC_SYSTEM_Init(traffic_system_t *system);

/* Restore straight green/cross red and initial countdowns, then leave paused.
 * Keeps GPIO bindings, timer indices and configured durations.
 * Call only after successful Init, with interrupts disabled.
 */
void TRAFFIC_SYSTEM_Reset(traffic_system_t *system);

/* Start an initialized/reset system with fresh 1 s timers; no-op if running.
 * Call with interrupts disabled, as for Reset.
 */
void TRAFFIC_SYSTEM_Start(traffic_system_t *system);

/* Call frequently from the main loop with interrupts disabled so both timers
 * are consumed and rearmed together. Keep the main loop nonblocking.
 * The interrupt interval must divide 1000 ms (e.g. 1, 10 or 100 ms).
 * Do not modify the system or consume its timer flags while it is running.
 */
void TRAFFIC_SYSTEM_Run(traffic_system_t *system);

#endif // TRAFFIC_LIGHT_H
