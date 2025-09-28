<<<<<<< HEAD
// ========== iq32_Mux.cpp ==========
#include "iq32_Mux.h"

static ADC_HandleTypeDef hadc1;
static bool isInitialized = false;

IQ32_Result_t MUX_Init(void)
{
    if(isInitialized) return IQ32_OK;

    __HAL_RCC_GPIOA_CLK_ENABLE();
=======
#include "iq32_Mux.h"

// --- ตัวแปร ADC ---
static ADC_HandleTypeDef hadc1;

// --- กำหนดขา S0–S3 และช่อง ADC ---
#define MUX_S0_PORT GPIOC
#define MUX_S0_PIN  GPIO_PIN_7

#define MUX_S1_PORT GPIOC
#define MUX_S1_PIN  GPIO_PIN_6

#define MUX_S2_PORT GPIOB
#define MUX_S2_PIN  GPIO_PIN_10

#define MUX_S3_PORT GPIOB
#define MUX_S3_PIN  GPIO_PIN_2

#define MUX_SIG_CHANNEL ADC_CHANNEL_3 // PA3

// --- ฟังก์ชันเริ่มต้น MUX ---
void MUX_Init(void)
{
>>>>>>> 39340a1 (v1)
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_ADC1_CLK_ENABLE();

<<<<<<< HEAD
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // MUX control pins
=======
    // ตั้งค่า GPIO สำหรับ S0–S3
    GPIO_InitTypeDef GPIO_InitStruct = {0};
>>>>>>> 39340a1 (v1)
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

<<<<<<< HEAD
    const struct {
        GPIO_TypeDef* port;
        uint16_t pin;
    } mux_pins[] = {
        {MUX_S0_PORT, MUX_S0_PIN},
        {MUX_S1_PORT, MUX_S1_PIN},
        {MUX_S2_PORT, MUX_S2_PIN},
        {MUX_S3_PORT, MUX_S3_PIN}
    };

    for(int i = 0; i < 4; i++) {
        GPIO_InitStruct.Pin = mux_pins[i].pin;
        HAL_GPIO_Init(mux_pins[i].port, &GPIO_InitStruct);
    }

    // ADC signal pin
    GPIO_InitStruct.Pin = MUX_SIGNAL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // ADC configuration
=======
    GPIO_InitStruct.Pin = MUX_S0_PIN;
    HAL_GPIO_Init(MUX_S0_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MUX_S1_PIN;
    HAL_GPIO_Init(MUX_S1_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MUX_S2_PIN;
    HAL_GPIO_Init(MUX_S2_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MUX_S3_PIN;
    HAL_GPIO_Init(MUX_S3_PORT, &GPIO_InitStruct);

    // ตั้งค่า ADC สำหรับ PA3
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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

    isInitialized = true;
    return IQ32_OK;
}

IQ32_Result_t MUX_SelectChannel(uint8_t channel)
{
    if(!isInitialized) return IQ32_NOT_INITIALIZED;
    if(channel >= MAX_SENSORS) return IQ32_INVALID_PARAM;
    
    const struct {
        GPIO_TypeDef* port;
        uint16_t pin;
    } control_pins[] = {
        {MUX_S0_PORT, MUX_S0_PIN},
        {MUX_S1_PORT, MUX_S1_PIN},
        {MUX_S2_PORT, MUX_S2_PIN},
        {MUX_S3_PORT, MUX_S3_PIN}
    };

    for(int i = 0; i < 4; i++) {
        GPIO_PinState state = GET_BIT(channel, i) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(control_pins[i].port, control_pins[i].pin, state);
    }
    
    HAL_Delay(MUX_SETTLE_TIME);
    return IQ32_OK;
}

uint16_t MUX_Read(void)
{
    if(!isInitialized) return 0;

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = MUX_SIGNAL_CHANNEL;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLE_TIME;
    
    if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        return 0;
    }

    if(HAL_ADC_Start(&hadc1) != HAL_OK) return 0;
    if(HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) != HAL_OK) return 0;
    
    uint16_t value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    
=======
    HAL_ADC_Init(&hadc1);
}

// --- เลือกช่องของ MUX ---
void MUX_SelectChannel(uint8_t channel)
{
    if (channel > 15) return;
    
    HAL_GPIO_WritePin(MUX_S0_PORT, MUX_S0_PIN, (channel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MUX_S1_PORT, MUX_S1_PIN, (channel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MUX_S2_PORT, MUX_S2_PIN, (channel & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MUX_S3_PORT, MUX_S3_PIN, (channel & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    
    // เพิ่ม settling time
    HAL_Delay(5); // หรือใช้ microsecond delay ถ้ามี
}

// --- อ่านค่าจากช่องที่เลือก ---
uint16_t MUX_Read(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = MUX_SIG_CHANNEL;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint16_t value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

>>>>>>> 39340a1 (v1)
    return value;
}