// main.cpp - การใช้งาน PID Controller แบบง่าย (ไม่ใช้ OLED)
#include "iq32_board.h"
#include "iq32_pid_improved.h"
#include "iq32_linesensor.h"

int main()
{
    // เริ่มต้นบอร์ด
    IQ32_Init();
    
    // เริ่มต้น PID Controller
    PID_Init();
    
    // ตั้งค่าพารามิเตอร์ PID
    PID_SetParameters(0.8f, 0.15f, 0.7f);  // Kp, Kd, Alpha
    
    // ตั้งค่าความเร็ว
    PID_SetSpeeds(250, 800, 200);  // Base Speed, Max Speed, Fan Speed
    
    // ตั้งค่า Threshold เซ็นเซอร์
    LineSensor_SetThreshold(2000);
    
    // รอกดปุ่ม SW1 เพื่อเริ่มการ Calibrate
    // LED1 และ LED2 จะติดแสดงว่าพร้อม calibrate
    LED_On(LED1);
    LED_On(LED2);
    Wait_SW1();
    LED_Off(LED1);
    LED_Off(LED2);
    
    // ทำการ Calibrate เซ็นเซอร์ (5 วินาที)
    // LED จะกะพริบแสดงสถานะ calibration
    LineSensor_Calibrate(5000);
    
    // รอกดปุ่ม SW1 เพื่อเริ่มการควบคุม
    // LED1 กะพริบแสดงว่าพร้อมเริ่มต้น
    for(int i = 0; i < 5; i++) {
        LED_On(LED1);
        HAL_Delay(200);
        LED_Off(LED1);
        HAL_Delay(200);
    }
    Wait_SW1();
    
    // เริ่มการควบคุม PID
    PID_Start();
    
    // Loop หลัก
    while(1) {
        // ตรวจสอบปุ่มกด
        if(Read_SW1()) {
            if(pidController.isRunning) {
                PID_Stop();
            } else {
                PID_Start();
            }
            HAL_Delay(300); // ป้องกันการกดซ้ำ
        }
        
        if(Read_SW2()) {
            // เปลี่ยนความเร็ว
            static uint8_t speedMode = 1; // เริ่มที่ medium
            speedMode = (speedMode + 1) % 3;
            
            switch(speedMode) {
                case 0: // Slow
                    PID_SetSpeeds(150, 500, 100);
                    // กะพริบ LED2 1 ครั้ง
                    LED_On(LED2);
                    HAL_Delay(300);
                    LED_Off(LED2);
                    break;
                case 1: // Medium
                    PID_SetSpeeds(250, 800, 200);
                    // กะพริบ LED2 2 ครั้ง
                    for(int i = 0; i < 2; i++) {
                        LED_On(LED2);
                        HAL_Delay(150);
                        LED_Off(LED2);
                        HAL_Delay(150);
                    }
                    break;
                case 2: // Fast
                    PID_SetSpeeds(350, 1000, 300);
                    // กะพริบ LED2 3 ครั้ง
                    for(int i = 0; i < 3; i++) {
                        LED_On(LED2);
                        HAL_Delay(150);
                        LED_Off(LED2);
                        HAL_Delay(150);
                    }
                    break;
            }
            HAL_Delay(300);
        }
        
        // อัพเดท PID Controller
        PID_Update();
        
        HAL_Delay(10);
    }
    
    return 0;
}

// =============== ฟังก์ชันเสริม ===============

// ฟังก์ชันสำหรับตั้งค่า PID ด้วยปุ่มกด
void QuickPIDSetup()
{
    float kp = 0.5f;
    float kd = 0.1f;
    int16_t speed = 200;
    uint16_t threshold = 2000;
    
    // เข้าสู่โหมดตั้งค่า - LED กะพริบ 3 ครั้ง
    for(int i = 0; i < 3; i++) {
        LED_On(LED1);
        LED_On(LED2);
        HAL_Delay(200);
        LED_Off(LED1);
        LED_Off(LED2);
        HAL_Delay(200);
    }
    
    uint8_t paramMode = 0; // 0=Kp, 1=Kd, 2=Speed, 3=Threshold
    
    while(paramMode < 4) {
        // แสดงพารามิเตอร์ปัจจุบัน
        LED_Off(LED1);
        LED_Off(LED2);
        HAL_Delay(1000);
        
        // กะพริบ LED1 ตามจำนวน parameter (1-4 ครั้ง)
        for(int i = 0; i <= paramMode; i++) {
            LED_On(LED1);
            HAL_Delay(200);
            LED_Off(LED1);
            HAL_Delay(200);
        }
        
        uint32_t startTime = HAL_GetTick();
        bool valueChanged = false;
        
        // รอการปรับค่า 5 วินาที
        while(HAL_GetTick() - startTime < 5000 && !valueChanged) {
            if(Read_SW1()) {
                // เพิ่มค่า
                switch(paramMode) {
                    case 0: // Kp
                        kp += 0.1f;
                        if(kp > 3.0f) kp = 0.1f;
                        break;
                    case 1: // Kd
                        kd += 0.05f;
                        if(kd > 1.0f) kd = 0.01f;
                        break;
                    case 2: // Speed
                        speed += 50;
                        if(speed > 500) speed = 100;
                        break;
                    case 3: // Threshold
                        threshold += 200;
                        if(threshold > 4000) threshold = 500;
                        break;
                }
                
                // แสดงการเปลี่ยนแปลงด้วย LED2
                LED_On(LED2);
                HAL_Delay(150);
                LED_Off(LED2);
                valueChanged = true;
                HAL_Delay(300);
            }
            
            if(Read_SW2()) {
                // ไปพารามิเตอร์ถัดไป
                paramMode++;
                valueChanged = true;
                HAL_Delay(300);
            }
        }
        
        // ถ้าไม่มีการกดปุ่มใน 5 วินาที ไปพารามิเตอร์ถัดไป
        if(!valueChanged) {
            paramMode++;
        }
    }
    
    // บันทึกค่าที่ตั้ง
    PID_SetParameters(kp, kd, 0.8f);
    PID_SetSpeeds(speed, speed * 3, speed);
    LineSensor_SetThreshold(threshold);
    
    // แสดงการบันทึกเสร็จสิ้น - LED กะพริบเร็ว
    for(int i = 0; i < 10; i++) {
        LED_Toggle(LED1);
        LED_Toggle(LED2);
        HAL_Delay(100);
    }
    LED_Off(LED1);
    LED_Off(LED2);
}

