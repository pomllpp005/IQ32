<<<<<<< HEAD
// ========== iq32_battery.h ==========
=======
>>>>>>> 39340a1 (v1)
#ifndef __IQ32_BATTERY_H
#define __IQ32_BATTERY_H

#include "stm32f4xx_hal.h"
<<<<<<< HEAD
#include "iq32_constants.h"

IQ32_Result_t Battery_Init(void);
uint16_t Battery_GetRaw(void);
float Battery_GetVoltage_Avg(void);
float Battery_GetVoltage(void);

=======

void Battery_Init(void);
float Battery_GetVoltage(void);
uint16_t Battery_GetRaw(void);
float Battery_GetVoltage_Avg(void);
>>>>>>> 39340a1 (v1)
#endif
