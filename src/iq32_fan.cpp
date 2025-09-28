// ========== iq32_fan.cpp ==========
#include "iq32_fan.h"

extern TIM_HandleTypeDef htim2;
static uint16_t currentSpeed = 0;

IQ32_Result_t Fan_SetSpeed(uint16_t speed)
{
    if(speed > PWM_MAX_DUTY) {
        return IQ32_OUT_OF_RANGE;
    }
    
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, speed);
    currentSpeed = speed;
    return IQ32_OK;
}

IQ32_Result_t Fan_Stop(void)
{
    return Fan_SetSpeed(0);
}

uint16_t Fan_GetSpeed(void)
{
    return currentSpeed;
}