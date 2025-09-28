<<<<<<< HEAD

// ========== iq32_motor.cpp ==========
#include "iq32_motor.h"

extern TIM_HandleTypeDef htim2;

static void Motor_SetDirection(uint8_t motorNum, int16_t speed)
{
    GPIO_TypeDef* inaPort;
    uint16_t inaPin, inbPin;
    uint32_t channel;
    
    if(motorNum == 1) {
        inaPort = MOTOR1_INA_PORT;
        inaPin = MOTOR1_INA_PIN;
        inbPin = MOTOR1_INB_PIN;
        channel = TIM_CHANNEL_1;
    } else {
        inaPort = MOTOR2_INA_PORT;
        inaPin = MOTOR2_INA_PIN;
        inbPin = MOTOR2_INB_PIN;
        channel = TIM_CHANNEL_2;
    }
    
    uint16_t pwmValue = CONSTRAIN(ABS(speed), 0, PWM_MAX_DUTY);
    
    if(speed >= 0) {
        HAL_GPIO_WritePin(inaPort, inbPin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(inaPort, inaPin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(inaPort, inbPin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(inaPort, inaPin, GPIO_PIN_SET);
    }
    
    __HAL_TIM_SET_COMPARE(&htim2, channel, pwmValue);
}

IQ32_Result_t Motor1(int16_t speed)
{
    if(!VALIDATE_RANGE(speed, MOTOR_MIN_SPEED, MOTOR_MAX_SPEED)) {
        return IQ32_OUT_OF_RANGE;
    }
    
    Motor_SetDirection(1, speed);
    return IQ32_OK;
}

IQ32_Result_t Motor2(int16_t speed)
{
    if(!VALIDATE_RANGE(speed, MOTOR_MIN_SPEED, MOTOR_MAX_SPEED)) {
        return IQ32_OUT_OF_RANGE;
    }
    
    Motor_SetDirection(2, speed);
    return IQ32_OK;
}

IQ32_Result_t Motor_SetBoth(int16_t leftSpeed, int16_t rightSpeed)
{
    IQ32_Result_t result1 = Motor1(leftSpeed);
    IQ32_Result_t result2 = Motor2(rightSpeed);
    
    return (result1 == IQ32_OK && result2 == IQ32_OK) ? IQ32_OK : IQ32_ERROR;
}

IQ32_Result_t Motor_Stop(void)
{
    return Motor_SetBoth(0, 0);
=======
#include "iq32_motor.h"

extern TIM_HandleTypeDef htim2; // ประกาศจากไฟล์ .c หลัก

// --- Motor Control ---
void Motor1(int16_t speed)
{
    if (speed >= 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, -speed);
    }
}

void Motor2(int16_t speed)
{
    if (speed >= 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, -speed);
    }
>>>>>>> 39340a1 (v1)
}
