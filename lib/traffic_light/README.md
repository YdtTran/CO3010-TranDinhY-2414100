# Individual traffic lights

Each traffic_light_t holds one GPIO binding, color and remaining seconds.
TRAFFIC_LIGHT_Init validates pins and blanks the light. SetColor accepts green,
yellow, red or LED_OFF; invalid colors leave outputs unchanged. SetTime changes
the counter only; IncreaseTime saturates at UINT16_MAX and DecreaseTime at zero.
Reaching zero never changes the color. Other operations require successful Init.

The application FSMs own phase sequencing, paired-output safety and timers.
They blank both directions before applying a new phase. Setting deliberately
blinks the selected color on both directions. All operations run in main.

The old TRAFFIC_SYSTEM_* API has been replaced by TRAFFIC_LIGHT_* operations.
The application keeps red derived as green + yellow, within 1–99 seconds.
Configuration is kept in RAM and returns to defaults after reset.
