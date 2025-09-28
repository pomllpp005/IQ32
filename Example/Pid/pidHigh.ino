// main.cpp - ตัวอย่างการใช้งาน PID Controller
#include "iq32_board.h"
#include "iq32_pid_improved.h"
#include "iq32_linesensor.h"
#include "ssd1306.h"

void setup()
{
    // เริ่มต้นบอร์ด
    IQ32_Init();
    
    // เริ่มต้น PID Controller
    PID_Init();
    
    // ตั้งค่าพารามิเตอร์ PID
    PID_SetParameters(0.8f, 0.15f, 0.7f);  // Kp, Kd, Alpha
    
    // ตั้งค่าความเร็ว
    PID_SetSpeeds(250, 800, 200);  // Base Speed, Max Speed, Fan Speed
    
    // ตั้งค่าการจัดการเมื่อออกจากเส้น
    PID_SetOutParams(300, -300, 1000);  // Left Speed, Right Speed, Timeout (ms)
    
    // รอกดปุ่ม SW1 เพื่อเริ่มการ Calibrate
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 1, Font_6x8);
    ssd1306_WriteString("Press SW1 to", Font_6x8, White);
    ssd1306_SetCursorLine(0, 2, Font_6x8);
    ssd1306_WriteString("Calibrate", Font_6x8, White);
    ssd1306_UpdateScreen();
    
    Wait_SW1();
    
    // ทำการ Calibrate เซ็นเซอร์ (10 วินาที)
    LineSensor_Calibrate(10000);
    
    // รอกดปุ่ม SW1 เพื่อเริ่มการควบคุม
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 1, Font_6x8);
    ssd1306_WriteString("Press SW1 to", Font_6x8, White);
    ssd1306_SetCursorLine(0, 2, Font_6x8);
    ssd1306_WriteString("Start PID", Font_6x8, White);
    ssd1306_UpdateScreen();
    
    Wait_SW1();
}

void loop()
{
    static uint32_t lastDisplay = 0;
    static uint8_t displayMode = 0;
    
    // ตรวจสอบปุ่มกด
    if(Read_SW1()) {
        if(pidController.isRunning) {
            PID_Stop();
        } else {
            PID_Start();
        }
        HAL_Delay(200); // ป้องกันการกดซ้ำ
    }
    
    if(Read_SW2()) {
        displayMode = (displayMode + 1) % 3; // เปลี่ยนโหมดแสดงผล
        HAL_Delay(200);
    }
    
    // อัพเดท PID Controller
    PID_Update();
    
    // แสดงผลทุก 100ms
    if(HAL_GetTick() - lastDisplay >= 100) {
        switch(displayMode) {
            case 0:
                // แสดงสถานะ PID
                PID_PrintStatus();
                break;
            case 1:
                // แสดงค่าเซ็นเซอร์
                LineSensor_PrintValues();
                break;
            case 2:
                // แสดงค่าแบตเตอรี่และข้อมูลเพิ่มเติม
                ssd1306_Fill(Black);
                
                char battery_str[20];
                sprintf(battery_str, "Batt: %.2fV", Battery_GetVoltage_Avg());
                ssd1306_SetCursorLine(0, 0, Font_6x8);
                ssd1306_WriteString(battery_str, Font_6x8, White);
                
                char status_str[20];
                sprintf(status_str, "Status: %s", pidController.isRunning ? "RUN" : "STOP");
                ssd1306_SetCursorLine(0, 1, Font_6x8);
                ssd1306_WriteString(status_str, Font_6x8, White);
                
                char sensor_str[20];
                sprintf(sensor_str, "Active: %d/16", LineSensor_GetActiveSensorCount());
                ssd1306_SetCursorLine(0, 2, Font_6x8);
                ssd1306_WriteString(sensor_str, Font_6x8, White);
                
                char control_str[20];
                sprintf(control_str, "SW1:Start SW2:Mode");
                ssd1306_SetCursorLine(0, 3, Font_6x8);
                ssd1306_WriteString(control_str, Font_6x8, White);
                
                ssd1306_UpdateScreen();
                break;
        }
        lastDisplay = HAL_GetTick();
    }
    
    HAL_Delay(10); // หน่วงเวลาเล็กน้อย
}

