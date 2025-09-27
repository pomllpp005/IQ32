// iq32_pid_advanced.h - ฟีเจอร์ขั้นสูงสำหรับ PID Controller

#ifndef __IQ32_PID_ADVANCED_H
#define __IQ32_PID_ADVANCED_H

#include "iq32_pid_improved.h"

// --- ฟีเจอร์การบันทึกและโหลดค่า ---
void PID_SaveSettings(void);
void PID_LoadSettings(void);

// --- ฟีเจอร์ Auto Tuning ---
void PID_AutoTune(void);

// --- ฟีเจอร์ Adaptive PID ---
void PID_EnableAdaptive(bool enable);
void PID_UpdateAdaptive(void);

// --- ฟีเจอร์ Smooth Start/Stop ---
void PID_SmoothStart(uint32_t rampTime);
void PID_SmoothStop(uint32_t rampTime);

// --- ฟีเจอร์การวิเคราะห์ประสิทธิภาพ ---
typedef struct {
    uint32_t totalTime;
    uint32_t onLineTime;
    uint32_t offLineTime;
    float averageError;
    float maxError;
    uint32_t oscillationCount;
    float efficiency;
} PIDPerformance_t;

void PID_StartPerformanceAnalysis(void);
void PID_UpdatePerformanceAnalysis(void);
PIDPerformance_t PID_GetPerformanceData(void);
void PID_ResetPerformanceData(void);

#endif

// ===== Implementation =====
#include "iq32_pid_advanced.h"
#include "iq32_mpu6500.h"
#include <math.h>

// --- ตัวแปรสำหรับฟีเจอร์ขั้นสูง ---
static bool adaptiveEnabled = false;
static float adaptiveKp = 0.0f;
static float adaptiveKd = 0.0f;
static PIDPerformance_t performance;
static bool performanceAnalysisActive = false;
static uint32_t performanceStartTime = 0;

// --- Smooth Start/Stop Variables ---
static bool smoothStartActive = false;
static bool smoothStopActive = false;
static uint32_t rampStartTime = 0;
static uint32_t rampDuration = 0;
static int16_t targetBaseSpeed = 0;
static int16_t startBaseSpeed = 0;

// --- บันทึกและโหลดการตั้งค่า (ใช้ Flash หรือ EEPROM simulation) ---
#define SETTINGS_MAGIC_NUMBER 0xABCD1234
#define FLASH_SETTINGS_ADDRESS 0x08040000  // กำหนดตำแหน่ง Flash ที่ว่าง

typedef struct {
    uint32_t magic;
    float kp;
    float kd;
    float alpha;
    int16_t baseSpeed;
    int16_t maxSpeed;
    int16_t fanSpeed;
    uint16_t threshold;
    uint32_t checksum;
} PIDSettings_t;

void PID_SaveSettings(void)
{
    PIDSettings_t settings;
    settings.magic = SETTINGS_MAGIC_NUMBER;
    settings.kp = pidController.kp;
    settings.kd = pidController.kd;
    settings.alpha = pidController.alpha;
    settings.baseSpeed = pidController.baseSpeed;
    settings.maxSpeed = pidController.maxSpeed;
    settings.fanSpeed = pidController.fanSpeed;
    settings.threshold = lineSensor.threshold;
    
    // คำนวณ checksum
    uint32_t checksum = 0;
    uint8_t* data = (uint8_t*)&settings;
    for(int i = 0; i < sizeof(settings) - 4; i++) {
        checksum += data[i];
    }
    settings.checksum = checksum;
    
    // บันทึกลง Flash (ในตัวอย่างนี้จำลองเท่านั้น)
    // HAL_FLASH_Unlock();
    // HAL_FLASH_Program(...);
    // HAL_FLASH_Lock();
    
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 1, Font_6x8);
    ssd1306_WriteString("Settings Saved!", Font_6x8, White);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);
}

