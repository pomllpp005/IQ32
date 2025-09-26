#ifndef __MUX_H__
#define __MUX_H__

#include "stm32f4xx_hal.h" // หรือรุ่นที่คุณใช้

void MUX_Init(void);
void MUX_SelectChannel(uint8_t channel);
uint16_t MUX_Read(void);


#endif