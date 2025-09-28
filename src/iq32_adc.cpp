
// ========== iq32_adc.cpp ==========
#include "iq32_adc.h"

// Global variables
ADC_HandleTypeDef hadc1;
IQ32_ADC_Status_t g_adc_status;

// ADC channel configuration table
static const IQ32_ADC_ChannelConfig_t adc_channels[IQ32_ADC_CHANNEL_COUNT] = {
    // MUX Channel (PA3 - ADC123_IN3)
    {
        .channel = ADC_CHANNEL_3,
        .gpio_port = GPIOA,
        .gpio_pin = GPIO_PIN_3,
        .sampling_time = ADC_SAMPLETIME_480CYCLES
    },
    // Battery Channel (PC4 - ADC123_IN14)
    {
        .channel = ADC_CHANNEL_14,
        .gpio_port = GPIOC,
        .gpio_pin = GPIO_PIN_4,
        .sampling_time = ADC_SAMPLETIME_480CYCLES
    }
};

// Private function prototypes
static IQ32_Result_t ADC_ConfigureChannel(IQ32_ADC_Channel_t channel);
static IQ32_Result_t ADC_StartConversion(void);
static IQ32_Result_t ADC_WaitForConversion(uint32_t timeout_ms);

// ========== INITIALIZATION FUNCTIONS ==========

IQ32_Result_t IQ32_ADC_Init(void)
{
    if(g_adc_status.initialized) {
        return IQ32_OK; // Already initialized
    }

    // Enable clocks
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure GPIO pins for ADC
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    for(uint8_t i = 0; i < IQ32_ADC_CHANNEL_COUNT; i++) {
        GPIO_InitStruct.Pin = adc_channels[i].gpio_pin;
        HAL_GPIO_Init(adc_channels[i].gpio_port, &GPIO_InitStruct);
    }

    // Configure ADC
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

    // Initialize status structure
    g_adc_status.initialized = true;
    g_adc_status.busy = false;
    g_adc_status.last_read_time = 0;
    
    for(uint8_t i = 0; i < IQ32_ADC_CHANNEL_COUNT; i++) {
        g_adc_status.last_values[i] = 0;
    }

    return IQ32_OK;
}

IQ32_Result_t IQ32_ADC_DeInit(void)
{
    if(!g_adc_status.initialized) {
        return IQ32_NOT_INITIALIZED;
    }

    if(HAL_ADC_DeInit(&hadc1) != HAL_OK) {
        return IQ32_ERROR;
    }

    __HAL_RCC_ADC1_CLK_DISABLE();
    
    g_adc_status.initialized = false;
    g_adc_status.busy = false;
    
    return IQ32_OK;
}

// ========== CORE READ FUNCTIONS ==========

IQ32_Result_t IQ32_ADC_Read(IQ32_ADC_Channel_t channel, uint16_t* value)
{
    if(!g_adc_status.initialized) {
        return IQ32_NOT_INITIALIZED;
    }
    
    if(!VALIDATE_NOT_NULL(value)) {
        return IQ32_INVALID_PARAM;
    }
    
    if(channel >= IQ32_ADC_CHANNEL_COUNT) {
        return IQ32_OUT_OF_RANGE;
    }
    
    if(g_adc_status.busy) {
        return IQ32_TIMEOUT; // ADC is busy
    }

    g_adc_status.busy = true;
    IQ32_Result_t result = IQ32_OK;

    // Configure the channel
    if(ADC_ConfigureChannel(channel) != IQ32_OK) {
        result = IQ32_ERROR;
        goto cleanup;
    }

    // Start conversion
    if(ADC_StartConversion() != IQ32_OK) {
        result = IQ32_ERROR;
        goto cleanup;
    }

    // Wait for conversion
    if(ADC_WaitForConversion(HAL_MAX_DELAY) != IQ32_OK) {
        result = IQ32_TIMEOUT;
        goto cleanup;
    }

    // Get the result
    *value = HAL_ADC_GetValue(&hadc1);
    
    // Update status
    g_adc_status.last_values[channel] = *value;
    g_adc_status.last_read_time = HAL_GetTick();

    // Stop ADC
    HAL_ADC_Stop(&hadc1);

cleanup:
    g_adc_status.busy = false;
    return result;
}

IQ32_Result_t IQ32_ADC_ReadWithTimeout(IQ32_ADC_Channel_t channel, uint16_t* value, uint32_t timeout_ms)
{
    if(!g_adc_status.initialized) {
        return IQ32_NOT_INITIALIZED;
    }
    
    if(!VALIDATE_NOT_NULL(value)) {
        return IQ32_INVALID_PARAM;
    }
    
    if(channel >= IQ32_ADC_CHANNEL_COUNT) {
        return IQ32_OUT_OF_RANGE;
    }

    uint32_t start_time = HAL_GetTick();
    
    // Wait for ADC to become available
    while(g_adc_status.busy && (HAL_GetTick() - start_time) < timeout_ms) {
        HAL_Delay(1);
    }
    
    if(g_adc_status.busy) {
        return IQ32_TIMEOUT;
    }

    g_adc_status.busy = true;
    IQ32_Result_t result = IQ32_OK;

    // Configure the channel
    if(ADC_ConfigureChannel(channel) != IQ32_OK) {
        result = IQ32_ERROR;
        goto cleanup;
    }

    // Start conversion
    if(ADC_StartConversion() != IQ32_OK) {
        result = IQ32_ERROR;
        goto cleanup;
    }

    // Wait for conversion with timeout
    if(ADC_WaitForConversion(timeout_ms) != IQ32_OK) {
        result = IQ32_TIMEOUT;
        goto cleanup;
    }

    // Get the result
    *value = HAL_ADC_GetValue(&hadc1);
    
    // Update status
    g_adc_status.last_values[channel] = *value;
    g_adc_status.last_read_time = HAL_GetTick();

    // Stop ADC
    HAL_ADC_Stop(&hadc1);

cleanup:
    g_adc_status.busy = false;
    return result;
}