void PID_LoadSettings(void)
{
    // โหลดจาก Flash (ในตัวอย่างนี้จำลองเท่านั้น)
    PIDSettings_t* settings = (PIDSettings_t*)FLASH_SETTINGS_ADDRESS;
    
    if(settings->magic == SETTINGS_MAGIC_NUMBER) {
        // ตรวจสอบ checksum
        uint32_t checksum = 0;
        uint8_t* data = (uint8_t*)settings;
        for(int i = 0; i < sizeof(PIDSettings_t) - 4; i++) {
            checksum += data[i];
        }
        
        if(checksum == settings->checksum) {
            PID_SetParameters(settings->kp, settings->kd, settings->alpha);
            PID_SetSpeeds(settings->baseSpeed, settings->maxSpeed, settings->fanSpeed);
            LineSensor_SetThreshold(settings->threshold);
            
            ssd1306_Fill(Black);
            ssd1306_SetCursorLine(0, 1, Font_6x8);
            ssd1306_WriteString("Settings Loaded!", Font_6x8, White);
            ssd1306_UpdateScreen();
            HAL_Delay(1000);
            return;
        }
    }
    
    // ถ้าโหลดไม่สำเร็จ ใช้ค่า default
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 1, Font_6x8);
    ssd1306_WriteString("Using Defaults", Font_6x8, White);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);
}

// --- Auto Tuning Algorithm (Ziegler-Nichols Method) ---
void PID_AutoTune(void)
{
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 0, Font_6x8);
    ssd1306_WriteString("Auto Tuning...", Font_6x8, White);
    ssd1306_SetCursorLine(0, 1, Font_6x8);
    ssd1306_WriteString("Please wait", Font_6x8, White);
    ssd1306_UpdateScreen();
    
    // หยุด PID ปัจจุบัน
    bool wasRunning = pidController.isRunning;
    PID_Stop();
    
    // ตั้งค่า Kd = 0 และทดสอบหา Kp ที่ทำให้เกิด oscillation
    float testKp = 0.1f;
    float ultimateKp = 0.0f;
    float ultimatePeriod = 0.0f;
    bool oscillationFound = false;
    
    pidController.kd = 0.0f;
    pidController.alpha = 0.9f;
    
    for(int attempt = 0; attempt < 20 && !oscillationFound; attempt++) {
        pidController.kp = testKp;
        
        // ทดสอบ oscillation
        uint32_t testStartTime = HAL_GetTick();
        int16_t lastError = 0;
        int16_t errorDirection = 0;
        uint32_t oscillationCount = 0;
        uint32_t lastOscillationTime = testStartTime;
        
        PID_Start();
        
        while(HAL_GetTick() - testStartTime < 5000) { // ทดสอบ 5 วินาที
            PID_Update();
            
            if(pidController.error * lastError < 0) { // เปลี่ยนทิศทาง
                oscillationCount++;
                if(oscillationCount > 1) {
                    uint32_t period = HAL_GetTick() - lastOscillationTime;
                    lastOscillationTime = HAL_GetTick();
                    
                    if(oscillationCount > 10) { // มี oscillation เพียงพอ
                        ultimateKp = testKp;
                        ultimatePeriod = period * 2; // full cycle
                        oscillationFound = true;
                        break;
                    }
                }
            }
            
            lastError = pidController.error;
            HAL_Delay(10);
        }
        
        PID_Stop();
        testKp += 0.1f;
    }
    
    if(oscillationFound && ultimateKp > 0) {
        // คำนวณค่า PID ตาม Ziegler-Nichols
        float tuned_kp = 0.6f * ultimateKp;
        float tuned_kd = (tuned_kp * ultimatePeriod) / 8.0f;
        
        PID_SetParameters(tuned_kp, tuned_kd, 0.8f);
        
        ssd1306_Fill(Black);
        ssd1306_SetCursorLine(0, 0, Font_6x8);
        ssd1306_WriteString("Auto Tune Done!", Font_6x8, White);
        
        char kp_str[20];
        sprintf(kp_str, "Kp: %.3f", tuned_kp);
        ssd1306_SetCursorLine(0, 1, Font_6x8);
        ssd1306_WriteString(kp_str, Font_6x8, White);
        
        char kd_str[20];
        sprintf(kd_str, "Kd: %.3f", tuned_kd);
        ssd1306_SetCursorLine(0, 2, Font_6x8);
        ssd1306_WriteString(kd_str, Font_6x8, White);
        
        ssd1306_UpdateScreen();
        HAL_Delay(3000);
    } else {
        ssd1306_Fill(Black);
        ssd1306_SetCursorLine(0, 1, Font_6x8);
        ssd1306_WriteString("Auto Tune Failed", Font_6x8, White);
        ssd1306_UpdateScreen();
        HAL_Delay(2000);
    }
    
    if(wasRunning) {
        PID_Start();
    }
}

// --- Adaptive PID ---
void PID_EnableAdaptive(bool enable)
{
    adaptiveEnabled = enable;
    if(enable) {
        adaptiveKp = pidController.kp;
        adaptiveKd = pidController.kd;
    }
}

