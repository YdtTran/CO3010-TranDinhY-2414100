#ifndef APP_H
#define APP_H

#include "stm32f1xx_hal.h"

#define DOT_PORT GPIOA
#define DOT_PIN GPIO_PIN_4

void appInit(void);
void appRun(void);

#endif /* APP_H */