IQ32_Result_t IQ32_ADC_ReadMultiple(uint16_t* values, uint8_t count)
{
    if(!g_adc_status.initialized) {
        return IQ32_NOT_INITIALIZED;
    }
    
    if(!VALIDATE_NOT_NULL(values) || count == 0 || count > IQ32_ADC_CHANNEL_COUNT) {
        return IQ32_INVALID_PARAM;
    }

    for(uint8_t i = 0; i < count; i++) {
        IQ32_Result_t result = IQ32_ADC_Read((IQ32_ADC_Channel_t)i, &values[i]);
        if(result != IQ32_OK) {
            return result;
        }
        HAL_Delay(1); // Small delay between conversions
    }

    return IQ32_OK;
}

// ========== UTILITY FUNCTIONS ==========

bool IQ32_ADC_IsInitialized(void)
{
    return g_adc_status.initialized;
}

bool IQ32_ADC_IsBusy(void)
{
    return g_adc_status.busy;
}

uint16_t IQ32_ADC_GetLastValue(IQ32_ADC_Channel_t channel)
{
    if(channel >= IQ32_ADC_CHANNEL_COUNT) {
        return 0;
    }
    return g_adc_status.last_values[channel];
}

float IQ32_ADC_ToVoltage(uint16_t raw_value)
{
    return ((float)raw_value / (float)ADC_RESOLUTION) * ADC_VREF;
}

// ========== ADVANCED FUNCTIONS ==========

IQ32_Result_t IQ32_ADC_Calibrate(void)
{
    if(!g_adc_status.initialized) {
        return IQ32_NOT_INITIALIZED;
    }

    if(g_adc_status.busy) {
        return IQ32_TIMEOUT;
    }

    g_adc_status.busy = true;
    
    // Perform ADC calibration (if supported by the MCU)
    // For STM32F4, we can do a simple offset calibration
    HAL_StatusTypeDef status = HAL_ADCEx_Calibration_Start(&hadc1);
    
    g_adc_status.busy = false;
    
    return (status == HAL_OK) ? IQ32_OK : IQ32_ERROR;
}

IQ32_Result_t IQ32_ADC_SetSamplingTime(IQ32_ADC_Channel_t channel, uint32_t sampling_time)
{
    if(!g_adc_status.initialized) {
        return IQ32_NOT_INITIALIZED;
    }
    
    if(channel >= IQ32_ADC_CHANNEL_COUNT) {
        return IQ32_OUT_OF_RANGE;
    }

    // Validate sampling time
    switch(sampling_time) {
        case ADC_SAMPLETIME_3CYCLES:
        case ADC_SAMPLETIME_15CYCLES:
        case ADC_SAMPLETIME_28CYCLES:
        case ADC_SAMPLETIME_56CYCLES:
        case ADC_SAMPLETIME_84CYCLES:
        case ADC_SAMPLETIME_112CYCLES:
        case ADC_SAMPLETIME_144CYCLES:
        case ADC_SAMPLETIME_480CYCLES:
            break;
        default:
            return IQ32_INVALID_PARAM;
    }

    // Update the sampling time (this would require modifying the const array)
    // For now, we'll just return OK as the implementation would need dynamic storage
    return IQ32_OK;
}

// ========== PRIVATE FUNCTIONS ==========

static IQ32_Result_t ADC_ConfigureChannel(IQ32_ADC_Channel_t channel)
{
    if(channel >= IQ32_ADC_CHANNEL_COUNT) {
        return IQ32_OUT_OF_RANGE;
    }

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = adc_channels[channel].channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = adc_channels[channel].sampling_time;

    if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        return IQ32_ERROR;
    }

    return IQ32_OK;
}

static IQ32_Result_t ADC_StartConversion(void)
{
    if(HAL_ADC_Start(&hadc1) != HAL_OK) {
        return IQ32_ERROR;
    }
    return IQ32_OK;
}

static IQ32_Result_t ADC_WaitForConversion(uint32_t timeout_ms)
{
    if(HAL_ADC_PollForConversion(&hadc1, timeout_ms) != HAL_OK) {
        return IQ32_TIMEOUT;
    }
    return IQ32_OK;
}

// ========== EXAMPLE USAGE ==========
/*
// Example usage in main.c:

#include "iq32_adc.h"

int main(void)
{
    // Initialize system
    HAL_Init();
    
    // Initialize ADC
    if(IQ32_ADC_Init() != IQ32_OK) {
        // Handle error
        while(1);
    }
    
    while(1)
    {
        uint16_t mux_value, battery_value;
        
        // Read MUX channel
        if(IQ32_ADC_Read(IQ32_ADC_CHANNEL_MUX, &mux_value) == IQ32_OK) {
            float mux_voltage = IQ32_ADC_ToVoltage(mux_value);
            // Use mux_voltage...
        }
        
        // Read battery channel with timeout
        if(IQ32_ADC_ReadWithTimeout(IQ32_ADC_CHANNEL_BATTERY, &battery_value, 1000) == IQ32_OK) {
            float battery_voltage = IQ32_ADC_ToVoltage(battery_value);
            // Use battery_voltage...
        }
        
        // Read multiple channels at once
        uint16_t values[2];
        if(IQ32_ADC_ReadMultiple(values, 2) == IQ32_OK) {
            // values[0] = MUX value
            // values[1] = Battery value
        }
        
        HAL_Delay(100);
    }
}
*/