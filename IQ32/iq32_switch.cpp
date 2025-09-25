#include "iq32_switch.h"

void Switch_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    // SW1 = PA12 (Active-Low)
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // ใช้ Pull-up ภายใน
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// อ่านค่าจาก Switch (0=กด, 1=ไม่กด)
uint8_t Switch_Read(IQ32_Switch_t sw)
{
    if (sw == SW1) {
        return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12);
    }
    return 1; // Default = ไม่กด
}

// คืนค่าเป็น boolean (1=กด, 0=ไม่กด)
uint8_t Switch_IsPressed(IQ32_Switch_t sw)
{
    return (Switch_Read(sw) == GPIO_PIN_RESET) ? 1 : 0;
}
