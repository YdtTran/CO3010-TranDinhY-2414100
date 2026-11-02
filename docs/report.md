# Lab 3 report

## 5.2 Exercise 1: Sketch an FSM

The existing traffic-light sketches describe two cooperating state machines. The
operating mode starts at NORMAL (mode 1). Each debounced button-1 press advances
NORMAL -> SET_RED (2) -> SET_AMBER (3) -> SET_GREEN (4) -> NORMAL. In a setting
mode, button 2 proposes a duration and button 3 confirms it. These adjustment
behaviors are a design for later work, not implemented by exercises 1-5.

In NORMAL, the traffic cycle is:

```mermaid
stateDiagram-v2
    [*] --> L_GREEN_T_RED
    L_GREEN_T_RED --> L_AMBER_T_RED: green duration elapsed
    L_AMBER_T_RED --> L_RED_T_GREEN: amber duration elapsed
    L_RED_T_GREEN --> L_RED_T_AMBER: green duration elapsed
    L_RED_T_AMBER --> L_GREEN_T_RED: amber duration elapsed
```

L denotes left/right traffic; T denotes top/bottom traffic. Opposing green
lights are never on together. Red duration equals the opposing green plus amber
durations. Setting modes blink the selected color at 2 Hz and display the mode
and proposed duration. Exact duration limits must follow the full assignment;
the supplied excerpt does not specify them.

Editable sketches: [FSM](traffic-light-fsm.drawio) and
[compatible copy](traffic-light-fsm-compatible.drawio). Illustrations:
[traffic cycle](traffic-light-cycle.png), [modes](traffic-light-modes.png).

This branch contains the FSM report and reusable libraries. The old Lab 2
application has been removed; main is an idle scaffold. Lab 3 schematic wiring,
timer demonstration and button integration belong to later branches.

Validation: firmware compiled and linked with `ninja -C build/Debug`.

## 5.3 Exercise 2: Proteus schematic

The existing editable [Lab 3 project](../protues/lab3.pdsprj) is retained as the
schematic for this exercise. It contains the STM32F103C8, opposing traffic LED
groups, common-anode displays with PNP drivers, three buttons and indicator D15.
The wiring contract used for the next exercise is:

| Signal | MCU pins | Polarity |
| --- | --- | --- |
| D15 indicator | PA5 | Active low |
| Seven-segment A-G | PA8-PA14 | Active low |
| Paired digit enables | PA0, PA1 | Active low PNP |
| Left/right traffic group | PB0-PB2 | Active high |
| Top/bottom traffic group | PB3-PB5 | Active high |
| Buttons 1-3 | PB13-PB15 | Pull-up, switch to ground |

The two enable lines select pairs of digits (0/2 and 1/3); paired digits cannot
show independent values with this wiring. PA13/PA14 and PB3/PB4 require debug
port release. Use 3.3 V MCU power and a common ground. Retain the schematic's
current-limiting resistors and PNP base resistors.

Validation: the Proteus project archive opens structurally and contains ROOT.DSN
and ROOT.CDB. Firmware still builds. No Proteus simulation or electrical/net
connectivity validation was run; compare the pin table with the drawing before
hardware use. No Lab 3 GPIO or timer application is introduced on this branch.
