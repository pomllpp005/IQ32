#ifndef __IQ32_PID_H
#define __IQ32_PID_H

#include "iq32_board.h"
#include "iq32_motor.h"
#include "iq32_readLine.h"


typedef struct {
    float kp, kd, alpha;
    int16_t error;
    float filteredError, previousFilteredError;
    int16_t previousError;
    int32_t controlSignal;
    int16_t baseSpeed, maxSpeed, fanSpeed;
    bool isRunning;
    uint32_t lastUpdateTime;
} PIDController_t;

extern PIDController_t pidController;

IQ32_Result_t PID_Reset(void);
IQ32_Result_t PID_SetParameters(float kp, float kd, float alpha);
int32_t PID_Calculate(int16_t error);
IQ32_Result_t iq32_PID(int16_t baseSpeed, int16_t maxSpeed, float kp, float kd);
IQ32_Result_t iq32_PIDtime(int16_t baseSpeed, int16_t maxSpeed, float kp, float kd, uint32_t runTimeMs);



// --- Advanced Features ---
IQ32_Result_t PID_DynamicSpeed(void);
IQ32_Result_t PID_AutoTune(void);
IQ32_Result_t PID_PerformanceAnalysis(void);
IQ32_Result_t PID_EmergencyStop(void);

#endif
