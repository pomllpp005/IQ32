#include "iq32_board.h"


GPIO_InitTypeDef GPIO_InitStruct;
TIM_HandleTypeDef htim2;
I2C_HandleTypeDef hi2c1;

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


void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000; // หรือ 100000 ถ้าใช้ I2C slow mode
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c1);
}
// --- ตั้งค่า GPIO ---
void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

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
    // SW2 = PC9 (Active-Low)
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; 
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // ใช้ Pull-up
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // I2C1 = PB8(SCL), PB9(SDA)
    __HAL_RCC_I2C1_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

   

    // เริ่มต้นอุปกรณ์เสริมอื่นๆ
    MUX_Init(); // เริ่มต้น MUX
    Battery_Init(); // เริ่มต้น ADC  สำหรับวัดแรงดันแบตเตอรี่
    MPU6500_Init(); // เริ่มต้น MPU6500 (IMU)
   

}
// --- ฟังก์ชันเริ่มต้นบอร์ด IQ32 ---

void IQ32_Init(void)
{
    HAL_Init();
    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_I2C1_Init();
    ssd1306_Init();
    for (int i = 0; i < 5; i++) {
        LED_On(LED1);
        LED_On(LED2);
        HAL_Delay(50);
        LED_Off(LED1);
        LED_Off(LED2);
        HAL_Delay(50);
    }
   
    // อนาคตถ้ามี UART, I2C, SPI → ใส่เพิ่ม
}



// Handle


void Wait_SW1(void){
    while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_SET){} // รอจนกว่าจะกด SW1
    HAL_Delay(50); // ดีเลย์ตัดเด้ง
    LED_On(LED2);
    HAL_Delay(100);
    LED_Off(LED2);
    HAL_Delay(100);
    LED_On(LED2); 
    HAL_Delay(100);
    LED_Off(LED2);
    HAL_Delay(100);
}
void Wait_SW2(void){
    while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == GPIO_PIN_SET){} // รอจนกว่าจะกด SW2
    HAL_Delay(50); // ดีเลย์ตัดเด้ง
    LED_On(LED1);
    HAL_Delay(100);
    LED_Off(LED1);
    HAL_Delay(100);
    LED_On(LED1);
    HAL_Delay(100);
    LED_Off(LED1);

}