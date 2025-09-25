// #include "stm32f4xx_hal.h"

// void MX_GPIO_Init(void)
// {
//     GPIO_InitTypeDef GPIO_InitStruct = {0}; // local, prevents multiple-definition

//     __HAL_RCC_GPIOA_CLK_ENABLE();
//     __HAL_RCC_GPIOB_CLK_ENABLE();

//     // Motor1 DIR (PA8, PA9)
//     GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
//     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//     // Motor2 DIR (PA10, PA11)
//     GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
//     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//     // PWM (PA0, PA1, PA2)
//     GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//     GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
//     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//     // LED (PB4, PA15)
//     GPIO_InitStruct.Pin = GPIO_PIN_4;
//     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//     HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//     GPIO_InitStruct.Pin = GPIO_PIN_15;
//     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//     // Switch (PA12)
//     GPIO_InitStruct.Pin = GPIO_PIN_12;
//     GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//     GPIO_InitStruct.Pull = GPIO_PULLUP;
//     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
// }
