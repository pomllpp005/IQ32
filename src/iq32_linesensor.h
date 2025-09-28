// ========== iq32_linesensor.h ==========
#ifndef __IQ32_LINESENSOR_H
#define __IQ32_LINESENSOR_H

#include "stm32f4xx_hal.h"
#include "iq32_constants.h"
#include "iq32_Mux.h"

typedef struct {
    uint16_t rawValues[MAX_SENSORS];
    uint16_t calibratedMin[MAX_SENSORS];
    uint16_t calibratedMax[MAX_SENSORS];
    uint16_t threshold;
    bool sensorActive[MAX_SENSORS];
    uint16_t position;
    bool isOnLine;
    bool isCalibrated;
    uint8_t activeSensorCount;
} LineSensor_t;

extern LineSensor_t lineSensor;

IQ32_Result_t LineSensor_Init(void);
IQ32_Result_t LineSensor_SetThreshold(uint16_t threshold);
IQ32_Result_t LineSensor_ReadRaw(void);
IQ32_Result_t LineSensor_Calibrate(uint32_t calibrationTime);
uint16_t LineSensor_ReadPosition(void);
bool LineSensor_IsOnLine(void);
uint8_t LineSensor_GetActiveSensorCount(void);
IQ32_Result_t LineSensor_PrintValues(void);

#endif
// ===== Implementation =====