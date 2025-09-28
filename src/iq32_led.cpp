<<<<<<< HEAD

// ========== iq32_led.cpp ==========
#include "iq32_led.h"

static bool isInitialized = false;

static const struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} led_config[] = {
    {LED1_PORT, LED1_PIN},
    {LED2_PORT, LED2_PIN}
};

IQ32_Result_t LED_Init(void)
{
    if(isInitialized) return IQ32_OK;
=======
#include "iq32_led.h"

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
>>>>>>> 39340a1 (v1)

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

<<<<<<< HEAD
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    // Initialize both LED pins
    for(int i = 0; i < 2; i++) {
        GPIO_InitStruct.Pin = led_config[i].pin;
        HAL_GPIO_Init(led_config[i].port, &GPIO_InitStruct);
        HAL_GPIO_WritePin(led_config[i].port, led_config[i].pin, GPIO_PIN_RESET);
    }

    isInitialized = true;
    return IQ32_OK;
}

static IQ32_Result_t LED_Control(IQ32_LED_t led, GPIO_PinState state)
{
    if(!isInitialized) return IQ32_NOT_INITIALIZED;

    if(led == LED_ALL) {
        for(int i = 0; i < 2; i++) {
            HAL_GPIO_WritePin(led_config[i].port, led_config[i].pin, state);
        }
    } else if(led < 2) {
        HAL_GPIO_WritePin(led_config[led].port, led_config[led].pin, state);
    } else {
        return IQ32_INVALID_PARAM;
    }
    
    return IQ32_OK;
}

IQ32_Result_t LED_On(IQ32_LED_t led)
{
    return LED_Control(led, GPIO_PIN_SET);
}

IQ32_Result_t LED_Off(IQ32_LED_t led)
{
    return LED_Control(led, GPIO_PIN_RESET);
}

IQ32_Result_t LED_Toggle(IQ32_LED_t led)
{
    if(!isInitialized) return IQ32_NOT_INITIALIZED;

    if(led == LED_ALL) {
        for(int i = 0; i < 2; i++) {
            HAL_GPIO_TogglePin(led_config[i].port, led_config[i].pin);
        }
    } else if(led < 2) {
        HAL_GPIO_TogglePin(led_config[led].port, led_config[led].pin);
    } else {
        return IQ32_INVALID_PARAM;
    }
    
    return IQ32_OK;
}

IQ32_Result_t LED_Blink(IQ32_LED_t led, uint16_t count, uint16_t delay_ms)
{
    if(count == 0 || delay_ms == 0) return IQ32_INVALID_PARAM;

    for(uint16_t i = 0; i < count; i++) {
        LED_On(led);
        HAL_Delay(delay_ms);
        LED_Off(led);
        if(i < count - 1) HAL_Delay(delay_ms);
    }
    
    return IQ32_OK;
}
// ===== Implementation =====
=======
    // LED1 = PB4
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // LED2 = PA15
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // ปิดไฟทั้งหมดก่อน
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
}

void LED_On(IQ32_LED_t led)
{
    if (led == LED1) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
    } else if (led == LED2) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
    }
}

void LED_Off(IQ32_LED_t led)
{
    if (led == LED1) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
    } else if (led == LED2) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
    }
}

void LED_Toggle(IQ32_LED_t led)
{
    if (led == LED1) {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
    } else if (led == LED2) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_15);
    }
}
>>>>>>> 39340a1 (v1)
