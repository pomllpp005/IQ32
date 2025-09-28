#ifndef __IQ32_BOARD_H
#define __IQ32_BOARD_H

#include "stm32f4xx_hal.h"
#include "iq32_constants.h"

#include "iq32_adc.h"
#include "iq32_motor.h"
#include "iq32_fan.h"
#include "iq32_led.h"
#include "iq32_battery.h"
#include "iq32_mpu6500.h"
#include "iq32_Mux.h"
#include "iq32_linesensor.h"
#include "iq32_pid_improved.h"
#include "iq32_pid_advanced.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

// Global handles
extern TIM_HandleTypeDef htim2;
extern I2C_HandleTypeDef hi2c1;
extern ADC_HandleTypeDef hadc1;
extern SPI_HandleTypeDef hspi1;

// === SYSTEM INFORMATION ===
#define IQ32_BOARD_VERSION      "2.0"
#define IQ32_FIRMWARE_VERSION   "1.0.0"
#define IQ32_BUILD_DATE         __DATE__
#define IQ32_BUILD_TIME         __TIME__

// === SYSTEM STATUS ===
typedef struct {
    bool system_initialized;
    bool error_system_ready;
    bool adc_ready;
    bool sensors_calibrated;
    bool pid_running;
    uint32_t system_uptime;
    uint32_t last_heartbeat;
} IQ32_SystemStatus_t;

extern IQ32_SystemStatus_t g_system_status;

// === RESULT TYPE (แทน IQ32_Error_t เดิม) ===
typedef enum {
    IQ32_OK = 0,
    IQ32_ERROR = -1,
    IQ32_ERROR_NOT_INITIALIZED = -2,
    IQ32_ERROR_NOT_READY = -3,
    IQ32_ERROR_HARDWARE = -4,
    IQ32_ERROR_NOT_FOUND = -5
} IQ32_Result_t;

// === CORE FUNCTIONS ===
IQ32_Result_t IQ32_Init(void);
IQ32_Result_t IQ32_DeInit(void);
IQ32_Result_t MX_GPIO_Init(void);
IQ32_Result_t MX_TIM2_Init(void);
IQ32_Result_t MX_I2C1_Init(void);
IQ32_Result_t MX_SPI1_Init(void);

// === SYSTEM MANAGEMENT ===
IQ32_Result_t IQ32_SystemCheck(void);
void IQ32_SystemUpdate(void);
void IQ32_Heartbeat(void);
uint32_t IQ32_GetUptime(void);
const char* IQ32_GetVersion(void);
const char* IQ32_GetBuildInfo(void);

// === SWITCH FUNCTIONS ===
void Wait_SW1(void);
void Wait_SW2(void);
uint8_t Read_SW1(void);
uint8_t Read_SW2(void);
IQ32_Result_t IQ32_WaitForSwitch(uint8_t switch_num, uint32_t timeout_ms);

// === EMERGENCY FUNCTIONS ===
IQ32_Result_t IQ32_EmergencyStop(void);
IQ32_Result_t IQ32_EmergencyRecovery(void);
bool IQ32_IsEmergencyState(void);

// === DIAGNOSTIC FUNCTIONS ===
IQ32_Result_t IQ32_SelfTest(void);
IQ32_Result_t IQ32_RunDiagnostics(void);
void IQ32_DisplaySystemInfo(void);
void IQ32_DisplayErrorStatus(void);

// === CALIBRATION FUNCTIONS ===
IQ32_Result_t IQ32_CalibrateAll(void);
IQ32_Result_t IQ32_CalibrateSensors(uint32_t duration_ms);
IQ32_Result_t IQ32_CalibrateIMU(void);
IQ32_Result_t IQ32_SaveCalibration(void);
IQ32_Result_t IQ32_LoadCalibration(void);

// === UTILITY MACROS ===
#define IQ32_SYSTEM_CHECK() \
    do { \
        if(!g_system_status.system_initialized) { \
            return IQ32_ERROR_NOT_INITIALIZED; \
        } \
    } while(0)

#define IQ32_UPDATE_HEARTBEAT() \
    do { \
        g_system_status.last_heartbeat = HAL_GetTick(); \
    } while(0)

#define IQ32_CHECK_SUBSYSTEM(condition, error_code) \
    do { \
        if(!(condition)) { \
            return error_code; \
        } \
    } while(0)

// === CONFIGURATION CONSTANTS ===
#define IQ32_HEARTBEAT_INTERVAL     1000    // ms
#define IQ32_WATCHDOG_TIMEOUT       5000    // ms  
#define IQ32_SELFTEST_TIMEOUT       10000   // ms
#define IQ32_EMERGENCY_TIMEOUT      2000    // ms
#define IQ32_DEFAULT_SWITCH_TIMEOUT 30000   // ms

// === SYSTEM STATES ===
typedef enum {
    IQ32_STATE_UNINITIALIZED = 0,
    IQ32_STATE_INITIALIZING,
    IQ32_STATE_READY,
    IQ32_STATE_RUNNING,
    IQ32_STATE_CALIBRATING,
    IQ32_STATE_ERROR,
    IQ32_STATE_EMERGENCY
} IQ32_SystemState_t;

IQ32_SystemState_t IQ32_GetSystemState(void);
const char* IQ32_SystemStateToString(IQ32_SystemState_t state);

#endif // __IQ32_BOARD_H
