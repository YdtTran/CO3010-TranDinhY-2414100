#include "traffic_light.h"
#include "software_timer.h"
#include <stddef.h>

_Static_assert(1000U % TICK_INTERVAL_MS == 0U,
               "Traffic countdown requires an integral number of ticks per second");

static uint8_t light_valid(const traffic_light_t *light)
{
    if (light->port == NULL || light->timer_index >= SOFT_TIMER_NUM_MAX)
        return 0;

    uint32_t used = 0U;
    for (unsigned i = 0; i < 3U; ++i)
    {
        uint32_t pin = light->gpio[i];
        if (pin == 0U || pin > GPIO_PIN_15 || (pin & (pin - 1U)) || (used & pin))
            return 0;
        used |= pin;
    }
    return 1;
}

static void light_off(const traffic_light_t *light)
{
    HAL_GPIO_WritePin(light->port,
                      light->gpio[0] | light->gpio[1] | light->gpio[2], GPIO_PIN_RESET);
}

static void system_display(const traffic_system_t *system)
{
    /* Clear both old outputs before enabling either new color. */
    light_off(&system->straight);
    light_off(&system->cross);
    HAL_GPIO_WritePin(system->straight.port,
                      system->straight.gpio[system->straight.state], GPIO_PIN_SET);
    HAL_GPIO_WritePin(system->cross.port,
                      system->cross.gpio[system->cross.state], GPIO_PIN_SET);
}

uint8_t TRAFFIC_SYSTEM_Init(traffic_system_t *system)
{
    if (system == NULL || !light_valid(&system->straight) || !light_valid(&system->cross) ||
        system->straight.timer_index == system->cross.timer_index ||
        system->green_s == 0U || system->yellow_s == 0U ||
        (uint32_t)system->green_s + system->yellow_s > UINT16_MAX)
        return 0;

    uint32_t straight_pins = system->straight.gpio[0] | system->straight.gpio[1] |
                             system->straight.gpio[2];
    uint32_t cross_pins = system->cross.gpio[0] | system->cross.gpio[1] |
                          system->cross.gpio[2];
    if (system->straight.port == system->cross.port && (straight_pins & cross_pins))
        return 0;

    TRAFFIC_SYSTEM_Reset(system);
    return 1;
}

void TRAFFIC_SYSTEM_Reset(traffic_system_t *system)
{
    system->running = 0U;
    SOFT_TIMER_Set(system->straight.timer_index, 0U);
    SOFT_TIMER_Set(system->cross.timer_index, 0U);
    system->straight.state = LED_GREEN;
    system->cross.state = LED_RED;
    system->straight.remaining_s = system->green_s;
    system->cross.remaining_s = system->green_s + system->yellow_s;
    system->straight.timeout_flag = 0U;
    system->cross.timeout_flag = 0U;
    system_display(system);
}

void TRAFFIC_SYSTEM_Start(traffic_system_t *system)
{
    if (system->running)
        return;

    SOFT_TIMER_Set(system->straight.timer_index, 1000U);
    SOFT_TIMER_Set(system->cross.timer_index, 1000U);
    system->running = 1U;
}

static void light_tick(traffic_light_t *light)
{
    if (SOFT_TIMER_Expired(light->timer_index))
    {
        if (light->remaining_s > 0U)
        {
            --light->remaining_s;
            if (light->remaining_s == 0U)
                light->timeout_flag = 1U;
        }
        SOFT_TIMER_Set(light->timer_index, 1000U);
    }
}

static void light_run(traffic_light_t *light, const traffic_system_t *system)
{
    if (!light->timeout_flag)
        return;

    light->timeout_flag = 0U;
    switch (light->state)
    {
    case LED_RED:
        light->state = LED_GREEN;
        light->remaining_s = system->green_s;
        break;
    case LED_GREEN:
        light->state = LED_YELLOW;
        light->remaining_s = system->yellow_s;
        break;
    case LED_YELLOW:
        light->state = LED_RED;
        light->remaining_s = system->green_s + system->yellow_s;
        break;
    }
}

void TRAFFIC_SYSTEM_Run(traffic_system_t *system)
{
    if (!system->running)
        return;

    if ((unsigned)system->straight.state > LED_RED ||
        (unsigned)system->cross.state > LED_RED)
    {
        TRAFFIC_SYSTEM_Reset(system);
        return;
    }
    light_tick(&system->straight);
    light_tick(&system->cross);
    if (!system->straight.timeout_flag && !system->cross.timeout_flag)
        return;

    light_run(&system->straight, system);
    light_run(&system->cross, system);
    system_display(system);
}