// ฟังก์ชันสำหรับการตั้งค่า Advanced PID
void AdvancedPIDSetup()
{
    uint8_t menuIndex = 0;
    bool inMenu = true;
    
    while(inMenu) {
        ssd1306_Fill(Black);
        ssd1306_SetCursorLine(0, 0, Font_6x8);
        ssd1306_WriteString("PID Setup Menu:", Font_6x8, White);
        
        // แสดงเมนู
        char menu_items[4][20] = {
            "1. Set Kp/Kd",
            "2. Set Speeds", 
            "3. Set Threshold",
            "4. Exit"
        };
        
        for(int i = 0; i < 4; i++) {
            if(i == menuIndex) {
                ssd1306_SetCursorLine(0, i + 1, Font_6x8);
                ssd1306_WriteString(">", Font_6x8, White);
                ssd1306_SetCursorLine(6, i + 1, Font_6x8);
                ssd1306_WriteString(menu_items[i], Font_6x8, White);
            } else {
                ssd1306_SetCursorLine(6, i + 1, Font_6x8);
                ssd1306_WriteString(menu_items[i], Font_6x8, White);
            }
        }
        ssd1306_UpdateScreen();
        
        // รอการกดปุ่ม
        if(Read_SW1()) {
            switch(menuIndex) {
                case 0: // Set Kp/Kd
                    SetPIDParameters();
                    break;
                case 1: // Set Speeds
                    SetSpeedParameters();
                    break;
                case 2: // Set Threshold
                    SetThresholdParameter();
                    break;
                case 3: // Exit
                    inMenu = false;
                    break;
            }
            HAL_Delay(300);
        }
        
        if(Read_SW2()) {
            menuIndex = (menuIndex + 1) % 4;
            HAL_Delay(200);
        }
    }
}

// ฟังก์ชัน Arduino-style สำหรับใช้ใน PlatformIO
void setup() {
    setup();
}

void loop() {
    loop();
}

void SetPIDParameters()
{
    float kp = pidController.kp;
    float kd = pidController.kd;
    uint8_t paramIndex = 0; // 0=Kp, 1=Kd
    
    while(true) {
        ssd1306_Fill(Black);
        ssd1306_SetCursorLine(0, 0, Font_6x8);
        ssd1306_WriteString("Set PID Parameters", Font_6x8, White);
        
        // แสดง Kp
        char kp_str[20];
        sprintf(kp_str, "%sKp: %.3f", (paramIndex == 0) ? ">" : " ", kp);
        ssd1306_SetCursorLine(0, 1, Font_6x8);
        ssd1306_WriteString(kp_str, Font_6x8, White);
        
        // แสดง Kd
        char kd_str[20];
        sprintf(kd_str, "%sKd: %.3f", (paramIndex == 1) ? ">" : " ", kd);
        ssd1306_SetCursorLine(0, 2, Font_6x8);
        ssd1306_WriteString(kd_str, Font_6x8, White);
        
        ssd1306_SetCursorLine(0, 3, Font_6x8);
        ssd1306_WriteString("SW1:+/- SW2:Select", Font_6x8, White);
        ssd1306_UpdateScreen();
        
        if(Read_SW1()) {
            if(paramIndex == 0) {
                kp += 0.05f;
                if(kp > 5.0f) kp = 0.0f;
            } else {
                kd += 0.01f;
                if(kd > 1.0f) kd = 0.0f;
            }
            HAL_Delay(200);
        }
        
        if(Read_SW2()) {
            if(paramIndex == 1) {
                // บันทึกและออก
                PID_SetParameters(kp, kd, pidController.alpha);
                
                ssd1306_Fill(Black);
                ssd1306_SetCursorLine(0, 1, Font_6x8);
                ssd1306_WriteString("Parameters Saved!", Font_6x8, White);
                ssd1306_UpdateScreen();
                HAL_Delay(1000);
                break;
            } else {
                paramIndex = 1;
                HAL_Delay(200);
            }
        }
    }
}

