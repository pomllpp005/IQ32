#include "iq32_linesensor.h"
#include "iq32_led.h"

// --- ตัวแปร Global ---
LineSensor_t lineSensor;

// --- เริ่มต้น Line Sensor ---
void LineSensor_Init(void)
{
    // เริ่มต้น MUX (ถ้ายังไม่ได้เริ่มต้น)
    MUX_Init();
    
    // ตั้งค่าเริ่มต้น
    lineSensor.threshold = DEFAULT_THRESHOLD;
    lineSensor.isCalibrated = false;
    lineSensor.isOnLine = false;
    lineSensor.position = 0;
    lineSensor.activeSensorCount = 0;
    
    // กำหนดค่าเริ่มต้นสำหรับ calibration
    for(int i = 0; i < NUM_SENSORS; i++) {
        lineSensor.calibratedMin[i] = 4095;  // ค่าสูงสุดของ ADC 12-bit
        lineSensor.calibratedMax[i] = 0;     // ค่าต่ำสุด
        lineSensor.sensorActive[i] = false;
        lineSensor.rawValues[i] = 0;
    }
}

// --- ตั้งค่า Threshold ---
void LineSensor_SetThreshold(uint16_t threshold)
{
    lineSensor.threshold = threshold;
}

// --- อ่านค่าดิบจาก MUX ---
void LineSensor_ReadRaw(void)
{
    for(int i = 0; i < NUM_SENSORS; i++) {
        MUX_SelectChannel(i);
        lineSensor.rawValues[i] = MUX_Read();
    }
}

// --- สอบเทียบเซ็นเซอร์ ---
void LineSensor_Calibrate(uint32_t calibrationTime)
{
    uint32_t startTime = HAL_GetTick();
    
    // กะพริบ LED เพื่อแสดงสถานะการ calibrate
    LED_On(LED1);
    LED_On(LED2);
    
    while((HAL_GetTick() - startTime) < calibrationTime) {
        LineSensor_ReadRaw();
        
        // อัพเดทค่า min และ max
        for(int i = 0; i < NUM_SENSORS; i++) {
            if(lineSensor.rawValues[i] < lineSensor.calibratedMin[i]) {
                lineSensor.calibratedMin[i] = lineSensor.rawValues[i];
            }
            if(lineSensor.rawValues[i] > lineSensor.calibratedMax[i]) {
                lineSensor.calibratedMax[i] = lineSensor.rawValues[i];
            }
        }
        
        // กะพริบ LED
        if((HAL_GetTick() - startTime) % 500 < 250) {
            LED_On(LED1);
            LED_Off(LED2);
        } else {
            LED_Off(LED1);
            LED_On(LED2);
        }
        
        HAL_Delay(10);
    }
    
    // คำนวณ threshold สำหรับแต่ละเซ็นเซอร์
    lineSensor.isCalibrated = true;
    
    // ปิด LED
    LED_Off(LED1);
    LED_Off(LED2);
    
    HAL_Delay(500); // หน่วงเล็กน้อยเพื่อแสดงว่าเสร็จสิ้น
}

// --- อ่านตำแหน่งของเส้น ---
uint16_t LineSensor_ReadPosition(void)
{
    LineSensor_ReadRaw();
    
    uint32_t sum = 0;
    uint32_t weightedSum = 0;
    lineSensor.activeSensorCount = 0;
    
    // คำนวณ weighted average
    for(int i = 0; i < NUM_SENSORS; i++) {
        uint16_t calibratedValue = lineSensor.rawValues[i];
        
        // ใช้ calibrated values ถ้ามีการ calibrate แล้ว
        if(lineSensor.isCalibrated) {
            uint16_t range = lineSensor.calibratedMax[i] - lineSensor.calibratedMin[i];
            if(range > 0) {
                calibratedValue = ((lineSensor.rawValues[i] - lineSensor.calibratedMin[i]) * 4095) / range;
            }
        }
        
        // ตรวจสอบว่าเซ็นเซอร์นี้เห็นเส้นหรือไม่
        if(calibratedValue >= lineSensor.threshold) {
            lineSensor.sensorActive[i] = true;
            lineSensor.activeSensorCount++;
            
            // คำนวณ position (0 = ซ้ายสุด, (NUM_SENSORS-1)*1000 = ขวาสุด)
            sum += calibratedValue;
            weightedSum += calibratedValue * (i * 1000);
        } else {
            lineSensor.sensorActive[i] = false;
        }
    }
    
    // คำนวณตำแหน่ง
    if(sum > 0) {
        lineSensor.position = weightedSum / sum;
        lineSensor.isOnLine = true;
    } else {
        lineSensor.isOnLine = false;
        // รักษาตำแหน่งเดิมถ้าไม่เจอเส้น
    }
    
    return lineSensor.position;
}

// --- ตรวจสอบว่ามีเส้นหรือไม่ ---
bool LineSensor_IsOnLine(void)
{
    return lineSensor.isOnLine;
}

// --- นับจำนวนเซ็นเซอร์ที่ active ---
uint8_t LineSensor_GetActiveSensorCount(void)
{
    return lineSensor.activeSensorCount;
}

// --- แสดงค่าเซ็นเซอร์ (ไม่ใช้ OLED) ---
void LineSensor_PrintValues(void)
{
    // ฟังก์ชันนี้ไม่ทำอะไร หรือสามารถส่งข้อมูลผ่าน UART ได้
    // หรือใช้ LED เพื่อแสดงสถานะแทน
    if(lineSensor.isOnLine) {
        LED_On(LED2);
    } else {
        LED_Off(LED2);
    }
}