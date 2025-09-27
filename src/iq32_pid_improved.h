#ifndef __IQ32_PID_H
#define __IQ32_PID_H

#include "stm32f4xx_hal.h"
#include "iq32_linesensor.h"
#include "iq32_motor.h"
#include "iq32_fan.h"
#include <stdint.h>
#include <stdbool.h>

// --- กำหนดค่าคงที่ ---
#define DEFAULT_MAX_SPEED        800
#define DEFAULT_MIN_SPEED       -800
#define MAX_CONTROL_SIGNAL       800
#define MIN_CONTROL_SIGNAL      -800

// --- โครงสร้างข้อมูล PID ---
typedef struct {
    // PID Parameters
    float kp;
    float kd;
    float alpha;  // Filter coefficient
    
    // Speed Parameters
    int16_t baseSpeed;
    int16_t maxSpeed;
    int16_t fanSpeed;
    
    // PID Variables
    float filteredError;
    float previousFilteredError;
    int16_t error;
    int16_t previousError;
    
    // Control Variables
    int32_t controlSignal;
    
    // Motor Variables
    int16_t leftMotorSpeed;
    int16_t rightMotorSpeed;
    
    // Timing
    uint32_t lastUpdateTime;
    
    // Out of Line Control
    int16_t outLeftSpeed;
    int16_t outRightSpeed;
    uint32_t outStartTime;
    uint32_t maxOutTime;
    bool isOut;
    
    // Status
    bool isRunning;
    
} PIDController_t;

// --- ฟังก์ชันสำหรับ PID Controller ---

// การตั้งค่าเริ่มต้น
void PID_Init(void);
void PID_SetParameters(float kp, float kd, float alpha);
void PID_SetSpeeds(int16_t baseSpeed, int16_t maxSpeed, int16_t fanSpeed);
void PID_SetOutParams(int16_t leftSpeed, int16_t rightSpeed, uint32_t maxOutTimeMs);

// การควบคุม PID
void PID_Start(void);
void PID_Stop(void);
void PID_Reset(void);
void PID_Update(void);

// ฟังก์ชันช่วย
int32_t PID_Calculate(int16_t error);
void PID_UpdateMotors(bool isOnLine);
void PID_HandleOutOfLine(void);

// การแสดงผล
void PID_PrintStatus(void);

// --- ตัวแปร Global ---
extern PIDController_t pidController;

#endif

// ===== Implementation =====
#include "iq32_pid_improved.h"
#include "iq32_led.h"

// --- ตัวแปร Global ---
PIDController_t pidController;

// --- เริ่มต้น PID Controller ---
void PID_Init(void)
{
    // ตั้งค่า PID parameters เริ่มต้น
    pidController.kp = 0.5f;
    pidController.kd = 0.1f;
    pidController.alpha = 0.8f;
    
    // ตั้งค่าความเร็วเริ่มต้น
    pidController.baseSpeed = 200;
    pidController.maxSpeed = DEFAULT_MAX_SPEED;
    pidController.fanSpeed = 0;
    
    // Out of line parameters
    pidController.outLeftSpeed = 300;
    pidController.outRightSpeed = -300;
    pidController.maxOutTime = 500; // 500ms
    
    // เริ่มต้น Line Sensor
    LineSensor_Init();
    
    PID_Reset();
}

// --- ตั้งค่าพารามิเตอร์ PID ---
void PID_SetParameters(float kp, float kd, float alpha)
{
    pidController.kp = kp;
    pidController.kd = kd;
    pidController.alpha = alpha;
}

// --- ตั้งค่าความเร็ว ---
void PID_SetSpeeds(int16_t baseSpeed, int16_t maxSpeed, int16_t fanSpeed)
{
    pidController.baseSpeed = baseSpeed;
    pidController.maxSpeed = maxSpeed;
    pidController.fanSpeed = fanSpeed;
    
    // ตั้งค่า Fan
    Fan_SetSpeed(fanSpeed);
}

// --- ตั้งค่าการจัดการเมื่อออกจากเส้น ---
void PID_SetOutParams(int16_t leftSpeed, int16_t rightSpeed, uint32_t maxOutTimeMs)
{
    pidController.outLeftSpeed = leftSpeed;
    pidController.outRightSpeed = rightSpeed;
    pidController.maxOutTime = maxOutTimeMs;
}

// --- เริ่มการควบคุม PID ---
void PID_Start(void)
{
    pidController.isRunning = true;
    pidController.lastUpdateTime = HAL_GetTick();
    
    // เปิด Fan
    Fan_SetSpeed(pidController.fanSpeed);
    
    // แสดงสถานะ
    LED_On(LED1);
}

// --- หยุดการควบคุม PID ---
void PID_Stop(void)
{
    pidController.isRunning = false;
    
    // หยุดมอเตอร์ทั้งหมด
    Motor1(0);
    Motor2(0);
    Fan_SetSpeed(0);
    
    // ปิด LED
    LED_Off(LED1);
    LED_Off(LED2);
}

// --- Reset ค่า PID ---
void PID_Reset(void)
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
}

