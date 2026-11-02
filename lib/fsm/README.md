# Project operating-mode FSM

This module owns the operating modes described in `docs/report.md`:

`NORMAL (1) -> SET_RED (2) -> SET_AMBER (3) -> SET_GREEN (4) -> NORMAL`.

`FSM_Init` selects NORMAL. `FSM_Run` accepts a debounced button-1 press event;
each event advances one mode. Without an event the mode stays unchanged.
`main.c` reads the event and displays the resulting mode number.

Traffic-color transitions, countdowns and start/reset behavior belong to
`lib/traffic_light`, which does not depend on this module. Button-2/3 duration
editing and setting-mode blinking remain deferred as described in Exercise 5.
