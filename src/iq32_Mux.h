<<<<<<< HEAD
// ========== iq32_Mux.h ==========
=======
>>>>>>> 39340a1 (v1)
#ifndef __IQ32_MUX_H__
#define __IQ32_MUX_H__

#include "stm32f4xx_hal.h"
<<<<<<< HEAD
#include "iq32_constants.h"

IQ32_Result_t MUX_Init(void);
IQ32_Result_t MUX_SelectChannel(uint8_t channel);
uint16_t MUX_Read(void);

#endif

=======

void MUX_Init(void);
void MUX_SelectChannel(uint8_t channel);
uint16_t MUX_Read(void);


#endif
>>>>>>> 39340a1 (v1)
