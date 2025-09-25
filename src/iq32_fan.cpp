#include "iq32_fan.h"
extern TIM_HandleTypeDef htim2;

void Fan_SetSpeed(uint16_t speed)
{
    if (speed > 1000) speed = 1000;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, speed);
}
