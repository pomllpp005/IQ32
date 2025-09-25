#include "iq32_board.h"

void IQ32_Init(void)
{
    HAL_Init();
    MX_GPIO_Init();
    MX_TIM2_Init();
    // อนาคตถ้ามี UART, I2C, SPI → ใส่เพิ่ม
}
