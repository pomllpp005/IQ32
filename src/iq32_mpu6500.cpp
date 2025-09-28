<<<<<<< HEAD
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
=======
#include "iq32_mpu6500.h"

SPI_HandleTypeDef hspi1;

// --- ฟังก์ชัน CS ---
static inline void MPU6500_CS_Low(void)  { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); }
static inline void MPU6500_CS_High(void) { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); }

// --- Init SPI1 และ GPIO ---
void MPU6500_Init(void)
{
    // เปิด Clock
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // ตั้งค่า GPIO สำหรับ SPI1
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // SCK (PA5), MOSI (PA7)
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
>>>>>>> 39340a1 (v1)
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

<<<<<<< HEAD
    // CS pin
    GPIO_InitStruct.Pin = SPI_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SPI_CS_PORT, &GPIO_InitStruct);
    MPU6500_CS_High();

    // Configure SPI
=======
    // MISO (PA6)
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;   // ✅ ดึงขึ้น ป้องกันอ่าน 0
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // CS (PA4) เป็น Output ปกติ
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // CS High

    // ตั้งค่า SPI1 (Mode 3)
>>>>>>> 39340a1 (v1)
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
<<<<<<< HEAD
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
=======
    hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;   // ✅ MODE3
    hspi1.Init.CLKPhase    = SPI_PHASE_2EDGE;     // ✅ MODE3
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64; // ช้าหน่อยเพื่อความเสถียร
>>>>>>> 39340a1 (v1)
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
<<<<<<< HEAD
    
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
=======

    if(HAL_SPI_Init(&hspi1) != HAL_OK) {
        ssd1306_Fill(Black);
        ssd1306_SetCursorLine(0, 0, Font_6x8);
        ssd1306_WriteString("SPI Init Error!", Font_6x8, White);
        ssd1306_UpdateScreen();
        while(1) {
            LED_Toggle(LED1);
            HAL_Delay(100);
        }
    }

    HAL_Delay(100);

    // Reset MPU6500
    MPU6500_WriteReg(0x6B, 0x80);
    HAL_Delay(100);

    // Wake up และตั้ง clock source
    MPU6500_WriteReg(0x6B, 0x01);
    HAL_Delay(50);

    // Sample Rate Divider
    MPU6500_WriteReg(0x19, 0x07);
    HAL_Delay(50);

    // Accelerometer range ±2g
    MPU6500_WriteReg(0x1C, 0x00);
    HAL_Delay(50);

    // Gyroscope range ±250 dps
    MPU6500_WriteReg(0x1B, 0x00);
    HAL_Delay(50);
}

void debug_mpu6500()
{
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 0, Font_6x8);
    ssd1306_WriteString("MPU6500 Debug:", Font_6x8, White);

    uint8_t reg = 0x75 | 0x80; // WHO_AM_I (0x75) | READ
    uint8_t val = 0;

    MPU6500_CS_Low();
    HAL_SPI_Transmit(&hspi1, &reg, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, &val, 1, HAL_MAX_DELAY);
    MPU6500_CS_High();

    OLED_ShowSInt("WHO_AM_I:", val, 0, 1);
    ssd1306_SetCursorLine(0, 2, Font_6x8);
    ssd1306_WriteString("Should be 0x70", Font_6x8, White);

    ssd1306_UpdateScreen();
    HAL_Delay(3000);
}

// --- เขียน Register ---
static void MPU6500_WriteReg(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg & 0x7F, val};
    MPU6500_CS_Low();
    HAL_SPI_Transmit(&hspi1, buf, 2, HAL_MAX_DELAY);
    MPU6500_CS_High();
}

// --- อ่าน Register หลายตัว ---
bool MPU6500_ReadRegs(uint8_t reg, uint8_t *buf, uint8_t len)
{
    reg |= 0x80; // set MSB สำหรับ read
    MPU6500_CS_Low();
    HAL_SPI_Transmit(&hspi1, &reg, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, buf, len, HAL_MAX_DELAY);
    MPU6500_CS_High();
    return true;
}

// --- อ่านค่า Accel/Gyro ---
bool MPU6500_ReadAccelGyro(float *ax, float *ay, float *az,
                           float *gx, float *gy, float *gz)
{
    uint8_t buf[14];

    if(!MPU6500_ReadRegs(0x3B, buf, 14)) {
        return false;
    }

    int16_t rawAx = (int16_t)((buf[0] << 8) | buf[1]);
    int16_t rawAy = (int16_t)((buf[2] << 8) | buf[3]);
    int16_t rawAz = (int16_t)((buf[4] << 8) | buf[5]);
    int16_t rawGx = (int16_t)((buf[8] << 8) | buf[9]);
    int16_t rawGy = (int16_t)((buf[10] << 8) | buf[11]);
    int16_t rawGz = (int16_t)((buf[12] << 8) | buf[13]);

    *ax = rawAx / 16384.0f;  // ±2g range
    *ay = rawAy / 16384.0f;
    *az = rawAz / 16384.0f;
    *gx = rawGx / 131.0f;    // ±250°/s range
    *gy = rawGy / 131.0f;
    *gz = rawGz / 131.0f;

    return true;
}
>>>>>>> 39340a1 (v1)
