#ifndef IQ32_MPU6500_H
#define IQ32_MPU6500_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>

// ใช้ SPI1
extern SPI_HandleTypeDef hspi1;

// ฟังก์ชัน Init
void MPU6500_Init(void);

// ฟังก์ชันอ่านค่า
bool MPU6500_ReadAccelGyro(float *ax, float *ay, float *az, float *gx, float *gy, float *gz);

#endif
