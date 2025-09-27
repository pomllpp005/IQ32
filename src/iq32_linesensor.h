#ifndef __IQ32_LINESENSOR_H
#define __IQ32_LINESENSOR_H

#include "stm32f4xx_hal.h"
#include "iq32_Mux.h"
#include <stdbool.h>
#include <stdint.h>

// --- กำหนดค่าคงที่ ---
#define NUM_SENSORS 16
#define DEFAULT_THRESHOLD 2000

// --- โครงสร้างข้อมูล Line Sensor ---
typedef struct {
    uint16_t rawValues[NUM_SENSORS];
    uint16_t calibratedMin[NUM_SENSORS];
    uint16_t calibratedMax[NUM_SENSORS];
    uint16_t threshold;
    bool sensorActive[NUM_SENSORS];
    uint16_t position;
    bool isOnLine;
    bool isCalibrated;
    uint8_t activeSensorCount;
} LineSensor_t;

// --- ฟังก์ชัน Line Sensor ---
void LineSensor_Init(void);
void LineSensor_SetThreshold(uint16_t threshold);
void LineSensor_ReadRaw(void);
void LineSensor_Calibrate(uint32_t calibrationTime);
uint16_t LineSensor_ReadPosition(void);
bool LineSensor_IsOnLine(void);
uint8_t LineSensor_GetActiveSensorCount(void);
void LineSensor_PrintValues(void);

// --- ตัวแปร Global ---
extern LineSensor_t lineSensor;

#endif