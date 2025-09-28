// ========== iq32_readLine.cpp ==========

#include "iq32_readLine.h"



LineSensor_t lineSensor;

IQ32_Result_t LineSensor_ReadRaw(void)
{
    for(int i = 0; i < MAX_SENSORS; i++) {
        IQ32_Result_t result = MUX_SelectChannel(i);
        if(result != IQ32_OK) return result;
        
        lineSensor.rawValues[i] = MUX_Read();
    }
    return IQ32_OK;
}
// ===== Implementation =====
IQ32_Result_t LineSensor_Calibrate(uint32_t calibrationTime)
{
    if(calibrationTime == 0) calibrationTime = DEFAULT_CALIB_TIME;

    uint32_t startTime = HAL_GetTick();
    // LED_On(LED_ALL);
    
    // ค่าเริ่มต้น
    for(int i = 0; i < MAX_SENSORS; i++) {
        lineSensor.calibratedMin[i] = ADC_RESOLUTION;
        lineSensor.calibratedMax[i] = 0;
    }

    while((HAL_GetTick() - startTime) < calibrationTime) {
        LineSensor_ReadRaw();
        
        for(int i = 0; i < MAX_SENSORS; i++) {
            if(lineSensor.rawValues[i] < lineSensor.calibratedMin[i]) {
                lineSensor.calibratedMin[i] = lineSensor.rawValues[i];
            }
            if(lineSensor.rawValues[i] > lineSensor.calibratedMax[i]) {
                lineSensor.calibratedMax[i] = lineSensor.rawValues[i];
            }
        }
        HAL_Delay(10);
    }
    
    lineSensor.isCalibrated = true;
    // LED_Off(LED_ALL);
    HAL_Delay(100);
    
    return IQ32_OK;
}
// ===== อ่านค่า sensor แบบ calibrate =====
void LineSensor_ReadCalibrated(void)
{
    LineSensor_ReadRaw();  // อ่าน raw

    for (int i = 0; i < MAX_SENSORS; i++) {
        int value = 1000 - map(lineSensor.rawValues[i], 
                                lineSensor.calibratedMin[i], 
                                lineSensor.calibratedMax[i],
                                0, 1000);

        value = CONSTRAIN(value, 0, 1000);
        lineSensor.sensorValues[i] = value;  // เก็บค่า calibrated
    }
}


// ===== Implementation =====
uint16_t LineSensor_ReadPosition(void)
{
    LineSensor_ReadCalibrated();  // อ่านค่า calibrate

    uint32_t sum = 0;
    uint32_t weightedSum = 0;

    for (int i = 0; i < MAX_SENSORS; i++) {
        if (lineSensor.sensorValues[i] >= lineSensor.threshold) {
            sum += sensorValues[i];
            weightedSum += (uint32_t)sensorValues[i] * (i * 1000);
        }
    }

    if (sum > 0) {
        lineSensor.position = weightedSum / sum;
        lineSensor.isOnLine = true;
    } else {
        lineSensor.position = 0;
        lineSensor.isOnLine = false;
    }

    return lineSensor.position;
}
bool LineSensor_IsOnLine(void)
{
    return lineSensor.isOnLine;
}
// ===== Implementation =====
