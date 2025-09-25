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

// helper: รอจนกว่าจะกดพินที่ระบุ (active-low) พร้อม debounce
void Switch_WaitForPressPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    for (;;) {
        while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET) {
            HAL_Delay(5);
        }
        HAL_Delay(20);
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET) {
            break;
        }
    }
}

// wrapper สำหรับ SW1 (PA12)
void Switch_WaitForPress_SW1(void)
{
    Switch_WaitForPressPin(GPIOA, GPIO_PIN_12);
}

// wrapper สำหรับ SW2 — ถ้าไม่กำหนด SW2_PORT/SW2_PIN จะเป็น no-op
#if defined(SW2_PORT) && defined(SW2_PIN)
void Switch_WaitForPress_SW2(void)
{
    Switch_WaitForPressPin(SW2_PORT, SW2_PIN);
}
#else
void Switch_WaitForPress_SW2(void)
{
    // SW2 ไม่ถูกกำหนดใน iq32_switch.h — ฟังก์ชันนี้จะไม่บล็อก
    // ถ้าต้องการให้บล็อก ให้เพิ่มใน iq32_switch.h:
    // #define SW2_PORT GPIOB
    // #define SW2_PIN  GPIO_PIN_0
}
#endif
