// ========== iq32_motor.h ==========
#ifndef __IQ32_MOTOR_H
#define __IQ32_MOTOR_H

#include "stm32f4xx_hal.h"
#include "iq32_constants.h"

IQ32_Result_t Motor1(int16_t speed);
IQ32_Result_t Motor2(int16_t speed);
IQ32_Result_t Motor_SetBoth(int16_t leftSpeed, int16_t rightSpeed);
IQ32_Result_t Motor_Stop(void);

#endif
// ===== Implementation =====