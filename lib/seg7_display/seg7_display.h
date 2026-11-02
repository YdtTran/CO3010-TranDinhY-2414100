#ifndef SEG7_DISPLAY_H
#define SEG7_DISPLAY_H

#include "seg7.h"

typedef struct seg7_display
{
    uint32_t segs[8];
    const uint32_t *en_pin;
    GPIO_TypeDef *seg_port;
    GPIO_TypeDef *en_port;
    uint8_t *buffer; // Array of num digit values; must outlive this manager.
    uint8_t num;
    uint8_t com; // SEG7_COMMON_CATHODE or SEG7_COMMON_ANODE. //
    uint8_t current_digit;
} seg7_display_t;

// Configure all fields before init; digit-enable pins are active LOW.
// Initialization resets scanning and blanks outputs, preserving the buffer.
void SEG7_Init(seg7_display_t *display);
void SEG7_Update(seg7_display_t *display, int index);
// Copy display->num digit values into the display buffer.
void SEG7_UpdateBuffer(seg7_display_t *display, const uint8_t *buffer);
void SEG7_Scan(seg7_display_t *display);

#endif /* SEG7_DISPLAY_H */
