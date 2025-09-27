#ifndef __IQ32_MUX_H__
#define __IQ32_MUX_H__

#include "stm32f4xx_hal.h"

void MUX_Init(void);
void MUX_SelectChannel(uint8_t channel);
uint16_t MUX_Read(void);


#endif