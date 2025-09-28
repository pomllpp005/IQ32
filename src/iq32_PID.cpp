#include "iq32_pid.h"
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// --- Config ---
#define NUM_SENSORS 16
#define SENSOR_CENTER 7000  // (NUM_SENSORS - 1) * 1000 / 2
#define TURN_SPEED 200
#define CONSTRAIN(x,a,b) ((x)<(a)?(a):((x)>(b)?(b):(x)))
uint16_t sensorValues[NUM_SENSORS];
// --- PID Controller ---
PIDController_t pidController;
int lastDetectedSide = 0;   // 0 = left, 1 = right

IQ32_Result_t PID_Reset(void) {
    memset(&pidController, 0, sizeof(pidController));
    pidController.lastUpdateTime = HAL_GetTick();
    return IQ32_OK;
}

IQ32_Result_t PID_SetParameters(float kp, float kd, float alpha) {
    if(kp < 0 || kd < 0 || alpha < 0 || alpha > 1) return IQ32_INVALID_PARAM;
    pidController.kp = kp; 
    pidController.kd = kd; 
    pidController.alpha = alpha;
    return IQ32_OK;
}

int32_t PID_Calculate(int16_t error) {
    pidController.filteredError = pidController.alpha * error +
                                  (1 - pidController.alpha) * pidController.filteredError;
    float dError = pidController.filteredError - pidController.previousFilteredError;

    int32_t u = (int32_t)(pidController.kp * pidController.filteredError +
                          pidController.kd * dError);

    pidController.previousFilteredError = pidController.filteredError;
    pidController.previousError = error;
    pidController.controlSignal = u;
    return u;
}

// ------------------ Main Algorithm ------------------
IQ32_Result_t iq32_PID(int16_t baseSpeed, int16_t maxSpeed, float kp, float kd) {
    PID_Reset();
    PID_SetParameters(kp, kd, 0.9f);
    pidController.baseSpeed = baseSpeed;
    pidController.maxSpeed  = maxSpeed;
    pidController.isRunning = true;

    while(pidController.isRunning) {
        uint16_t pos = LineSensor_ReadPosition();
        bool onLine = LineSensor_IsOnLine();

        if(onLine && pos && pos != (NUM_SENSORS - 1) * 1000) {
            if(sensorValues[0]) lastDetectedSide = 0;
            if(sensorValues[NUM_SENSORS-1]) lastDetectedSide = 1;

            pidController.error = pos - SENSOR_CENTER;
            pidController.controlSignal = PID_Calculate(pidController.error);

            int motorL = CONSTRAIN(baseSpeed + pidController.controlSignal, -100, maxSpeed);
            int motorR = CONSTRAIN(baseSpeed - pidController.controlSignal, -100, maxSpeed);

            Motor1(motorL);
            Motor2(motorR);
        } else {
            if(lastDetectedSide == 0) {
                Motor1(-TURN_SPEED);
                Motor2(maxSpeed);
            } else {
                Motor1(maxSpeed);
                Motor2(-TURN_SPEED);
            }
        }

        HAL_Delay(10);
    }
    return IQ32_OK;
}

// ------------------ เวอร์ชันจำกัดเวลา ------------------
IQ32_Result_t iq32_PIDtime(int16_t baseSpeed, int16_t maxSpeed, float kp, float kd, uint32_t runTimeMs) {
    PID_Reset();
    PID_SetParameters(kp, kd, 0.9f);
    pidController.baseSpeed = baseSpeed;
    pidController.maxSpeed  = maxSpeed;
    pidController.isRunning = true;

    uint32_t start = HAL_GetTick();
    while((HAL_GetTick() - start) < runTimeMs && pidController.isRunning) {
        uint16_t pos = LineSensor_ReadPosition();
        bool onLine = LineSensor_IsOnLine();

        if(onLine && pos && pos != (NUM_SENSORS - 1) * 1000) 
        {
            if(lineSensor.sensorValues[0]) lastDetectedSide = 0;
            if(lineSensor.sensorValues[NUM_SENSORS-1]) lastDetectedSide = 1;

            pidController.error = pos - SENSOR_CENTER;
            pidController.controlSignal = PID_Calculate(pidController.error);

            int motorL = CONSTRAIN(baseSpeed + pidController.controlSignal, -100, maxSpeed);
            int motorR = CONSTRAIN(baseSpeed - pidController.controlSignal, -100, maxSpeed);

            Motor1(motorL);
            Motor2(motorR);
        } else {
            if(lastDetectedSide == 0) {
                Motor1(-TURN_SPEED);
                Motor2(maxSpeed);
            } else {
                Motor1(maxSpeed);
                Motor2(-TURN_SPEED);
            }
        }

        
    }

    pidController.isRunning = false;
    Motor1(0);
    Motor2(0);
    return IQ32_OK;
}

// ------------------ Advanced ------------------
// IQ32_Result_t PID_DynamicSpeed(void) {
//     float curve = fabsf((float)pidController.error / SENSOR_CENTER);
//     int16_t dynSp = CONSTRAIN(pidController.baseSpeed * (1 - 0.5f * curve),
//                               pidController.baseSpeed/2, pidController.maxSpeed);

//     Motor1(dynSp - pidController.controlSignal);
//     Motor2(dynSp + pidController.controlSignal);
//     return IQ32_OK;
// }

// IQ32_Result_t PID_AutoTune(void) {
//     pidController.kp *= 1.05f; 
//     pidController.kd *= 0.95f;
//     return IQ32_OK;
// }

// IQ32_Result_t PID_PerformanceAnalysis(void) {
//     printf("Err:%d Ctrl:%ld\n", pidController.error, pidController.controlSignal);
//     return IQ32_OK;
// }

// IQ32_Result_t PID_EmergencyStop(void) {
//     Motor1(0);
//     Motor2(0);
//     pidController.isRunning = false;
//     return IQ32_OK;
// }
