#include "iq32_pid_advanced.h"
#include "iq32_mpu6500.h"
#include "iq32_led.h"
#include <math.h>
#include <string.h>

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

// --- Dynamic Speed Variables ---
static bool dynamicSpeedEnabled = false;
static float speedMultiplier = 1.0f;

// --- Emergency Stop Variables ---
static bool emergencyStopActive = false;

// --- Settings Structure for Flash Storage ---
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

// --- บันทึกการตั้งค่า ---
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
    
    // บันทึกลง Flash (ตัวอย่างการจำลอง)
    // HAL_FLASH_Unlock();
    // HAL_FLASH_Program(...);
    // HAL_FLASH_Lock();
    
    // แสดงสถานะด้วย LED
    for(int i = 0; i < 3; i++) {
        LED_On(LED1);
        LED_On(LED2);
        HAL_Delay(200);
        LED_Off(LED1);
        LED_Off(LED2);
        HAL_Delay(200);
    }
}

// --- โหลดการตั้งค่า ---
void PID_LoadSettings(void)
{
    // โหลดจาก Flash (ตัวอย่างการจำลอง)
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
            
            // แสดงสถานะสำเร็จ
            LED_On(LED1);
            HAL_Delay(1000);
            LED_Off(LED1);
            return;
        }
    }
    
    // ถ้าโหลดไม่สำเร็จ แสดงสถานะ error
    for(int i = 0; i < 5; i++) {
        LED_Toggle(LED2);
        HAL_Delay(100);
    }
    LED_Off(LED2);
}

// --- Auto Tuning Algorithm (Simplified Ziegler-Nichols) ---
void PID_AutoTune(void)
{
    // แสดงสถานะเริ่มต้น Auto Tune
    for(int i = 0; i < 10; i++) {
        LED_Toggle(LED1);
        LED_Toggle(LED2);
        HAL_Delay(100);
    }
    
    // หยุด PID ปัจจุบัน
    bool wasRunning = pidController.isRunning;
    PID_Stop();
    
    // ตั้งค่าเริ่มต้นสำหรับการทดสอบ
    float testKp = 0.1f;
    float ultimateKp = 0.0f;
    float ultimatePeriod = 0.0f;
    bool oscillationFound = false;
    
    pidController.kd = 0.0f;
    pidController.alpha = 0.9f;
    
    // ทดสอบหาค่า Kp ที่ทำให้เกิด oscillation
    for(int attempt = 0; attempt < 20 && !oscillationFound; attempt++) {
        pidController.kp = testKp;
        
        uint32_t testStartTime = HAL_GetTick();
        int16_t lastError = 0;
        uint32_t oscillationCount = 0;
        uint32_t lastOscillationTime = testStartTime;
        
        PID_Start();
        
        while(HAL_GetTick() - testStartTime < 5000) { // ทดสอบ 5 วินาที
            PID_Update();
            
            // ตรวจสอบการเปลี่ยนทิศทาง error
            if(pidController.error * lastError < 0) {
                oscillationCount++;
                if(oscillationCount > 1) {
                    uint32_t period = HAL_GetTick() - lastOscillationTime;
                    lastOscillationTime = HAL_GetTick();
                    
                    if(oscillationCount > 8) { // มี oscillation เพียงพอ
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
        
        // แสดงความคืบหน้าด้วย LED
        LED_Toggle(LED1);
    }
    
    if(oscillationFound && ultimateKp > 0) {
        // คำนวณค่า PID ตาม Ziegler-Nichols
        float tuned_kp = 0.6f * ultimateKp;
        float tuned_kd = (tuned_kp * ultimatePeriod) / 8000.0f; // แปลง ms เป็น s
        
        PID_SetParameters(tuned_kp, tuned_kd, 0.8f);
        
        // แสดงสถานะสำเร็จ
        for(int i = 0; i < 5; i++) {
            LED_On(LED1);
            LED_On(LED2);
            HAL_Delay(300);
            LED_Off(LED1);
            LED_Off(LED2);
            HAL_Delay(300);
        }
    } else {
        // Auto tune ล้มเหลว
        for(int i = 0; i < 10; i++) {
            LED_Toggle(LED2);
            HAL_Delay(100);
        }
        LED_Off(LED2);
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
    if(!adaptiveEnabled || !pidController.isRunning) return;
    
    // อ่านค่า IMU เพื่อประเมินความเร่ง
    float ax, ay, az, gx, gy, gz;
    if(MPU6500_ReadAccelGyro(&ax, &ay, &az, &gx, &gy, &gz)) {
        
        // คำนวณความเร่งรวม
        float totalAccel = sqrtf(ax*ax + ay*ay + az*az);
        float angularVelocity = sqrtf(gx*gx + gy*gy + gz*gz);
        
        // ปรับ Kp ตามความเร่ง
        if(totalAccel > 1.5f) { // High acceleration
            pidController.kp = adaptiveKp * 0.6f;
            pidController.kd = adaptiveKd * 1.4f;
        } else if(totalAccel < 0.7f) { // Low acceleration  
            pidController.kp = adaptiveKp * 1.3f;
            pidController.kd = adaptiveKd * 0.7f;
        } else { // Normal acceleration
            pidController.kp = adaptiveKp;
            pidController.kd = adaptiveKd;
        }
        
        // ปรับตามความเร็วเชิงมุม
        if(angularVelocity > 150.0f) {
            pidController.kd *= 0.4f; // ลด Kd เมื่อหมุนเร็ว
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
                (int16_t)((targetBaseSpeed - startBaseSpeed) * progress);
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
            performance.averageError = (performance.averageError * 0.9f) + (absError * 0.1f);
            
            if(absError > performance.maxError) {
                performance.maxError = absError;
            }
            
            // นับ oscillation
            static int16_t lastError = 0;
            if(pidController.error * lastError < 0 && abs(pidController.error) > 500) {
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

// --- Dynamic Speed Control ---
void PID_EnableDynamicSpeed(bool enable)
{
    dynamicSpeedEnabled = enable;
}

void PID_UpdateDynamicSpeed(void)
{
    if(!dynamicSpeedEnabled || !pidController.isRunning) return;
    
    // ปรับความเร็วตาม error ขนาด
    float errorMagnitude = fabsf((float)pidController.error);
    
    if(errorMagnitude > 3000) {
        speedMultiplier = 0.6f; // ลดความเร็วเมื่อ error สูง
    } else if(errorMagnitude < 500) {
        speedMultiplier = 1.2f; // เพิ่มความเร็วเมื่อ error ต่ำ
    } else {
        speedMultiplier = 1.0f; // ความเร็วปกติ
    }
    
    // จำกัดค่า multiplier
    if(speedMultiplier > 1.5f) speedMultiplier = 1.5f;
    if(speedMultiplier < 0.3f) speedMultiplier = 0.3f;
    
    // อัพเดทความเร็วฐาน
    pidController.baseSpeed = (int16_t)(pidController.baseSpeed * speedMultiplier);
}

// --- Emergency Stop ---
void PID_EmergencyStop(void)
{
    emergencyStopActive = true;
    PID_Stop();
    
    // แสดงสถานะ emergency stop ด้วย LED กะพริบเร็ว
    for(int i = 0; i < 20; i++) {
        LED_Toggle(LED1);
        LED_Toggle(LED2);
        HAL_Delay(50);
    }
}

bool PID_IsEmergencyStop(void)
{
    return emergencyStopActive;
}

void PID_ClearEmergencyStop(void)
{
    emergencyStopActive = false;
    LED_Off(LED1);
    LED_Off(LED2);
}