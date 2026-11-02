# Two-direction traffic system

`traffic_light_t` represents one direction. It owns its GPIO bindings, current
color, software-timer index and `remaining_s` countdown. `traffic_system_t` owns
the straight and cross directions, shared green/yellow durations in seconds and
two synchronized three-state FSMs: red -> green -> yellow -> red.
Red lasts `green_s + yellow_s` seconds.
Traffic-color transitions remain in this library; lib/fsm owns project operating modes.

Populate both ports, `gpio[LED_GREEN/LED_YELLOW/LED_RED]`, two distinct unused
timer indices, `green_s` and `yellow_s`. Configure the active-high GPIO outputs,
then call `TRAFFIC_SYSTEM_Init(&system)` **before starting TIM2**. A return value
of zero means invalid configuration; no outputs or timers are changed.
Initialization leaves the system paused. Call `TRAFFIC_SYSTEM_Start(&system)`
explicitly to begin counting, before starting TIM2 or with interrupts disabled.

Call only SOFT_TIMER_Run() from TIM2. Call TRAFFIC_SYSTEM_Run(&system) from
main with interrupts briefly disabled to keep both timer updates together.
Keep the main loop nonblocking: delayed polling delays countdowns because
one-shot timers are rearmed by the loop. Timer slots belong exclusively to
this system while running. Initialize before calling Run.

Each light sets `timeout_flag` when its countdown reaches zero. The FSM checks
that flag to advance each light; its transition clears its flag, and reset clears both.

Initialization displays straight green/cross red without counting. At each transition both
directions are switched off before enabling the new colors, so no intermediate GPIO
write enables conflicting greens. A direction's red countdown continues across
the opposite direction's green-to-yellow transition.

`TRAFFIC_SYSTEM_Reset(&system)` restores those initial states and countdowns,
disables both timers and clears pending expiration flags. It leaves `running`
false; `Run` does nothing until `Start` arms fresh one-second timers. Repeated
`Start` calls while running do not restart the timers. GPIO bindings, timer
indices and configured durations are preserved. Call reset only on an initialized
system, with interrupts disabled. `Run` uses a
shared three-state switch for each light and resets if either color state is invalid.

Durations and countdowns use `uint16_t` seconds. Both durations must be positive
and their sum must fit 65535 seconds. There is no 65-second millisecond limit.
Runtime duration editing is deferred until the setting-mode implementation.

The application initializes and starts this system before TIM2, then runs it
freely from the main loop without mode handling. Green lasts 2 seconds,
yellow 3 seconds and red 5 seconds. Straight uses L and cross uses T, with
indices 0/1/2 bound as green/yellow/red; timer slots 0 and 1 are reserved.

`tests/traffic_light_selfcheck.c` exercises the real traffic and software-timer
implementations with a fake GPIO writer. It checks repeated full cycles,
countdowns beyond 65 seconds, invalid configuration and conflicting greens at
every output write. Link it with `traffic_light.c` and `software_timer.c`
as standalone test firmware without the hardware HAL
GPIO driver. Cross-compilation does not execute these assertions.
