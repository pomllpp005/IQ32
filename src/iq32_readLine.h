#ifndef __IQ32_READLINE_H
#define __IQ32_READLINE_H
#define  MAX_SENSORS 16
uint16_t sensorValues[MAX_SENSORS]; 

#include "iq32_board.h" // include board ก่อน
#include "iq32_Mux.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t rawValues[MAX_SENSORS];
    uint16_t calibratedMin[MAX_SENSORS];
    uint16_t calibratedMax[MAX_SENSORS];
    bool isCalibrated;
    bool isOnLine;
    uint16_t position;
    uint16_t threshold;
} LineSensor_t;

extern LineSensor_t lineSensor;

IQ32_Result_t LineSensor_ReadRaw(void);
IQ32_Result_t LineSensor_Calibrate(uint32_t calibrationTime);
uint16_t LineSensor_ReadPosition(void);
void LineSensor_ReadCalibrated(void);
bool LineSensor_IsOnLine(void);

#endif
// ===== Implementation =====