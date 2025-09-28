<<<<<<< HEAD

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
=======
#include "iq32_battery.h"

static ADC_HandleTypeDef hadc1;

// ค่า reference และ divider ของวงจรแบ่งแรงดัน
// เช่น ถ้าแบ่งครึ่ง ให้ vDivider = 2.0f
static const float vRef = 3.3f;
static const uint16_t adcMax = 4095;  // 12-bit ADC
static const float R1 = 1000.0f;
static const float R2 = 220.0f;
static const float vDivider = (R1 + R2) / R2;

void Battery_Init(void)
{
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_4;         // PC4 = ADC123_IN14
>>>>>>> 39340a1 (v1)
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

<<<<<<< HEAD
    // ADC configuration
=======
>>>>>>> 39340a1 (v1)
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
<<<<<<< HEAD
    
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
=======
    HAL_ADC_Init(&hadc1);

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_14;   // PC4
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
>>>>>>> 39340a1 (v1)
}

float Battery_GetVoltage(void)
{
<<<<<<< HEAD
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
=======
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint16_t raw = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    float vadc = (float)raw / (float)adcMax * vRef;
    return vadc * vDivider;
}
uint16_t Battery_GetRaw(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint16_t raw = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return raw;
}
float Battery_GetVoltage_Avg(void )
{
    const int samples = 100;
    uint32_t sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += Battery_GetRaw();
    }
    uint16_t raw = sum / samples;

    float vadc = (float)raw / (float)adcMax * vRef;
    const float calFactor = 1.0131f;  
    return vadc * vDivider * calFactor;
>>>>>>> 39340a1 (v1)
}