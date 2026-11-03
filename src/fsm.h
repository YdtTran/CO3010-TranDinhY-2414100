#ifndef FSM_H
#define FSM_H
#include "global.h"
void FSM_Init(fsm_t *fsm);
/* Call once per TIM2 tick, after FSM_Init. */
void FSM_ScanButtons(void);
void FSM_ClearButtons(void);
/* Service outputs after all three mode FSMs run. */
void FSM_ServiceOutputs(void);
/* Shared output operation; each mode owns its phase sequencing. */
void FSM_SetPhase(uint8_t phase);
void FSM_Automatic_Init(void);
void FSM_Automatic_Run(void);
void FSM_Manual_Init(void);
void FSM_Manual_Run(void);
void FSM_Setting_Init(void);
void FSM_Setting_Run(void);
#endif
