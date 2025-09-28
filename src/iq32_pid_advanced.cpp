
// ========== iq32_pid_advanced.cpp ==========
#include "iq32_pid_advanced.h"
#include "iq32_mpu6500.h"
#include "iq32_led.h"
#include <math.h>
#include <string.h>

// Advanced control variables
static bool adaptiveEnabled = false;
static bool dynamicSpeedEnabled = false;
static bool performanceAnalysisActive = false;
static bool emergencyStopActive = false;
static bool smoothStartActive = false;
static bool smoothStopActive = false;

// Adaptive parameters
static float adaptiveKp = 0.0f;
static float adaptiveKd = 0.0f;

// Performance data
static PIDPerformance_t performance;
static uint32_t performanceStartTime = 0;

// Smooth control parameters
static uint32_t rampStartTime = 0;
static uint32_t rampDuration = 0;
static int16_t targetBaseSpeed = 0;
static int16_t startBaseSpeed = 0;

// Dynamic speed
static float speedMultiplier = 1.0f;

// Settings structure for flash storage
#define SETTINGS_MAGIC_NUMBER 0xABCD1234

typedef struct {
    uint32_t magic;
    float kp, kd, alpha;
    int16_t baseSpeed, maxSpeed, fanSpeed;
    uint16_t threshold;
    uint32_t checksum;
} PIDSettings_t;

static uint32_t CalculateChecksum(const PIDSettings_t* settings)
{
    uint32_t checksum = 0;
    const uint8_t* data = (const uint8_t*)settings;
    for(size_t i = 0; i < sizeof(PIDSettings_t) - sizeof(uint32_t); i++) {
        checksum += data[i];
    }
    return checksum;
}

IQ32_Result_t PID_SaveSettings(void)
{
    PIDSettings_t settings = {
        .magic = SETTINGS_MAGIC_NUMBER,
        .kp = pidController.kp,
        .kd = pidController.kd,
        .alpha = pidController.alpha,
        .baseSpeed = pidController.baseSpeed,
        .maxSpeed = pidController.maxSpeed,
        .fanSpeed = pidController.fanSpeed,
        .threshold = lineSensor.threshold
    };
    
    settings.checksum = CalculateChecksum(&settings);
    
    // Flash programming would go here
    // For now, just provide visual feedback
    LED_Blink(LED_ALL, 3, 200);
    return IQ32_OK;
}

IQ32_Result_t PID_LoadSettings(void)
{
    // Flash reading would go here
    // For now, just provide visual feedback
    LED_On(LED1);
    HAL_Delay(1000);
    LED_Off(LED1);
    return IQ32_OK;
}

IQ32_Result_t PID_AutoTune(void)
{
    LED_Blink(LED_ALL, 10, 100);
    
    bool wasRunning = pidController.isRunning;
    if(wasRunning) PID_Stop();
    
    float testKp = 0.1f;
    float ultimateKp = 0.0f;
    bool oscillationFound = false;
    
    pidController.kd = 0.0f;
    pidController.alpha = 0.9f;
    
    for(int attempt = 0; attempt < AUTOTUNE_MAX_ATTEMPTS && !oscillationFound; attempt++) {
        pidController.kp = testKp;
        
        uint32_t testStartTime = HAL_GetTick();
        int16_t lastError = 0;
        uint32_t oscillationCount = 0;
        
        PID_Start();
        
        while(HAL_GetTick() - testStartTime < AUTOTUNE_TEST_DURATION) {
            PID_Update();
            
            if(pidController.error * lastError < 0) {
                oscillationCount++;
                if(oscillationCount > AUTOTUNE_MIN_OSC_COUNT) {
                    ultimateKp = testKp;
                    oscillationFound = true;
                    break;
                }
            }
            
            lastError = pidController.error;
            HAL_Delay(10);
        }
        
        PID_Stop();
        testKp += 0.1f;
        LED_Toggle(LED1);
    }
    
    if(oscillationFound && ultimateKp > 0) {
        float tuned_kp = ZIEGLER_NICHOLS_KP * ultimateKp;
        float tuned_kd = tuned_kp / ZIEGLER_NICHOLS_KD_DIV;
        
        PID_SetParameters(tuned_kp, tuned_kd, 0.8f);
        LED_Blink(LED_ALL, 5, 300);
    } else {
        LED_Blink(LED2, 10, 100);
    }
    
    if(wasRunning) PID_Start();
    return oscillationFound ? IQ32_OK : IQ32_ERROR;
}

IQ32_Result_t PID_EnableAdaptive(bool enable)
{
    adaptiveEnabled = enable;
    if(enable) {
        adaptiveKp = pidController.kp;
        adaptiveKd = pidController.kd;
    }
    return IQ32_OK;
}

