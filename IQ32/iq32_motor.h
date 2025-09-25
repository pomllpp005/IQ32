#ifndef __IQ32_MOTOR_H
#define __IQ32_MOTOR_H

#include "stm32f4xx_hal.h"

void MX_TIM2_Init(void);
void Motor1(int16_t speed);
void Motor2(int16_t speed);

#endif