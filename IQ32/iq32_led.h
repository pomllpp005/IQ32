#ifndef __IQ32_LED_H
#define __IQ32_LED_H

#include "stm32f4xx_hal.h"

// LED ที่ใช้ในบอร์ด
typedef enum {
    LED1 = 0,   // PB4
    LED2        // PA15
} IQ32_LED_t;

// ฟังก์ชันควบคุม LED
void LED_Init(void);
void LED_On(IQ32_LED_t led);
void LED_Off(IQ32_LED_t led);
void LED_Toggle(IQ32_LED_t led);

#endif
