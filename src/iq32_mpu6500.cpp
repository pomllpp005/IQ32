// ========== iq32_mpu6500.cpp ==========
#include "iq32_mpu6500.h"

SPI_HandleTypeDef hspi1;
static bool isInitialized = false;

// Helper functions
static inline void MPU6500_CS_Low(void)  { HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_RESET); }
static inline void MPU6500_CS_High(void) { HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET); }

static IQ32_Result_t MPU6500_WriteReg(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg & 0x7F, val};
    MPU6500_CS_Low();
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, buf, 2, HAL_MAX_DELAY);
    MPU6500_CS_High();
    return (status == HAL_OK) ? IQ32_OK : IQ32_ERROR;
}

static IQ32_Result_t MPU6500_ReadRegs(uint8_t reg, uint8_t *buf, uint8_t len)
{
    reg |= 0x80; // Set MSB for read
    MPU6500_CS_Low();
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, &reg, 1, HAL_MAX_DELAY);
    if(status == HAL_OK) {
        status = HAL_SPI_Receive(&hspi1, buf, len, HAL_MAX_DELAY);
    }
    MPU6500_CS_High();
    return (status == HAL_OK) ? IQ32_OK : IQ32_ERROR;
}

static int16_t MPU6500_BE16(const uint8_t *b)
{
    return (int16_t)((b[0] << 8) | b[1]);
}

IQ32_Result_t MPU6500_Init(void)
{
    if(isInitialized) return IQ32_OK;

    // Enable clocks
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configure SPI pins
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // SCK, MOSI, MISO
    GPIO_InitStruct.Pin = SPI_SCK_PIN | SPI_MOSI_PIN | SPI_MISO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // CS pin
    GPIO_InitStruct.Pin = SPI_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SPI_CS_PORT, &GPIO_InitStruct);
    MPU6500_CS_High();

    // Configure SPI
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    
    if(HAL_SPI_Init(&hspi1) != HAL_OK) {
        return IQ32_ERROR;
    }

    // Reset MPU6500
    if(MPU6500_WriteReg(MPU6500_PWR_MGMT_1, 0x80) != IQ32_OK) {
        return IQ32_ERROR;
    }
    HAL_Delay(100);

    // Set clock source (use PLL)
    if(MPU6500_WriteReg(MPU6500_PWR_MGMT_1, 0x01) != IQ32_OK) {
        return IQ32_ERROR;
    }
    HAL_Delay(10);

    // Verify WHO_AM_I
    uint8_t whoami;
    if(MPU6500_WhoAmI(&whoami) != IQ32_OK || whoami != MPU6500_WHO_AM_I_VALUE) {
        return IQ32_ERROR;
    }

    isInitialized = true;
    return IQ32_OK;
}

IQ32_Result_t MPU6500_WhoAmI(uint8_t *whoami)
{
    if(!whoami) return IQ32_INVALID_PARAM;
    return MPU6500_ReadRegs(MPU6500_WHO_AM_I_REG, whoami, 1);
}

IQ32_Result_t MPU6500_ReadAccelGyro(float *ax, float *ay, float *az,
                                   float *gx, float *gy, float *gz)
{
    if(!isInitialized) return IQ32_NOT_INITIALIZED;
    if(!ax || !ay || !az || !gx || !gy || !gz) return IQ32_INVALID_PARAM;

    uint8_t buf[14];
    if(MPU6500_ReadRegs(MPU6500_ACCEL_XOUT_H, buf, 14) != IQ32_OK) {
        return IQ32_ERROR;
    }

    // Parse accelerometer data (skip temperature at buf[6], buf[7])
    int16_t rawAx = MPU6500_BE16(&buf[0]);
    int16_t rawAy = MPU6500_BE16(&buf[2]);
    int16_t rawAz = MPU6500_BE16(&buf[4]);
    
    // Parse gyroscope data
    int16_t rawGx = MPU6500_BE16(&buf[8]);
    int16_t rawGy = MPU6500_BE16(&buf[10]);
    int16_t rawGz = MPU6500_BE16(&buf[12]);

    // Convert to physical units
    *ax = rawAx / ACCEL_SCALE_2G;
    *ay = rawAy / ACCEL_SCALE_2G;
    *az = rawAz / ACCEL_SCALE_2G;
    *gx = rawGx / GYRO_SCALE_250DPS;
    *gy = rawGy / GYRO_SCALE_250DPS;
    *gz = rawGz / GYRO_SCALE_250DPS;

    return IQ32_OK;
}