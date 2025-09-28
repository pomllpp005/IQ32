// ========== iq32_pid_improved.h ==========
#ifndef __IQ32_PID_H
#define __IQ32_PID_H

#include "stm32f4xx_hal.h"
#include "iq32_constants.h"
#include "iq32_linesensor.h"
#include "iq32_motor.h"
#include "iq32_fan.h"

typedef struct {
    // PID Parameters
    float kp, kd, alpha;
    
    // Speed Parameters
    int16_t baseSpeed, maxSpeed, fanSpeed;
    
    // PID Variables
    float filteredError, previousFilteredError;
    int16_t error, previousError;
    int32_t controlSignal;
    
    // Motor Variables
    int16_t leftMotorSpeed, rightMotorSpeed;
    
    // Timing
    uint32_t lastUpdateTime;
    
    // Out of Line Control
    int16_t outLeftSpeed, outRightSpeed;
    uint32_t outStartTime, maxOutTime;
    bool isOut, isRunning;
} PIDController_t;

extern PIDController_t pidController;

// Core Functions
IQ32_Result_t PID_Init(void);
IQ32_Result_t PID_SetParameters(float kp, float kd, float alpha);
IQ32_Result_t PID_SetSpeeds(int16_t baseSpeed, int16_t maxSpeed, int16_t fanSpeed);
IQ32_Result_t PID_SetOutParams(int16_t leftSpeed, int16_t rightSpeed, uint32_t maxOutTimeMs);

// Control Functions
IQ32_Result_t PID_Start(void);
IQ32_Result_t PID_Stop(void);
IQ32_Result_t PID_Reset(void);
IQ32_Result_t PID_Update(void);

// Helper Functions
int32_t PID_Calculate(int16_t error);
IQ32_Result_t PID_UpdateMotors(bool isOnLine);
IQ32_Result_t PID_HandleOutOfLine(void);
IQ32_Result_t PID_PrintStatus(void);

#endif
