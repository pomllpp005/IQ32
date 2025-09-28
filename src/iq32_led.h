<<<<<<< HEAD
// ========== iq32_led.h ==========
=======
>>>>>>> 39340a1 (v1)
#ifndef __IQ32_LED_H
#define __IQ32_LED_H

#include "stm32f4xx_hal.h"
<<<<<<< HEAD
#include "iq32_constants.h"

typedef enum {
    LED1 = 0,
    LED2,
    LED_ALL
} IQ32_LED_t;

IQ32_Result_t LED_Init(void);
IQ32_Result_t LED_On(IQ32_LED_t led);
IQ32_Result_t LED_Off(IQ32_LED_t led);
IQ32_Result_t LED_Toggle(IQ32_LED_t led);
IQ32_Result_t LED_Blink(IQ32_LED_t led, uint16_t count, uint16_t delay_ms);
=======

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
>>>>>>> 39340a1 (v1)

#endif
