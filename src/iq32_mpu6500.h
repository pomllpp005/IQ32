// ========== iq32_mpu6500.h ==========
#ifndef IQ32_MPU6500_H
#define IQ32_MPU6500_H

#include "stm32f4xx_hal.h"
#include "iq32_constants.h"

extern SPI_HandleTypeDef hspi1;

IQ32_Result_t MPU6500_Init(void);
IQ32_Result_t MPU6500_ReadAccelGyro(float *ax, float *ay, float *az, 
                                   float *gx, float *gy, float *gz);
IQ32_Result_t MPU6500_WhoAmI(uint8_t *whoami);

#endif