IQ32_Result_t PID_UpdateAdaptive(void)
{
    if(!adaptiveEnabled || !pidController.isRunning) return IQ32_OK;
    
    float ax, ay, az, gx, gy, gz;
    if(MPU6500_ReadAccelGyro(&ax, &ay, &az, &gx, &gy, &gz) == IQ32_OK) {
        float totalAccel = sqrtf(ax*ax + ay*ay + az*az);
        float angularVelocity = sqrtf(gx*gx + gy*gy + gz*gz);
        
        if(totalAccel > HIGH_ACCEL_THRESHOLD) {
            pidController.kp = adaptiveKp * ADAPTIVE_KP_HIGH_ACCEL;
            pidController.kd = adaptiveKd * ADAPTIVE_KD_HIGH_ACCEL;
        } else if(totalAccel < LOW_ACCEL_THRESHOLD) {
            pidController.kp = adaptiveKp * ADAPTIVE_KP_LOW_ACCEL;
            pidController.kd = adaptiveKd * ADAPTIVE_KD_LOW_ACCEL;
        } else {
            pidController.kp = adaptiveKp;
            pidController.kd = adaptiveKd;
        }
        
        if(angularVelocity > HIGH_GYRO_THRESHOLD) {
            pidController.kd *= ADAPTIVE_KD_HIGH_GYRO;
        }
    }
    
    return IQ32_OK;
}

IQ32_Result_t PID_SmoothStart(uint32_t rampTime)
{
    if(pidController.isRunning) return IQ32_ERROR;
    
    smoothStartActive = true;
    rampStartTime = HAL_GetTick();
    rampDuration = rampTime;
    startBaseSpeed = 0;
    targetBaseSpeed = pidController.baseSpeed;
    
    return PID_Start();
}

IQ32_Result_t PID_SmoothStop(uint32_t rampTime)
{
    if(!pidController.isRunning) return IQ32_ERROR;
    
    smoothStopActive = true;
    rampStartTime = HAL_GetTick();
    rampDuration = rampTime;
    startBaseSpeed = pidController.baseSpeed;
    targetBaseSpeed = 0;
    
    return IQ32_OK;
}

IQ32_Result_t PID_UpdateSmoothControl(void)
{
    uint32_t currentTime = HAL_GetTick();
    
    if(smoothStartActive || smoothStopActive) {
        uint32_t elapsed = currentTime - rampStartTime;
        
        if(elapsed >= rampDuration) {
            pidController.baseSpeed = targetBaseSpeed;
            smoothStartActive = false;
            
            if(smoothStopActive) {
                smoothStopActive = false;
                return PID_Stop();
            }
        } else {
            float progress = (float)elapsed / (float)rampDuration;
            pidController.baseSpeed = startBaseSpeed + 
                (int16_t)((targetBaseSpeed - startBaseSpeed) * progress);
        }
    }
    
    return IQ32_OK;
}

IQ32_Result_t PID_StartPerformanceAnalysis(void)
{
    PID_ResetPerformanceData();
    performanceAnalysisActive = true;
    performanceStartTime = HAL_GetTick();
    return IQ32_OK;
}

IQ32_Result_t PID_UpdatePerformanceAnalysis(void)
{
    if(!performanceAnalysisActive) return IQ32_OK;
    
    uint32_t currentTime = HAL_GetTick();
    performance.totalTime = currentTime - performanceStartTime;
    
    if(pidController.isRunning) {
        if(lineSensor.isOnLine) {
            performance.onLineTime += PERF_UPDATE_INTERVAL;
            
            float absError = fabsf((float)pidController.error);
            performance.averageError = (performance.averageError * 0.9f) + (absError * 0.1f);
            
            if(absError > performance.maxError) {
                performance.maxError = absError;
            }
            
            static int16_t lastError = 0;
            if(pidController.error * lastError < 0 && ABS(pidController.error) > MAX_ERROR_THRESHOLD) {
                performance.oscillationCount++;
            }
            lastError = pidController.error;
            
        } else {
            performance.offLineTime += PERF_UPDATE_INTERVAL;
        }
    }
    
    if(performance.totalTime > 0) {
        performance.efficiency = ((float)performance.onLineTime / (float)performance.totalTime) * 100.0f;
    }
    
    return IQ32_OK;
}

PIDPerformance_t PID_GetPerformanceData(void)
{
    return performance;
}

IQ32_Result_t PID_ResetPerformanceData(void)
{
    memset(&performance, 0, sizeof(PIDPerformance_t));
    return IQ32_OK;
}

IQ32_Result_t PID_EnableDynamicSpeed(bool enable)
{
    dynamicSpeedEnabled = enable;
    return IQ32_OK;
}

IQ32_Result_t PID_UpdateDynamicSpeed(void)
{
    if(!dynamicSpeedEnabled || !pidController.isRunning) return IQ32_OK;
    
    float errorMagnitude = fabsf((float)pidController.error);
    
    if(errorMagnitude > 3000) {
        speedMultiplier = 0.6f;
    } else if(errorMagnitude < 500) {
        speedMultiplier = 1.2f;
    } else {
        speedMultiplier = 1.0f;
    }
    
    speedMultiplier = CONSTRAIN(speedMultiplier, 0.3f, 1.5f);
    pidController.baseSpeed = (int16_t)(pidController.baseSpeed * speedMultiplier);
    
    return IQ32_OK;
}

IQ32_Result_t PID_EmergencyStop(void)
{
    emergencyStopActive = true;
    PID_Stop();
    LED_Blink(LED_ALL, EMERGENCY_BLINK_COUNT, EMERGENCY_BLINK_DELAY);
    return IQ32_OK;
}

bool PID_IsEmergencyStop(void)
{
    return emergencyStopActive;
}

IQ32_Result_t PID_ClearEmergencyStop(void)
{
    emergencyStopActive = false;
    LED_Off(LED_ALL);
    return IQ32_OK;
}