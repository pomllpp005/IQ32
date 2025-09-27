#ifndef __IQ32_BATTERY_H
#define __IQ32_BATTERY_H

#include "stm32f4xx_hal.h"

void Battery_Init(void);
float Battery_GetVoltage(void);
uint16_t Battery_GetRaw(void);
float Battery_GetVoltage_Avg(void);
#endif
