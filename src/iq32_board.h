#ifndef __IQ32_BOARD_H
#define __IQ32_BOARD_H

#include "stm32f4xx_hal.h"
#include <Arduino.h>
#include "iq32_motor.h"
#include "iq32_fan.h"
#include "iq32_led.h"
#include "iq32_battery.h"
#include "iq32_mpu6500.h"
#include "iq32_Mux.h"
#include "ssd1306_fonts.h"
#include "ssd1306.h"

#include <stdbool.h>
#include <stdint.h>

// TIM handle (define in one .c file, declare extern here)
extern TIM_HandleTypeDef htim2;

// Prototypes that IQ32_Init calls
void MX_GPIO_Init(void);
void MX_TIM2_Init(void);
void Wait_SW1(void);
void Wait_SW2(void);
uint8_t Read_SW1(void);
uint8_t Read_SW2(void);
void IQ32_Init(void);   // Init GPIO, TIM, etc.


#endif