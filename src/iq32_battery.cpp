
// ========== iq32_battery.cpp ==========
#include "iq32_battery.h"

static ADC_HandleTypeDef hadc1;
static bool isInitialized = false;

IQ32_Result_t Battery_Init(void)
{
    if(isInitialized) return IQ32_OK;

    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure battery ADC pin
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = BATTERY_ADC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // ADC configuration
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    
    if(HAL_ADC_Init(&hadc1) != HAL_OK) {
        return IQ32_ERROR;
    }

    // Configure ADC channel
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = BATTERY_ADC_CHANNEL;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLE_TIME;
    
    if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        return IQ32_ERROR;
    }

    isInitialized = true;
    return IQ32_OK;
}

uint16_t Battery_GetRaw(void)
{
    if(!isInitialized) return 0;

    if(HAL_ADC_Start(&hadc1) != HAL_OK) return 0;
    if(HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) != HAL_OK) return 0;
    
    uint16_t raw = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    
    return raw;
}

float Battery_GetVoltage(void)
{
    uint16_t raw = Battery_GetRaw();
    float vadc = ((float)raw / (float)ADC_RESOLUTION) * ADC_VREF;
    return vadc * BATTERY_DIVIDER;
}
float Battery_GetVoltage_Avg(void)
{
    if(!isInitialized) return 0.0f;

    uint32_t sum = 0;
    for(int i = 0; i < BATTERY_AVG_SAMPLES; i++) {
        sum += Battery_GetRaw();
    }
    
    uint16_t avgRaw = sum / BATTERY_AVG_SAMPLES;
    float vadc = ((float)avgRaw / (float)ADC_RESOLUTION) * ADC_VREF;
    return vadc * BATTERY_DIVIDER * BATTERY_CAL_FACTOR;
}