// ฟังก์ชันสำหรับทดสอบมอเตอร์
void TestMotors()
{
    // ทดสอบมอเตอร์ซ้าย
    LED_On(LED1);
    Motor1(300);
    HAL_Delay(1000);
    Motor1(0);
    LED_Off(LED1);
    HAL_Delay(500);
    
    // ทดสอบมอเตอร์ขวา
    LED_On(LED2);
    Motor2(300);
    HAL_Delay(1000);
    Motor2(0);
    LED_Off(LED2);
    HAL_Delay(500);
    
    // ทดสอบทั้งสองมอเตอร์ไปข้างหน้า
    LED_On(LED1);
    LED_On(LED2);
    Motor1(200);
    Motor2(200);
    HAL_Delay(1000);
    Motor1(0);
    Motor2(0);
    LED_Off(LED1);
    LED_Off(LED2);
    HAL_Delay(500);
    
    // ทดสอบการหมุน
    Motor1(200);
    Motor2(-200);
    HAL_Delay(1000);
    Motor1(0);
    Motor2(0);
}

// ฟังก์ชันสำหรับทดสอบเซ็นเซอร์
void TestLineSensor()
{
    uint32_t startTime = HAL_GetTick();
    
    while(HAL_GetTick() - startTime < 10000) { // ทดสอบ 10 วินาที
        LineSensor_ReadRaw();
        
        // แสดงสถานะด้วย LED
        if(LineSensor_GetActiveSensorCount() > 0) {
            LED_On(LED2);
        } else {
            LED_Off(LED2);
        }
        
        // กะพริบ LED1 ทุกวินาที
        if((HAL_GetTick() - startTime) % 1000 < 100) {
            LED_On(LED1);
        } else {
            LED_Off(LED1);
        }
        
        HAL_Delay(50);
    }
    
    LED_Off(LED1);
    LED_Off(LED2);
}

// ฟังก์ชันแสดงสถานะแบตเตอรี่ด้วย LED
void ShowBatteryStatus()
{
    float voltage = Battery_GetVoltage_Avg();
    
    if(voltage > 12.0f) {
        // แบตเตอรี่เต็ม - LED กะพริบช้า
        for(int i = 0; i < 5; i++) {
            LED_On(LED1);
            LED_On(LED2);
            HAL_Delay(500);
            LED_Off(LED1);
            LED_Off(LED2);
            HAL_Delay(500);
        }
    } else if(voltage > 11.0f) {
        // แบตเตอรี่ปานกลาง - LED กะพริบปานกลาง
        for(int i = 0; i < 10; i++) {
            LED_Toggle(LED1);
            HAL_Delay(300);
        }
        LED_Off(LED1);
    } else {
        // แบตเตอรี่ต่ำ - LED กะพริบเร็ว
        for(int i = 0; i < 20; i++) {
            LED_Toggle(LED2);
            HAL_Delay(100);
        }
        LED_Off(LED2);
    }
}

// ฟังก์ชันแสดงค่า IMU ด้วย LED (ถ้ามี)
void ShowIMUStatus()
{
    float ax, ay, az, gx, gy, gz;
    
    if(MPU6500_ReadAccelGyro(&ax, &ay, &az, &gx, &gy, &gz)) {
        // คำนวณความเอียง
        float pitch = atan2f(ax, sqrtf(ay*ay + az*az)) * 180.0f / 3.14159f;
        float roll = atan2f(ay, az) * 180.0f / 3.14159f;
        
        // แสดงความเอียงด้วย LED
        if(fabsf(pitch) > 15.0f || fabsf(roll) > 15.0f) {
            // เอียงมาก - LED กะพริบเร็ว
            for(int i = 0; i < 10; i++) {
                LED_Toggle(LED1);
                LED_Toggle(LED2);
                HAL_Delay(100);
            }
        } else if(fabsf(pitch) > 5.0f || fabsf(roll) > 5.0f) {
            // เอียงเล็กน้อย - LED กะพริบช้า
            for(int i = 0; i < 3; i++) {
                LED_On(LED1);
                HAL_Delay(300);
                LED_Off(LED1);
                HAL_Delay(300);
            }
        } else {
            // ตั้งตรง - LED ติดค้าง
            LED_On(LED1);
            LED_On(LED2);
            HAL_Delay(1000);
        }
    }
    
    LED_Off(LED1);
    LED_Off(LED2);
}