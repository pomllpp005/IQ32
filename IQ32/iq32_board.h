#ifndef __IQ32_BOARD_H
#define __IQ32_BOARD_H

#include "stm32f4xx_hal.h"
#include "iq32_motor.h"
#include "iq32_fan.h"
#include "iq32_led.h"
#include "iq32_switch.h"
#include "iq32_battery.h"
#include "iq32_mpu6500.h"
#include "OLED_I2C_SSD1309.h"
#include <stdbool.h>
#include <stdint.h>

// TIM handle (define in one .c file, declare extern here)
extern TIM_HandleTypeDef htim2;

// Prototypes that IQ32_Init calls
void MX_GPIO_Init(void);
void MX_TIM2_Init(void);

void IQ32_Init(void);   // Init GPIO, TIM, etc.

#endif