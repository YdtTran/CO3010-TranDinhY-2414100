#ifndef FSM_H
#define FSM_H

#include <stdint.h>

typedef enum
{
    FSM_NORMAL = 1,
    FSM_SET_RED,
    FSM_SET_AMBER,
    FSM_SET_GREEN
} fsm_mode_t;

typedef struct
{
    fsm_mode_t mode;
} fsm_t;

void FSM_Init(fsm_t *fsm);

/* Consume a debounced button-1 event to advance the project operating mode. */
void FSM_Run(fsm_t *fsm, uint8_t mode_pressed);

#endif /* FSM_H */
