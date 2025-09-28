<<<<<<< HEAD
// ========== iq32_mpu6500.h ==========
=======
>>>>>>> 39340a1 (v1)
#ifndef IQ32_MPU6500_H
#define IQ32_MPU6500_H

#include "stm32f4xx_hal.h"
<<<<<<< HEAD
#include "iq32_constants.h"

extern SPI_HandleTypeDef hspi1;

IQ32_Result_t MPU6500_Init(void);
IQ32_Result_t MPU6500_ReadAccelGyro(float *ax, float *ay, float *az, 
                                   float *gx, float *gy, float *gz);
IQ32_Result_t MPU6500_WhoAmI(uint8_t *whoami);

#endif

=======
#include "ssd1306_fonts.h"
#include "iq32_led.h"
#include <stdbool.h>
#include <math.h>
// ใช้ SPI1
extern SPI_HandleTypeDef hspi1;

// ฟังก์ชัน Init
void MPU6500_Init(void);
void debug_mpu6500(void);
// ฟังก์ชันอ่านค่า
bool MPU6500_ReadAccelGyro(float *ax, float *ay, float *az, float *gx, float *gy, float *gz);
bool MPU6500_ReadRegs(uint8_t reg, uint8_t *buf, uint8_t len);
static void MPU6500_WriteReg(uint8_t reg, uint8_t val);

#endif
>>>>>>> 39340a1 (v1)
