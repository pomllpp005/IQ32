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

    // ตั้งค่า GPIO สำหรับ SPI1 (PA5=SCK, PA6=MISO, PA7=MOSI, PA4=NSS)
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // SCK, MOSI
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // MISO
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // NSS (CS) เป็น Output ปกติ
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    MPU6500_CS_High();

    // ตั้งค่า SPI1
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; // ปรับได้ตามต้องการ
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    HAL_SPI_Init(&hspi1);

    // Reset MPU6500
    uint8_t data[2] = {0x6B & 0x7F, 0x80};  // reg=0x6B, val=0x80
    MPU6500_CS_Low();
    HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
    MPU6500_CS_High();
    HAL_Delay(100);

    // ตั้ง clock source (ใช้ PLL)
    data[0] = 0x6B & 0x7F;
    data[1] = 0x01;
    MPU6500_CS_Low();
    HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
    MPU6500_CS_High();
    HAL_Delay(10);
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
static void MPU6500_ReadRegs(uint8_t reg, uint8_t *buf, uint8_t len)
{
    reg |= 0x80; // set MSB สำหรับ read
    MPU6500_CS_Low();
    HAL_SPI_Transmit(&hspi1, &reg, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, buf, len, HAL_MAX_DELAY);
    MPU6500_CS_High();
}

// --- Helper: big-endian to int16 ---
static int16_t be16(const uint8_t *b)
{
    return (int16_t)((b[0] << 8) | b[1]);
}

// --- อ่านค่า Accel/Gyro ---
bool MPU6500_ReadAccelGyro(float *ax, float *ay, float *az,
                           float *gx, float *gy, float *gz)
{
    uint8_t buf[14];
    MPU6500_ReadRegs(0x3B, buf, 14);

    int16_t rawAx = be16(&buf[0]);
    int16_t rawAy = be16(&buf[2]);
    int16_t rawAz = be16(&buf[4]);
    int16_t rawGx = be16(&buf[8]);
    int16_t rawGy = be16(&buf[10]);
    int16_t rawGz = be16(&buf[12]);

    *ax = rawAx / 16384.0f;
    *ay = rawAy / 16384.0f;
    *az = rawAz / 16384.0f;
    *gx = rawGx / 131.0f;
    *gy = rawGy / 131.0f;
    *gz = rawGz / 131.0f;

    return true;
}
