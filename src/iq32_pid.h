// PIDController.h
#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include "stm32f4xx_hal.h"
#include "LineSensor.h"
// #include "motor_library.h"  // <- เพิ่ม include motor library ของคุณตรงนี้
#include <stdint.h>
#include <stdbool.h>

// --- กำหนดค่าคงที่ ---
#define MAX_SPEED                 255
#define MIN_SPEED                -255
#define MAX_CONTROL_SIGNAL       510
#define MIN_CONTROL_SIGNAL      -510

// --- โครงสร้างข้อมูล PID ---
typedef struct {
    // PID Parameters
    float kp;
    float kd;
    float alpha;  // Filter coefficient
    
    // PID Variables
    float filteredError;
    float previousFilteredError;
    int16_t error;
    int16_t previousError;
    
    // Control Variables
    int16_t baseSpeed;
    int16_t speedImp;
    int16_t speedDecrement;
    int32_t controlSignal;
    
    // Motor Variables
    int16_t motorA_PWM;
    int16_t motorB_PWM;
    bool isSpeedBase;
    
    // Timing
    uint32_t startTime;
    uint32_t speedTime;
    
    // Out of Line Control
    int16_t outOuterSpeed;
    int16_t outInnerSpeed;
    uint32_t outStartTime;
    uint32_t maxOutTime;
    bool isOut;
    
    // Cross Control
    bool correctCrossError;
    
} PIDController_t;

// --- ต้องมีการประกาศฟังก์ชัน motor ที่จะใช้ ---
extern void motor1(int16_t speed);  // ประกาศฟังก์ชัน motor1()
extern void motor2(int16_t speed);  // ประกาศฟังก์ชัน motor2()

// --- ฟังก์ชันสำหรับ PID Controller ---
void PID_Init(float kp, float kd, float alpha);
void PID_SetSpeed(int16_t baseSpeed, int16_t impellerSpeed);
void PID_SetOutParams(int16_t outerSpeed, int16_t innerSpeed, uint32_t maxOutTimeMs);
void PID_SetCrossCorrection(bool enable);
void PID_Reset(void);
void PID_StartSpeedRamp(uint32_t speedTimeMs);
int32_t PID_Calculate(uint16_t position, bool isOnLine, bool isOnCross);
void PID_UpdateMotors(bool isOnLine);

// --- Motor Functions (ใช้ library ที่มีอยู่) ---
// ฟังก์ชันเหล่านี้จะเรียกใช้ motor library ของคุณ
void PID_SetMotorA(int16_t speed);  // เรียก motor library function
void PID_SetMotorB(int16_t speed);  // เรียก motor library function  
void PID_SetImpeller(int16_t speed); // เรียก motor library function
void PID_StopMotors(void);          // เรียก motor library function
 
// --- ตัวแปร Global ---
extern PIDController_t pidController;

#endif // PID_CONTROLLER_H

// PIDController.c
#include "PIDController.h"
// #include "your_motor_library.h"  // <- include motor library ของคุณ
#include <stdio.h>

// --- ตัวแปร Global ---
PIDController_t pidController;

// --- เริ่มต้น PID Controller ---
void PID_Init(float kp, float kd, float alpha)
{
    // ตั้งค่า PID parameters
    pidController.kp = kp;
    pidController.kd = kd;
    pidController.alpha = alpha;
    
    // ค่าเริ่มต้น
    pidController.baseSpeed = 40;
    pidController.speedImp = 180;
    pidController.speedDecrement = 0;
    pidController.isSpeedBase = false;
    
    // Out parameters
    pidController.outOuterSpeed = 255;
    pidController.outInnerSpeed = -255;
    pidController.maxOutTime = 10; // 10ms
    pidController.isOut = false;
    
    // Cross correction
    pidController.correctCrossError = true;
    
    PID_Reset();
}

// --- ตั้งค่าความเร็ว ---
void PID_SetSpeed(int16_t baseSpeed, int16_t impellerSpeed)
{
    pidController.baseSpeed = baseSpeed;
    pidController.speedImp = impellerSpeed;
}

// --- ตั้งค่า Out Parameters ---
void PID_SetOutParams(int16_t outerSpeed, int16_t innerSpeed, uint32_t maxOutTimeMs)
{
    pidController.outOuterSpeed = outerSpeed;
    pidController.outInnerSpeed = innerSpeed;
    pidController.maxOutTime = maxOutTimeMs;
}

// --- ตั้งค่า Cross Correction ---
void PID_SetCrossCorrection(bool enable)
{
    pidController.correctCrossError = enable;
}

// --- Reset PID Values ---
void PID_Reset(void)
{
    pidController.filteredError = 0;
    pidController.previousFilteredError = 0;
    pidController.error = 0;
    pidController.previousError = 0;
    pidController.controlSignal = 0;
    pidController.motorA_PWM = 0;
    pidController.motorB_PWM = 0;
    pidController.startTime = HAL_GetTick();
    pidController.outStartTime = pidController.startTime;
    pidController.isOut = false;
}