void PID_UpdateAdaptive(void)
{
    if(!adaptiveEnabled) return;
    
    // อ่านค่า IMU เพื่อประเมินความเร่ง
    float ax, ay, az, gx, gy, gz;
    if(MPU6500_ReadAccelGyro(&ax, &ay, &az, &gx, &gy, &gz)) {
        
        // คำนวณความเร่งรวม
        float totalAccel = sqrtf(ax*ax + ay*ay + az*az);
        float angularVelocity = sqrtf(gx*gx + gy*gy + gz*gz);
        
        // ปรับ Kp ตามความเร่ง (ความเร่งสูง = Kp ต่ำ)
        if(totalAccel > 1.2f) { // High acceleration
            pidController.kp = adaptiveKp * 0.7f;
            pidController.kd = adaptiveKd * 1.3f;
        } else if(totalAccel < 0.8f) { // Low acceleration
            pidController.kp = adaptiveKp * 1.2f;
            pidController.kd = adaptiveKd * 0.8f;
        } else { // Normal acceleration
            pidController.kp = adaptiveKp;
            pidController.kd = adaptiveKd;
        }
        
        // ปรับตามความเร็วเชิงมุม (หุ่นยนต์หมุนเร็ว)
        if(angularVelocity > 100.0f) {
            pidController.kd *= 0.5f; // ลด Kd เมื่อหมุนเร็ว
        }
    }
}

// --- Smooth Start/Stop ---
void PID_SmoothStart(uint32_t rampTime)
{
    if(pidController.isRunning) return;
    
    smoothStartActive = true;
    rampStartTime = HAL_GetTick();
    rampDuration = rampTime;
    startBaseSpeed = 0;
    targetBaseSpeed = pidController.baseSpeed;
    
    PID_Start();
}

void PID_SmoothStop(uint32_t rampTime)
{
    if(!pidController.isRunning) return;
    
    smoothStopActive = true;
    rampStartTime = HAL_GetTick();
    rampDuration = rampTime;
    startBaseSpeed = pidController.baseSpeed;
    targetBaseSpeed = 0;
}

// --- อัพเดท Smooth Control ใน PID_Update ---
void PID_UpdateSmoothControl(void)
{
    uint32_t currentTime = HAL_GetTick();
    
    if(smoothStartActive || smoothStopActive) {
        uint32_t elapsed = currentTime - rampStartTime;
        
        if(elapsed >= rampDuration) {
            // จบการ ramp
            pidController.baseSpeed = targetBaseSpeed;
            smoothStartActive = false;
            
            if(smoothStopActive) {
                smoothStopActive = false;
                PID_Stop();
                return;
            }
        } else {
            // คำนวณความเร็วแบบ interpolation
            float progress = (float)elapsed / (float)rampDuration;
            pidController.baseSpeed = startBaseSpeed + 
                (targetBaseSpeed - startBaseSpeed) * progress;
        }
    }
}

// --- Performance Analysis ---
void PID_StartPerformanceAnalysis(void)
{
    PID_ResetPerformanceData();
    performanceAnalysisActive = true;
    performanceStartTime = HAL_GetTick();
}

void PID_UpdatePerformanceAnalysis(void)
{
    if(!performanceAnalysisActive) return;
    
    uint32_t currentTime = HAL_GetTick();
    performance.totalTime = currentTime - performanceStartTime;
    
    if(pidController.isRunning) {
        if(lineSensor.isOnLine) {
            performance.onLineTime += 10; // สมมติว่าเรียกทุก 10ms
            
            // วิเคราะห์ error
            float absError = fabsf((float)pidController.error);
            performance.averageError = (performance.averageError + absError) / 2.0f;
            
            if(absError > performance.maxError) {
                performance.maxError = absError;
            }
            
            // นับ oscillation (เปลี่ยนทิศทาง error)
            static int16_t lastError = 0;
            if(pidController.error * lastError < 0) {
                performance.oscillationCount++;
            }
            lastError = pidController.error;
            
        } else {
            performance.offLineTime += 10;
        }
    }
    
    // คำนวณประสิทธิภาพ
    if(performance.totalTime > 0) {
        performance.efficiency = ((float)performance.onLineTime / (float)performance.totalTime) * 100.0f;
    }
}

PIDPerformance_t PID_GetPerformanceData(void)
{
    return performance;
}

void PID_ResetPerformanceData(void)
{
    memset(&performance, 0, sizeof(PIDPerformance_t));
    performance.maxError = 0.0f;
    performance.averageError = 0.0f;
}