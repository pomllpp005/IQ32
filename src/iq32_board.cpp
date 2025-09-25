#include "iq32_board.h"
#include "iq32_switch.h" // เพิ่มเพื่อเช็คสวิตช์

#ifndef LED_PORT
  #define LED_PORT GPIOA
#endif
#ifndef LED_PIN
  #define LED_PIN GPIO_PIN_4
#endif

// เพิ่ม LED2 ค่าเริ่มต้น (สามารถ override ใน iq32_board.h)
#ifndef LED2_PORT
  #define LED2_PORT GPIOB
#endif
#ifndef LED2_PIN
  #define LED2_PIN GPIO_PIN_15
#endif

static void StartupBlink(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // เปิด clock กว้างๆ (ปลอดภัยถ้าไม่ได้ใช้พอร์ตรวมบางพอร์ต)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    // กำหนดพินเป็น output push-pull สำหรับทั้งสอง LED
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED2_PIN;
    HAL_GPIO_Init(LED2_PORT, &GPIO_InitStruct);

    // กระพริบตัวอย่าง 7 ครั้ง (50ms on/off)
    for (int i = 0; i < 7; ++i) {
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_SET);
        HAL_Delay(50);
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
        HAL_Delay(50);
    }
}

// กระพริบทั้งสอง LED ครั้งเดียว (on_ms แบบเปิด, off_ms แบบปิด)
void Blink_BothLEDsOnce(uint32_t on_ms, uint32_t off_ms)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_SET);
    HAL_Delay(on_ms);
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
    HAL_Delay(off_ms);
}

// กระพริบทั้งสอง LED จำนวนครั้ง
void Blink_BothLEDsTimes(int times, uint32_t on_ms, uint32_t off_ms)
{
    for (int i = 0; i < times; ++i) {
        Blink_BothLEDsOnce(on_ms, off_ms);
    }
}

// กระพริบทั้งสอง LED ต่อเนื่องจนกว่าจะกด SW1
void Blink_BothUntilSW1Pressed(uint32_t on_ms, uint32_t off_ms)
{
    for (;;) {
        Blink_BothLEDsOnce(on_ms, off_ms);
        if (Switch_IsPressed(SW1)) {
            // ปิด LED ก่อนออก
            HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
            break;
        }
    }
}

void IQ32_Init(void)
{
    HAL_Init();
    MX_GPIO_Init();
    // blink after GPIO init
    StartupBlink();
    MX_TIM2_Init();
    // อนาคตถ้ามี UART, I2C, SPI → ใส่เพิ่ม
}



// Handle
GPIO_InitTypeDef GPIO_InitStruct;
TIM_HandleTypeDef htim2;

// --- ตั้งค่า Timer2 สำหรับ PWM บน PA0 (CH1), PA1 (CH2), PA2 (CH3) ---
void MX_TIM2_Init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 84 - 1;    // APB1=84MHz → 1MHz timer tick
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000 - 1;     // PWM 1 kHz
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim2);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    // CH1 = PA0
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    // CH2 = PA1
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
    // CH3 = PA2 (Fan)
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
}

// --- ตั้งค่า GPIO ---
void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // INA/INB มอเตอร์1 (ซ้าย) = PA8, PA9
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // INA/INB มอเตอร์2 (ขวา) = PA10, PA11
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PWM = PA0 (CH1), PA1 (CH2), PA2 (CH3 - Fan)
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // LED = PB4, PA15
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // SW = PA12 (Active-Low)
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // ใช้ Pull-up
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}