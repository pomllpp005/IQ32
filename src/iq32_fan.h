// ========== iq32_fan.h ==========
#ifndef __IQ32_FAN_H
#define __IQ32_FAN_H

#include "stm32f4xx_hal.h"
#include "iq32_constants.h"

IQ32_Result_t Fan_SetSpeed(uint16_t speed);
IQ32_Result_t Fan_Stop(void);
uint16_t Fan_GetSpeed(void);

#endif
// ===== Implementation =====