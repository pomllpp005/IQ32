// ========== iq32_adc.h ==========
#ifndef __IQ32_ADC_H
#define __IQ32_ADC_H

#include "stm32f4xx_hal.h"
#include "iq32_constants.h"
#include <stdint.h>
#include <stdbool.h>

// ADC Channel definitions
typedef enum {
    IQ32_ADC_CHANNEL_MUX = 0,
    IQ32_ADC_CHANNEL_BATTERY,
    IQ32_ADC_CHANNEL_COUNT
} IQ32_ADC_Channel_t;

// ADC Configuration structure
typedef struct {
    uint32_t channel;
    GPIO_TypeDef* gpio_port;
    uint16_t gpio_pin;
    uint32_t sampling_time;
} IQ32_ADC_ChannelConfig_t;

// ADC Status
typedef struct {
    bool initialized;
    bool busy;
    uint32_t last_read_time;
    uint16_t last_values[IQ32_ADC_CHANNEL_COUNT];
} IQ32_ADC_Status_t;

// Global ADC handle and status
extern ADC_HandleTypeDef hadc1;
extern IQ32_ADC_Status_t g_adc_status;

// Function prototypes
IQ32_Result_t IQ32_ADC_Init(void);
IQ32_Result_t IQ32_ADC_DeInit(void);
IQ32_Result_t IQ32_ADC_Read(IQ32_ADC_Channel_t channel, uint16_t* value);
IQ32_Result_t IQ32_ADC_ReadMultiple(uint16_t* values, uint8_t count);
IQ32_Result_t IQ32_ADC_ReadWithTimeout(IQ32_ADC_Channel_t channel, uint16_t* value, uint32_t timeout_ms);

// Utility functions
bool IQ32_ADC_IsInitialized(void);
bool IQ32_ADC_IsBusy(void);
uint16_t IQ32_ADC_GetLastValue(IQ32_ADC_Channel_t channel);
float IQ32_ADC_ToVoltage(uint16_t raw_value);

// Advanced functions
IQ32_Result_t IQ32_ADC_Calibrate(void);
IQ32_Result_t IQ32_ADC_SetSamplingTime(IQ32_ADC_Channel_t channel, uint32_t sampling_time);

#endif // __IQ32_ADC_H