void SetSpeedParameters()
{
    int16_t baseSpeed = pidController.baseSpeed;
    int16_t maxSpeed = pidController.maxSpeed;
    int16_t fanSpeed = pidController.fanSpeed;
    uint8_t paramIndex = 0; // 0=Base, 1=Max, 2=Fan
    
    while(true) {
        ssd1306_Fill(Black);
        ssd1306_SetCursorLine(0, 0, Font_6x8);
        ssd1306_WriteString("Set Speed Parameters", Font_6x8, White);
        
        // แสดง Base Speed
        char base_str[20];
        sprintf(base_str, "%sBase: %d", (paramIndex == 0) ? ">" : " ", baseSpeed);
        ssd1306_SetCursorLine(0, 1, Font_6x8);
        ssd1306_WriteString(base_str, Font_6x8, White);
        
        // แสดง Max Speed
        char max_str[20];
        sprintf(max_str, "%sMax: %d", (paramIndex == 1) ? ">" : " ", maxSpeed);
        ssd1306_SetCursorLine(0, 2, Font_6x8);
        ssd1306_WriteString(max_str, Font_6x8, White);
        
        // แสดง Fan Speed
        char fan_str[20];
        sprintf(fan_str, "%sFan: %d", (paramIndex == 2) ? ">" : " ", fanSpeed);
        ssd1306_SetCursorLine(0, 3, Font_6x8);
        ssd1306_WriteString(fan_str, Font_6x8, White);
        
        ssd1306_UpdateScreen();
        
        if(Read_SW1()) {
            switch(paramIndex) {
                case 0:
                    baseSpeed += 25;
                    if(baseSpeed > 800) baseSpeed = 50;
                    break;
                case 1:
                    maxSpeed += 50;
                    if(maxSpeed > 1000) maxSpeed = 200;
                    break;
                case 2:
                    fanSpeed += 50;
                    if(fanSpeed > 1000) fanSpeed = 0;
                    break;
            }
            HAL_Delay(200);
        }
        
        if(Read_SW2()) {
            if(paramIndex == 2) {
                // บันทึกและออก
                PID_SetSpeeds(baseSpeed, maxSpeed, fanSpeed);
                
                ssd1306_Fill(Black);
                ssd1306_SetCursorLine(0, 1, Font_6x8);
                ssd1306_WriteString("Speeds Saved!", Font_6x8, White);
                ssd1306_UpdateScreen();
                HAL_Delay(1000);
                break;
            } else {
                paramIndex = (paramIndex + 1) % 3;
                HAL_Delay(200);
            }
        }
    }
}

void SetThresholdParameter()
{
    uint16_t threshold = lineSensor.threshold;
    
    while(true) {
        // อ่านค่าเซ็นเซอร์ปัจจุบัน
        LineSensor_ReadRaw();
        
        ssd1306_Fill(Black);
        ssd1306_SetCursorLine(0, 0, Font_6x8);
        ssd1306_WriteString("Set Threshold", Font_6x8, White);
        
        char thresh_str[20];
        sprintf(thresh_str, "Threshold: %d", threshold);
        ssd1306_SetCursorLine(0, 1, Font_6x8);
        ssd1306_WriteString(thresh_str, Font_6x8, White);
        
        // แสดงจำนวนเซ็นเซอร์ที่ active ด้วย threshold ปัจจุบัน
        uint8_t activeCount = 0;
        for(int i = 0; i < NUM_SENSORS; i++) {
            if(lineSensor.rawValues[i] >= threshold) {
                activeCount++;
            }
        }
        
        char active_str[20];
        sprintf(active_str, "Active: %d/16", activeCount);
        ssd1306_SetCursorLine(0, 2, Font_6x8);
        ssd1306_WriteString(active_str, Font_6x8, White);
        
        ssd1306_SetCursorLine(0, 3, Font_6x8);
        ssd1306_WriteString("SW1:+100 SW2:Save", Font_6x8, White);
        ssd1306_UpdateScreen();
        
        if(Read_SW1()) {
            threshold += 100;
            if(threshold > 4000) threshold = 500;
            HAL_Delay(200);
        }
        
        if(Read_SW2()) {
            // บันทึกและออก
            LineSensor_SetThreshold(threshold);
            
            ssd1306_Fill(Black);
            ssd1306_SetCursorLine(0, 1, Font_6x8);
            ssd1306_WriteString("Threshold Saved!", Font_6x8, White);
            ssd1306_UpdateScreen();
            HAL_Delay(1000);
            break;
        }
        
        HAL_Delay(50); // อัพเดทหน้าจอเร็วขึ้น
    }