// --- เริ่มต้น Speed Ramp ---
void PID_StartSpeedRamp(uint32_t speedTimeMs)
{
    pidController.speedTime = speedTimeMs;
    pidController.isSpeedBase = false;
    pidController.startTime = HAL_GetTick();
}

// --- คำนวณ PID ---
int32_t PID_Calculate(uint16_t position, bool isOnLine, bool isOnCross)
{
    uint32_t currentTime = HAL_GetTick();
    
    // Speed increment method
    if(!pidController.isSpeedBase) {
        uint32_t elapsed = currentTime - pidController.startTime;
        if(elapsed >= pidController.speedTime) {
            PID_SetImpeller(pidController.speedImp);
            pidController.speedDecrement = 0;
            pidController.isSpeedBase = true;
        } else {
            pidController.speedDecrement = pidController.baseSpeed - 
                                         (pidController.baseSpeed * elapsed / pidController.speedTime);
            if(elapsed >= pidController.speedTime / 2) {
                PID_SetImpeller(pidController.speedImp);
            }
        }
    }
    
    // คำนวณ error
    pidController.error = (pidController.correctCrossError && isOnCross) ? 0 : 
                         (position - (NUM_SENSORS - 1) * 1000 / 2);
    
    // Filter error
    pidController.filteredError = pidController.alpha * (float)pidController.error + 
                                 (1 - pidController.alpha) * pidController.filteredError;
    
    // คำนวณ derivative
    int16_t dxFilteredError = pidController.filteredError - pidController.previousFilteredError;
    
    // คำนวณ control signal
    pidController.controlSignal = (pidController.error * pidController.kp) + 
                                 ((int32_t)dxFilteredError * pidController.kd);
    
    // จำกัด control signal
    if(pidController.controlSignal < MIN_CONTROL_SIGNAL) {
        pidController.controlSignal = MIN_CONTROL_SIGNAL;
    } else if(pidController.controlSignal > MAX_CONTROL_SIGNAL) {
        pidController.controlSignal = MAX_CONTROL_SIGNAL;
    }
    
    // อัพเดทค่าก่อนหน้า
    pidController.previousFilteredError = pidController.filteredError;
    pidController.previousError = pidController.error;
    
    return pidController.controlSignal;
}

// --- อัพเดท Motors ---
void PID_UpdateMotors(bool isOnLine)
{
    uint32_t currentTime = HAL_GetTick();
    
    if(isOnLine) {
        // คำนวณ motor power
        pidController.motorA_PWM = pidController.baseSpeed + pidController.controlSignal - pidController.speedDecrement;
        pidController.motorB_PWM = pidController.baseSpeed - pidController.controlSignal - pidController.speedDecrement;
        
        // จำกัดค่า PWM
        if(pidController.motorA_PWM < MIN_SPEED) pidController.motorA_PWM = MIN_SPEED;
        else if(pidController.motorA_PWM > MAX_SPEED) pidController.motorA_PWM = MAX_SPEED;
        
        if(pidController.motorB_PWM < MIN_SPEED) pidController.motorB_PWM = MIN_SPEED;
        else if(pidController.motorB_PWM > MAX_SPEED) pidController.motorB_PWM = MAX_SPEED;
        
        // ตั้งค่า Motors
        PID_SetMotorA(pidController.motorA_PWM);
        PID_SetMotorB(pidController.motorB_PWM);
        
        // Reset out state
        pidController.isOut = false;
        pidController.outStartTime = currentTime;
        
    } else {
        // Out of line method
        if(!pidController.isOut) {
            pidController.isOut = true;
            pidController.outStartTime = currentTime;
        }
        
        // ตรวจสอบ timeout
        if(currentTime - pidController.outStartTime >= pidController.maxOutTime) {
            PID_StopMotors();
            return;
        }
        
        // หยุดหุ่นยนต์เมื่อไม่เจอเส้น (ไม่หมุนตามทิศทาง)
        PID_StopMotors();
        
        pidController.motorA_PWM = 0;
        pidController.motorB_PWM = 0;
    }
}

// --- ตั้งค่า Motor A ---
void PID_SetMotorA(int16_t speed)
{
    motor1(speed);  // ใช้ฟังก์ชัน motor1() ของคุณ
}

// --- ตั้งค่า Motor B ---
void PID_SetMotorB(int16_t speed)
{
    motor2(speed);  // ใช้ฟังก์ชัน motor2() ของคุณ
}

// --- ตั้งค่า Impeller ---
void PID_SetImpeller(int16_t speed)
{
    // ถ้ามี impeller/fan แยก ใส่ฟังก์ชันตรงนี้
    // เช่น motor3(speed); หรือ fan_control(speed);
    
    // ถ้าไม่มี impeller ก็ไม่ต้องทำอะไร
    // printf("Impeller Speed: %d\n", speed);
}

// --- หยุด Motors ---
void PID_StopMotors(void)
{
    motor1(0);    // หยุด motor A
    motor2(0);    // หยุด motor B
    PID_SetImpeller(0);  // หยุด impeller (ถ้ามี)
}