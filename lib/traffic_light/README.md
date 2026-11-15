# Traffic light

One traffic_light_t represents both directions of an intersection on one GPIO port.
The six pins are straight green/yellow/red, followed by cross green/yellow/red.
TRAFFIC_LIGHT_Init validates all six pins and blanks the outputs.

TRAFFIC_LIGHT_SetColor(&light_system, LED_GREEN, LED_RED) blanks all six LEDs,
then enables the requested colors together. LED_OFF disables a direction.
Invalid colors leave the outputs unchanged. Both greens are supported for setting-mode blinking.
Call SetColor from the main loop after successful initialization.
