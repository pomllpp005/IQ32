
// ========== iq32_linesensor.cpp ==========
#include "iq32_linesensor.h"
#include "iq32_led.h"

LineSensor_t lineSensor;

IQ32_Result_t LineSensor_Init(void)
{
    // Initialize MUX if not already done
    IQ32_Result_t result = MUX_Init();
    if(result != IQ32_OK) return result;
    
    // Initialize sensor structure
    lineSensor.threshold = DEFAULT_THRESHOLD;
    lineSensor.isCalibrated = false;
    lineSensor.isOnLine = false;
    lineSensor.position = SENSOR_CENTER;
    lineSensor.activeSensorCount = 0;
    
    // Initialize calibration arrays
    for(int i = 0; i < MAX_SENSORS; i++) {
        lineSensor.calibratedMin[i] = ADC_RESOLUTION;
        lineSensor.calibratedMax[i] = 0;
        lineSensor.sensorActive[i] = false;
        lineSensor.rawValues[i] = 0;
    }
    
    return IQ32_OK;
}

IQ32_Result_t LineSensor_SetThreshold(uint16_t threshold)
{
    if(!VALIDATE_RANGE(threshold, 0, ADC_RESOLUTION)) {
        return IQ32_OUT_OF_RANGE;
    }
    
    lineSensor.threshold = threshold;
    return IQ32_OK;
}

IQ32_Result_t LineSensor_ReadRaw(void)
{
    for(int i = 0; i < MAX_SENSORS; i++) {
        IQ32_Result_t result = MUX_SelectChannel(i);
        if(result != IQ32_OK) return result;
        
        lineSensor.rawValues[i] = MUX_Read();
    }
    return IQ32_OK;
}

IQ32_Result_t LineSensor_Calibrate(uint32_t calibrationTime)
{
    if(calibrationTime == 0) calibrationTime = DEFAULT_CALIB_TIME;

    uint32_t startTime = HAL_GetTick();
    LED_On(LED_ALL);
    
    while((HAL_GetTick() - startTime) < calibrationTime) {
        LineSensor_ReadRaw();
        
        // Update min and max values
        for(int i = 0; i < MAX_SENSORS; i++) {
            if(lineSensor.rawValues[i] < lineSensor.calibratedMin[i]) {
                lineSensor.calibratedMin[i] = lineSensor.rawValues[i];
            }
            if(lineSensor.rawValues[i] > lineSensor.calibratedMax[i]) {
                lineSensor.calibratedMax[i] = lineSensor.rawValues[i];
            }
        }
        
        // Visual indication
        if((HAL_GetTick() - startTime) % BLINK_INTERVAL < (BLINK_INTERVAL / 2)) {
            LED_On(LED1);
            LED_Off(LED2);
        } else {
            LED_Off(LED1);
            LED_On(LED2);
        }
        
        HAL_Delay(10);
    }
    
    lineSensor.isCalibrated = true;
    LED_Off(LED_ALL);
    HAL_Delay(500);
    
    return IQ32_OK;
}

uint16_t LineSensor_ReadPosition(void)
{
    LineSensor_ReadRaw();
    
    uint32_t sum = 0;
    uint32_t weightedSum = 0;
    lineSensor.activeSensorCount = 0;
    
    // Calculate weighted average
    for(int i = 0; i < MAX_SENSORS; i++) {
        uint16_t calibratedValue = lineSensor.rawValues[i];
        
        // Apply calibration if available
        if(lineSensor.isCalibrated) {
            uint16_t range = lineSensor.calibratedMax[i] - lineSensor.calibratedMin[i];
            if(range > 0) {
                calibratedValue = ((lineSensor.rawValues[i] - lineSensor.calibratedMin[i]) * ADC_RESOLUTION) / range;
            }
        }
        
        // Check if sensor sees line
        if(calibratedValue >= lineSensor.threshold) {
            lineSensor.sensorActive[i] = true;
            lineSensor.activeSensorCount++;
            
            sum += calibratedValue;
            weightedSum += calibratedValue * (i * 1000);
        } else {
            lineSensor.sensorActive[i] = false;
        }
    }
    
    // Calculate position
    if(sum > 0) {
        lineSensor.position = weightedSum / sum;
        lineSensor.isOnLine = true;
    } else {
        lineSensor.isOnLine = false;
        // Keep previous position when no line detected
    }
    
    return lineSensor.position;
}

bool LineSensor_IsOnLine(void)
{
    return lineSensor.isOnLine;
}

uint8_t LineSensor_GetActiveSensorCount(void)
{
    return lineSensor.activeSensorCount;
}

IQ32_Result_t LineSensor_PrintValues(void)
{
    // Use LED to indicate line detection status
    if(lineSensor.isOnLine) {
        LED_On(LED2);
    } else {
        LED_Off(LED2);
    }
    
    return IQ32_OK;
}