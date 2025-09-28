// ========== iq32_pid_advanced.h ==========
#ifndef __IQ32_PID_ADVANCED_H
#define __IQ32_PID_ADVANCED_H

#include "iq32_pid_improved.h"

// Performance analysis structure
typedef struct {
    uint32_t totalTime;
    uint32_t onLineTime;
    uint32_t offLineTime;
    float averageError;
    float maxError;
    uint32_t oscillationCount;
    float efficiency;
} PIDPerformance_t;

// Settings management
IQ32_Result_t PID_SaveSettings(void);
IQ32_Result_t PID_LoadSettings(void);

// Auto tuning
IQ32_Result_t PID_AutoTune(void);

// Adaptive PID
IQ32_Result_t PID_EnableAdaptive(bool enable);
IQ32_Result_t PID_UpdateAdaptive(void);

// Smooth control
IQ32_Result_t PID_SmoothStart(uint32_t rampTime);
IQ32_Result_t PID_SmoothStop(uint32_t rampTime);
IQ32_Result_t PID_UpdateSmoothControl(void);

// Performance analysis
IQ32_Result_t PID_StartPerformanceAnalysis(void);
IQ32_Result_t PID_UpdatePerformanceAnalysis(void);
PIDPerformance_t PID_GetPerformanceData(void);
IQ32_Result_t PID_ResetPerformanceData(void);

// Dynamic speed control
IQ32_Result_t PID_EnableDynamicSpeed(bool enable);
IQ32_Result_t PID_UpdateDynamicSpeed(void);

// Emergency stop
IQ32_Result_t PID_EmergencyStop(void);
bool PID_IsEmergencyStop(void);
IQ32_Result_t PID_ClearEmergencyStop(void);

#endif
