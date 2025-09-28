
// ========== iq32_pid_improved.cpp ==========
#include "iq32_pid_improved.h"
#include "iq32_led.h"

PIDController_t pidController;

IQ32_Result_t PID_Init(void)
{
    // Set default PID parameters
    pidController.kp = DEFAULT_KP;
    pidController.kd = DEFAULT_KD;
    pidController.alpha = DEFAULT_ALPHA;
    
    // Set default speeds
    pidController.baseSpeed = DEFAULT_BASE_SPEED;
    pidController.maxSpeed = DEFAULT_MAX_SPEED;
    pidController.fanSpeed = 0;
    
    // Set out of line parameters
    pidController.outLeftSpeed = DEFAULT_OUT_LEFT_SPEED;
    pidController.outRightSpeed = DEFAULT_OUT_RIGHT_SPEED;
    pidController.maxOutTime = DEFAULT_MAX_OUT_TIME;
    
    // Initialize line sensor
    IQ32_Result_t result = LineSensor_Init();
    if(result != IQ32_OK) return result;
    
    PID_Reset();
    return IQ32_OK;
}

IQ32_Result_t PID_SetParameters(float kp, float kd, float alpha)
{
    if(kp < 0 || kd < 0 || !VALIDATE_RANGE(alpha, 0.0f, 1.0f)) {
        return IQ32_INVALID_PARAM;
    }
    
    pidController.kp = kp;
    pidController.kd = kd;
    pidController.alpha = alpha;
    return IQ32_OK;
}

IQ32_Result_t PID_SetSpeeds(int16_t baseSpeed, int16_t maxSpeed, int16_t fanSpeed)
{
    if(!VALIDATE_RANGE(baseSpeed, 0, MOTOR_MAX_SPEED) ||
       !VALIDATE_RANGE(maxSpeed, 0, MOTOR_MAX_SPEED) ||
       !VALIDATE_RANGE(fanSpeed, 0, PWM_MAX_DUTY)) {
        return IQ32_OUT_OF_RANGE;
    }
    
    pidController.baseSpeed = baseSpeed;
    pidController.maxSpeed = maxSpeed;
    pidController.fanSpeed = fanSpeed;
    
    Fan_SetSpeed(fanSpeed);
    return IQ32_OK;
}

IQ32_Result_t PID_SetOutParams(int16_t leftSpeed, int16_t rightSpeed, uint32_t maxOutTimeMs)
{
    pidController.outLeftSpeed = leftSpeed;
    pidController.outRightSpeed = rightSpeed;
    pidController.maxOutTime = maxOutTimeMs;
    return IQ32_OK;
}

IQ32_Result_t PID_Start(void)
{
    pidController.isRunning = true;
    pidController.lastUpdateTime = HAL_GetTick();
    
    Fan_SetSpeed(pidController.fanSpeed);
    LED_On(LED1);
    return IQ32_OK;
}

IQ32_Result_t PID_Stop(void)
{
    pidController.isRunning = false;
    
    Motor_Stop();
    Fan_SetSpeed(0);
    LED_Off(LED_ALL);
    return IQ32_OK;
}

IQ32_Result_t PID_Reset(void)
{
    pidController.filteredError = 0;
    pidController.previousFilteredError = 0;
    pidController.error = 0;
    pidController.previousError = 0;
    pidController.controlSignal = 0;
    pidController.leftMotorSpeed = 0;
    pidController.rightMotorSpeed = 0;
    pidController.outStartTime = 0;
    pidController.isOut = false;
    pidController.isRunning = false;
    pidController.lastUpdateTime = HAL_GetTick();
    return IQ32_OK;
}

IQ32_Result_t PID_Update(void)
{
    if(!pidController.isRunning) return IQ32_NOT_INITIALIZED;
    
    uint32_t currentTime = HAL_GetTick();
    uint16_t position = LineSensor_ReadPosition();
    bool isOnLine = LineSensor_IsOnLine();
    
    if(isOnLine) {
        // Calculate error from center position
        pidController.error = position - SENSOR_CENTER;
        
        // Calculate control signal
        pidController.controlSignal = PID_Calculate(pidController.error);
        
        // Update motors
        PID_UpdateMotors(true);
        
        // Reset out of line state
        pidController.isOut = false;
        
        // Status LED indication
        if((currentTime / 100) % 10 < 5) {
            LED_On(LED2);
        } else {
            LED_Off(LED2);
        }
        
    } else {
        PID_HandleOutOfLine();
    }
    
    pidController.lastUpdateTime = currentTime;
    return IQ32_OK;
}

int32_t PID_Calculate(int16_t error)
{
    // Apply low-pass filter to error
    pidController.filteredError = pidController.alpha * (float)error + 
                                 (1.0f - pidController.alpha) * pidController.filteredError;
    
    // Calculate derivative
    float dError = pidController.filteredError - pidController.previousFilteredError;
    
    // Calculate PID output
    int32_t controlSignal = (int32_t)(pidController.kp * pidController.filteredError + 
                                     pidController.kd * dError);
    
    // Constrain output
    controlSignal = CONSTRAIN(controlSignal, MIN_CONTROL_SIGNAL, MAX_CONTROL_SIGNAL);
    
    // Update previous values
    pidController.previousFilteredError = pidController.filteredError;
    pidController.previousError = error;
    
    return controlSignal;
}

IQ32_Result_t PID_UpdateMotors(bool isOnLine)
{
    if(isOnLine) {
        // Calculate motor speeds
        pidController.leftMotorSpeed = pidController.baseSpeed - pidController.controlSignal;
        pidController.rightMotorSpeed = pidController.baseSpeed + pidController.controlSignal;
        
        // Constrain speeds
        pidController.leftMotorSpeed = CONSTRAIN(pidController.leftMotorSpeed, 
                                               -pidController.maxSpeed, pidController.maxSpeed);
        pidController.rightMotorSpeed = CONSTRAIN(pidController.rightMotorSpeed, 
                                                -pidController.maxSpeed, pidController.maxSpeed);
        
        // Set motor speeds
        return Motor_SetBoth(pidController.leftMotorSpeed, pidController.rightMotorSpeed);
    } else {
        return Motor_Stop();
    }
}

IQ32_Result_t PID_HandleOutOfLine(void)
{
    uint32_t currentTime = HAL_GetTick();
    
    if(!pidController.isOut) {
        pidController.isOut = true;
        pidController.outStartTime = currentTime;
        LED_On(LED2);
    }
    
    // Check timeout
    if((currentTime - pidController.outStartTime) >= pidController.maxOutTime) {
        PID_Stop();
        LED_Blink(LED_ALL, 5, 100);
    } else {
        Motor_Stop();
    }
    
    return IQ32_OK;
}

IQ32_Result_t PID_PrintStatus(void)
{
    if(pidController.isRunning) {
        static uint32_t lastBlink = 0;
        if(HAL_GetTick() - lastBlink > 1000) {
            LED_Toggle(LED1);
            lastBlink = HAL_GetTick();
        }
    }
    return IQ32_OK;
}