// --- อัพเดท PID Controller ---
void PID_Update(void)
{
    if(!pidController.isRunning) return;
    
    uint32_t currentTime = HAL_GetTick();
    
    // อ่านตำแหน่งเส้น
    uint16_t position = LineSensor_ReadPosition();
    bool isOnLine = LineSensor_IsOnLine();
    
    if(isOnLine) {
        // คำนวณ error (ตำแหน่งกึ่งกลาง = (NUM_SENSORS-1)*1000/2)
        int16_t centerPosition = (NUM_SENSORS - 1) * 1000 / 2;
        pidController.error = position - centerPosition;
        
        // คำนวณ control signal
        pidController.controlSignal = PID_Calculate(pidController.error);
        
        // อัพเดทมอเตอร์
        PID_UpdateMotors(true);
        
        // Reset out of line timer
        pidController.isOut = false;
        
        // กะพริบ LED เพื่อแสดงสถานะ
        if((currentTime / 100) % 10 < 5) {
            LED_On(LED2);
        } else {
            LED_Off(LED2);
        }
        
    } else {
        // จัดการเมื่อออกจากเส้น
        PID_HandleOutOfLine();
    }
    
    pidController.lastUpdateTime = currentTime;
}

// --- คำนวณ PID ---
int32_t PID_Calculate(int16_t error)
{
    // Filter error
    pidController.filteredError = pidController.alpha * (float)error + 
                                 (1.0f - pidController.alpha) * pidController.filteredError;
    
    // คำนวณ derivative
    float dError = pidController.filteredError - pidController.previousFilteredError;
    
    // คำนวณ control signal
    int32_t controlSignal = (int32_t)(pidController.kp * pidController.filteredError + 
                                     pidController.kd * dError);
    
    // จำกัด control signal
    if(controlSignal > MAX_CONTROL_SIGNAL) {
        controlSignal = MAX_CONTROL_SIGNAL;
    } else if(controlSignal < MIN_CONTROL_SIGNAL) {
        controlSignal = MIN_CONTROL_SIGNAL;
    }
    
    // อัพเดทค่าก่อนหน้า
    pidController.previousFilteredError = pidController.filteredError;
    pidController.previousError = error;
    
    return controlSignal;
}

// --- อัพเดทมอเตอร์ ---
void PID_UpdateMotors(bool isOnLine)
{
    if(isOnLine) {
        // คำนวณความเร็วมอเตอร์
        pidController.leftMotorSpeed = pidController.baseSpeed - pidController.controlSignal;
        pidController.rightMotorSpeed = pidController.baseSpeed + pidController.controlSignal;
        
        // จำกัดความเร็วไม่ให้เกิน maxSpeed
        if(pidController.leftMotorSpeed > pidController.maxSpeed) {
            pidController.leftMotorSpeed = pidController.maxSpeed;
        } else if(pidController.leftMotorSpeed < -pidController.maxSpeed) {
            pidController.leftMotorSpeed = -pidController.maxSpeed;
        }
        
        if(pidController.rightMotorSpeed > pidController.maxSpeed) {
            pidController.rightMotorSpeed = pidController.maxSpeed;
        } else if(pidController.rightMotorSpeed < -pidController.maxSpeed) {
            pidController.rightMotorSpeed = -pidController.maxSpeed;
        }
        
        // ส่งค่าไปยังมอเตอร์
        Motor1(pidController.leftMotorSpeed);   // มอเตอร์ซ้าย
        Motor2(pidController.rightMotorSpeed);  // มอเตอร์ขวา
        
    } else {
        // หยุดมอเตอร์เมื่อไม่เจอเส้น
        Motor1(0);
        Motor2(0);
        pidController.leftMotorSpeed = 0;
        pidController.rightMotorSpeed = 0;
    }
}

// --- จัดการเมื่อออกจากเส้น ---
void PID_HandleOutOfLine(void)
{
    uint32_t currentTime = HAL_GetTick();
    
    if(!pidController.isOut) {
        pidController.isOut = true;
        pidController.outStartTime = currentTime;
        LED_On(LED2); // แสดงสถานะออกจากเส้น
    }
    
    // ตรวจสอบ timeout
    if((currentTime - pidController.outStartTime) >= pidController.maxOutTime) {
        // หยุดหุ่นยนต์ถ้าออกจากเส้นนานเกินไป
        PID_Stop();
        
        // กะพริบ LED เพื่อแสดงว่าหยุดแล้ว
        for(int i = 0; i < 5; i++) {
            LED_On(LED1);
            LED_On(LED2);
            HAL_Delay(100);
            LED_Off(LED1);
            LED_Off(LED2);
            HAL_Delay(100);
        }
        
    } else {
        // หยุดหุ่นยนต์ชั่วคราว (ไม่หมุนตามทิศทางเดิม)
        Motor1(0);
        Motor2(0);
        pidController.leftMotorSpeed = 0;
        pidController.rightMotorSpeed = 0;
    }
}

// --- แสดงสถานะ PID (ไม่ใช้ OLED) ---
void PID_PrintStatus(void)
{
    // ฟังก์ชันนี้ไม่ทำอะไร หรือสามารถใช้ LED แสดงสถานะได้
    // หรือส่งข้อมูลผ่าน UART
    
    if(pidController.isRunning) {
        // กะพริบ LED1 เพื่อแสดงว่า PID กำลังทำงาน
        static uint32_t lastBlink = 0;
        if(HAL_GetTick() - lastBlink > 1000) {
            LED_Toggle(LED1);
            lastBlink = HAL_GetTick();
        }
    }
}