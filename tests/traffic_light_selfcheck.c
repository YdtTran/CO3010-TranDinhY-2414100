/* Standalone target test: link traffic_light.c and software_timer.c, without HAL GPIO. */
#include <assert.h>
#include <stddef.h>
#include "traffic_light.h"
#include "software_timer.h"

static GPIO_TypeDef port;

void HAL_GPIO_WritePin(GPIO_TypeDef *gpio, uint16_t pins, GPIO_PinState state)
{
    if (state == GPIO_PIN_SET)
        gpio->ODR |= pins;
    else
        gpio->ODR &= ~pins;
    /* Check every output write, including intermediate phase transitions. */
    assert((gpio->ODR & (GPIO_PIN_0 | GPIO_PIN_3)) != (GPIO_PIN_0 | GPIO_PIN_3));
}

static void second(traffic_system_t *system)
{
    for (unsigned i = 0; i < 1000U / TICK_INTERVAL_MS; ++i)
    {
        SOFT_TIMER_Run();
        TRAFFIC_SYSTEM_Run(system);
    }
}

int main(void)
{
    traffic_system_t system = {
        .straight = {.port = &port, .gpio = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2},
                     .timer_index = 0U},
        .cross = {.port = &port, .gpio = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5},
                  .timer_index = 1U},
        .green_s = 3U, .yellow_s = 2U};

    assert(!TRAFFIC_SYSTEM_Init(NULL));
    assert(TRAFFIC_SYSTEM_Init(&system));
    assert(system.straight.remaining_s == 3U && system.cross.remaining_s == 5U);
    assert(!system.straight.timeout_flag && !system.cross.timeout_flag);
    assert(!system.running);
    second(&system);
    second(&system);
    assert(system.straight.remaining_s == 3U && system.cross.remaining_s == 5U);
    TRAFFIC_SYSTEM_Start(&system);
    assert(system.running);
    for (unsigned cycle = 0; cycle < 3U; ++cycle)
    {
        for (unsigned i = 0; i < 3U; ++i) second(&system);
        assert(system.straight.state == LED_YELLOW && system.cross.state == LED_RED);
        assert(system.straight.remaining_s == 2U && system.cross.remaining_s == 2U);
        for (unsigned i = 0; i < 2U; ++i) second(&system);
        assert(system.straight.state == LED_RED && system.cross.state == LED_GREEN);
        assert(system.straight.remaining_s == 5U && system.cross.remaining_s == 3U);
        for (unsigned i = 0; i < 3U; ++i) second(&system);
        assert(system.straight.state == LED_RED && system.cross.state == LED_YELLOW);
        for (unsigned i = 0; i < 2U; ++i) second(&system);
        assert(system.straight.state == LED_GREEN && system.cross.state == LED_RED);
    }

    const unsigned transition_s[] = {0U, 3U, 5U, 8U};
    const light_state_t straight_states[] = {LED_GREEN, LED_YELLOW, LED_RED, LED_RED};
    const light_state_t cross_states[] = {LED_RED, LED_RED, LED_GREEN, LED_YELLOW};
    for (unsigned sample = 0; sample < 4U; ++sample)
    {
        TRAFFIC_SYSTEM_Reset(&system);
        TRAFFIC_SYSTEM_Start(&system);
        for (unsigned i = 0; i < transition_s[sample]; ++i) second(&system);
        assert(system.straight.state == straight_states[sample]);
        assert(system.cross.state == cross_states[sample]);
        /* Leave an expiration pending to check that reset clears both flags. */
        for (unsigned i = 0; i < 1000U / TICK_INTERVAL_MS; ++i) SOFT_TIMER_Run();
        TRAFFIC_SYSTEM_Reset(&system);
        assert(system.straight.state == LED_GREEN && system.cross.state == LED_RED);
        assert(system.straight.remaining_s == 3U && system.cross.remaining_s == 5U);
        assert(port.ODR == (GPIO_PIN_0 | GPIO_PIN_5));
        assert(!system.running);
        second(&system);
        assert(system.straight.remaining_s == 3U && system.cross.remaining_s == 5U);
        assert(!SOFT_TIMER_Expired(system.straight.timer_index));
        assert(!SOFT_TIMER_Expired(system.cross.timer_index));
        TRAFFIC_SYSTEM_Start(&system);
        TRAFFIC_SYSTEM_Run(&system);
        for (unsigned i = 1; i < 1000U / TICK_INTERVAL_MS; ++i)
        {
            SOFT_TIMER_Run();
            TRAFFIC_SYSTEM_Run(&system);
        }
        assert(system.straight.remaining_s == 3U && system.cross.remaining_s == 5U);
        TRAFFIC_SYSTEM_Start(&system); /* Must not restart an active timer. */
        SOFT_TIMER_Run();
        TRAFFIC_SYSTEM_Run(&system);
        assert(system.straight.remaining_s == 2U && system.cross.remaining_s == 4U);
    }
    system.straight.state = (light_state_t)99;
    TRAFFIC_SYSTEM_Run(&system);
    assert(system.straight.state == LED_GREEN && system.cross.state == LED_RED);
    assert(system.straight.remaining_s == 3U && system.cross.remaining_s == 5U);

    assert(!system.running);
    TRAFFIC_SYSTEM_Start(&system);
    system.straight.remaining_s = 0U;
    TRAFFIC_SYSTEM_Run(&system);
    assert(system.straight.state == LED_GREEN && system.cross.state == LED_RED); /* FSM waits for the flag. */
    system.straight.timeout_flag = 1U;
    TRAFFIC_SYSTEM_Run(&system);
    assert(system.straight.state == LED_YELLOW && system.cross.state == LED_RED);
    assert(!system.straight.timeout_flag && !system.cross.timeout_flag);
    system.cross.timeout_flag = 1U;
    TRAFFIC_SYSTEM_Reset(&system);
    assert(!system.straight.timeout_flag && !system.cross.timeout_flag);

    system.green_s = 2U;
    system.yellow_s = 3U;
    assert(TRAFFIC_SYSTEM_Init(&system));
    TRAFFIC_SYSTEM_Start(&system);
    for (unsigned cycle = 0; cycle < 3U; ++cycle)
    {
        for (unsigned elapsed = 0; elapsed < 10U; ++elapsed)
        {
            assert(system.straight.state == (elapsed < 2U ? LED_GREEN :
                   elapsed < 5U ? LED_YELLOW : LED_RED));
            assert(system.cross.state == (elapsed < 5U ? LED_RED :
                   elapsed < 7U ? LED_GREEN : LED_YELLOW));
            second(&system);
        }
    }

    system.yellow_s = 2U;
    system.green_s = 70U;
    assert(TRAFFIC_SYSTEM_Init(&system));
    TRAFFIC_SYSTEM_Start(&system);
    second(&system);
    assert(system.straight.remaining_s == 69U && system.cross.remaining_s == 71U);

    system.green_s = 0U;
    uint32_t before = port.ODR;
    assert(!TRAFFIC_SYSTEM_Init(&system));
    assert(port.ODR == before);
    system.green_s = UINT16_MAX;
    assert(!TRAFFIC_SYSTEM_Init(&system));
    system.green_s = 3U;
    system.cross.timer_index = system.straight.timer_index;
    assert(!TRAFFIC_SYSTEM_Init(&system));
    system.cross.timer_index = SOFT_TIMER_NUM_MAX;
    assert(!TRAFFIC_SYSTEM_Init(&system));
    system.cross.timer_index = 1U;
    system.cross.gpio[0] = system.straight.gpio[0];
    assert(!TRAFFIC_SYSTEM_Init(&system));
    return 0;
}
