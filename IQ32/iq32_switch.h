#ifndef __IQ32_SWITCH_H
#define __IQ32_SWITCH_H

#include "stm32f4xx_hal.h"

// มี Switch หลักตัวเดียวบนบอร์ด
typedef enum {
    SW1 = 0   // PA12
} IQ32_Switch_t;

// ฟังก์ชันควบคุม Switch
void Switch_Init(void);
uint8_t Switch_Read(IQ32_Switch_t sw);    // คืนค่า 0=กด, 1=ไม่กด
uint8_t Switch_IsPressed(IQ32_Switch_t sw);

#endif
