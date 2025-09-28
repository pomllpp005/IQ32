#include "iq32_board.h"

<<<<<<< HEAD
TIM_HandleTypeDef htim2;
I2C_HandleTypeDef hi2c1;
IQ32_SystemStatus_t g_system_status;

// Private variables
static IQ32_SystemState_t current_system_state = IQ32_STATE_UNINITIALIZED;

// === SYSTEM STATE MANAGEMENT ===
IQ32_SystemState_t IQ32_GetSystemState(void)
{
    return current_system_state;
}

IQ32_Result_t IQ32_SetSystemState(IQ32_SystemState_t state)
{
    current_system_state = state;
    return IQ32_OK;
}

const char* IQ32_SystemStateToString(IQ32_SystemState_t state)
{
    switch(state) {
        case IQ32_STATE_UNINITIALIZED: return "UNINITIALIZED";
        case IQ32_STATE_INITIALIZING:  return "INITIALIZING";
        case IQ32_STATE_READY:         return "READY";
        case IQ32_STATE_RUNNING:       return "RUNNING";
        case IQ32_STATE_CALIBRATING:   return "CALIBRATING";
        case IQ32_STATE_ERROR:         return "ERROR";
        case IQ32_STATE_EMERGENCY:     return "EMERGENCY";
        default:                       return "UNKNOWN";
    }
}

// === TIMER INITIALIZATION ===
IQ32_Result_t MX_TIM2_Init(void)
=======

GPIO_InitTypeDef GPIO_InitStruct;
TIM_HandleTypeDef htim2;
I2C_HandleTypeDef hi2c1;

// --- ตั้งค่า Timer2 สำหรับ PWM บน PA0 (CH1), PA1 (CH2), PA2 (CH3) ---
void MX_TIM2_Init(void)
>>>>>>> 39340a1 (v1)
{
    __HAL_RCC_TIM2_CLK_ENABLE();

    htim2.Instance = TIM2;
<<<<<<< HEAD
    htim2.Init.Prescaler = TIMER_PRESCALER;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = PWM_PERIOD;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    if(HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
        return IQ32_ERROR;
    }
=======
    htim2.Init.Prescaler = 84 - 1;    // APB1=84MHz → 1MHz timer tick
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000 - 1;     // PWM 1 kHz
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim2);
>>>>>>> 39340a1 (v1)

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

<<<<<<< HEAD
    // Configure all PWM channels
    for(uint32_t channel = TIM_CHANNEL_1; channel <= TIM_CHANNEL_3; channel <<= 1) {
        if(HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, channel) != HAL_OK) {
            return IQ32_ERROR;
        }
        if(HAL_TIM_PWM_Start(&htim2, channel) != HAL_OK) {
            return IQ32_ERROR;
        }
    }

    return IQ32_OK;
}

// === I2C INITIALIZATION ===
IQ32_Result_t MX_I2C1_Init(void)
{
    __HAL_RCC_I2C1_CLK_ENABLE();
    
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = I2C_CLOCK_SPEED;
=======
    // CH1 = PA0
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    // CH2 = PA1
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
    // CH3 = PA2 (Fan)
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
}


void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000; // หรือ 100000 ถ้าใช้ I2C slow mode
>>>>>>> 39340a1 (v1)
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
<<<<<<< HEAD
    
    if(HAL_I2C_Init(&hi2c1) != HAL_OK) {
        return IQ32_ERROR;
    }
    return IQ32_OK;
}

// === SPI INITIALIZATION ===
IQ32_Result_t MX_SPI1_Init(void)
{
    // SPI initialization is handled in MPU6500_Init()
    // This function is here for completeness
    return IQ32_OK;
}

// === GPIO INITIALIZATION ===
IQ32_Result_t MX_GPIO_Init(void)
{
    // Enable all required clocks
=======
    HAL_I2C_Init(&hi2c1);
}
// --- ตั้งค่า GPIO ---
void MX_GPIO_Init(void)
{
>>>>>>> 39340a1 (v1)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

<<<<<<< HEAD
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Motor direction pins (PA8, PA9, PA10, PA11)
    GPIO_InitStruct.Pin = MOTOR1_INA_PIN | MOTOR1_INB_PIN | MOTOR2_INA_PIN | MOTOR2_INB_PIN;
=======
    // INA/INB มอเตอร์1 (ซ้าย) = PA8, PA9
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
>>>>>>> 39340a1 (v1)
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

<<<<<<< HEAD
    // PWM pins (PA0, PA1, PA2)
    GPIO_InitStruct.Pin = PWM_MOTOR1_PIN | PWM_MOTOR2_PIN | PWM_FAN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
=======
    // INA/INB มอเตอร์2 (ขวา) = PA10, PA11
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PWM = PA0 (CH1), PA1 (CH2), PA2 (CH3 - Fan)
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
>>>>>>> 39340a1 (v1)
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

<<<<<<< HEAD
    // LED pins
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = LED1_PIN;
    HAL_GPIO_Init(LED1_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = LED2_PIN;
    HAL_GPIO_Init(LED2_PORT, &GPIO_InitStruct);

    // Switch pins with pull-up
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Pin = SW1_PIN;
    HAL_GPIO_Init(SW1_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = SW2_PIN;
    HAL_GPIO_Init(SW2_PORT, &GPIO_InitStruct);

    // I2C pins
    GPIO_InitStruct.Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    return IQ32_OK;
}

// === MAIN INITIALIZATION ===
IQ32_Result_t IQ32_Init(void)
{
    IQ32_SetSystemState(IQ32_STATE_INITIALIZING);
    
    // Initialize HAL
    if(HAL_Init() != HAL_OK) {
        return IQ32_ERROR;
    }

    // Initialize peripherals
    if(MX_GPIO_Init() != IQ32_OK ||
       MX_TIM2_Init() != IQ32_OK ||
       MX_I2C1_Init() != IQ32_OK) {
        return IQ32_ERROR;
    }

    // Initialize ADC system
    if(IQ32_ADC_Init() != IQ32_OK) {
        return IQ32_ERROR;
    }
    g_system_status.adc_ready = true;

    // Initialize subsystems
    if(MUX_Init() != IQ32_OK ||
       Battery_Init() != IQ32_OK ||
       MPU6500_Init() != IQ32_OK ||
       LED_Init() != IQ32_OK) {
        return IQ32_ERROR;
    }
    
    // Initialize display
    ssd1306_Init();

    // Initialize system status
    g_system_status.system_initialized = true;
    g_system_status.sensors_calibrated = false;
    g_system_status.pid_running = false;
    g_system_status.system_uptime = 0;
    g_system_status.last_heartbeat = HAL_GetTick();

    // Startup LED sequence
    if(LED_Blink(LED_ALL, STARTUP_BLINK_COUNT, STARTUP_BLINK_DELAY) != IQ32_OK) {
        return IQ32_ERROR;
    }

    IQ32_SetSystemState(IQ32_STATE_READY);
    return IQ32_OK;
}

// === SYSTEM DEINITIALIZATION ===
IQ32_Result_t IQ32_DeInit(void)
{
    IQ32_SetSystemState(IQ32_STATE_UNINITIALIZED);
    
    // Stop all operations
    PID_Stop();
    Motor_Stop();
    Fan_SetSpeed(0);
    LED_Off(LED_ALL);
    
    // Deinitialize subsystems
    IQ32_ADC_DeInit();
    
    g_system_status.system_initialized = false;
    return IQ32_OK;
}

// === SYSTEM MANAGEMENT ===
IQ32_Result_t IQ32_SystemCheck(void)
{
    IQ32_SYSTEM_CHECK();
    
    // Check critical subsystems
    IQ32_CHECK_SUBSYSTEM(g_system_status.adc_ready, IQ32_ERROR);
    
    return IQ32_OK;
}

void IQ32_SystemUpdate(void)
{
    uint32_t current_time = HAL_GetTick();
    
    // Update system uptime
    g_system_status.system_uptime = current_time;
    
    // Check heartbeat
    if((current_time - g_system_status.last_heartbeat) > IQ32_HEARTBEAT_INTERVAL) {
        IQ32_Heartbeat();
    }
    
    // Update PID status
    g_system_status.pid_running = pidController.isRunning;
}

void IQ32_Heartbeat(void)
{
    IQ32_UPDATE_HEARTBEAT();
    
    // Brief LED flash to show system is alive
    if(IQ32_GetSystemState() == IQ32_STATE_READY || 
       IQ32_GetSystemState() == IQ32_STATE_RUNNING) {
        LED_On(LED1);
        HAL_Delay(10);
        LED_Off(LED1);
    }
}

uint32_t IQ32_GetUptime(void)
{
    return g_system_status.system_uptime;
}

const char* IQ32_GetVersion(void)
{
    return IQ32_FIRMWARE_VERSION;
}

const char* IQ32_GetBuildInfo(void)
{
    static char build_info[64];
    snprintf(build_info, sizeof(build_info), "Built: %s %s", IQ32_BUILD_DATE, IQ32_BUILD_TIME);
    return build_info;
}// Initialize HAL
    IQ32_CHECK_HAL_STATUS(HAL_Init());

    // Initialize peripherals
    IQ32_RETURN_IF_ERROR(MX_GPIO_Init());
    IQ32_RETURN_IF_ERROR(MX_TIM2_Init());
    IQ32_RETURN_IF_ERROR(MX_I2C1_Init());

    // Initialize ADC system
    IQ32_RETURN_IF_ERROR(IQ32_ADC_Init());
    g_system_status.adc_ready = true;

    // Initialize subsystems
    IQ32_RETURN_IF_ERROR(MUX_Init());
    IQ32_RETURN_IF_ERROR(Battery_Init());
    IQ32_RETURN_IF_ERROR(MPU6500_Init());
    IQ32_RETURN_IF_ERROR(LED_Init());
    
    // Initialize display
    ssd1306_Init();

    // Initialize system status
    g_system_status.system_initialized = true;
    g_system_status.sensors_calibrated = false;
    g_system_status.pid_running = false;
    g_system_status.system_uptime = 0;
    g_system_status.last_heartbeat = HAL_GetTick();

    // Startup LED sequence
    IQ32_RETURN_IF_ERROR(LED_Blink(LED_ALL, STARTUP_BLINK_COUNT, STARTUP_BLINK_DELAY));

    IQ32_SetSystemState(IQ32_STATE_READY);
    return IQ32_OK;
}

// === SYSTEM DEINITIALIZATION ===
IQ32_Error_t IQ32_DeInit(void)
{
    IQ32_SetSystemState(IQ32_STATE_UNINITIALIZED);
    
    // Stop all operations
    PID_Stop();
    Motor_Stop();
    Fan_SetSpeed(0);
    LED_Off(LED_ALL);
    
    // Deinitialize subsystems
    IQ32_ADC_DeInit();
    
    g_system_status.system_initialized = false;
    return IQ32_OK;
}

// === SYSTEM MANAGEMENT ===
IQ32_Error_t IQ32_SystemCheck(void)
{
    IQ32_SYSTEM_CHECK();
    
    // Check critical subsystems
    IQ32_CHECK_SUBSYSTEM(g_system_status.error_system_ready, IQ32_ERROR_NOT_READY);
    IQ32_CHECK_SUBSYSTEM(g_system_status.adc_ready, IQ32_ERROR_HARDWARE);
    
    // Check for critical errors
    if(IQ32_Error_HasCriticalErrors()) {
        IQ32_SetSystemState(IQ32_STATE_ERROR);
        return IQ32_ERROR_HARDWARE;
    }
    
    return IQ32_OK;
}

void IQ32_SystemUpdate(void)
{
    uint32_t current_time = HAL_GetTick();
    
    // Update system uptime
    g_system_status.system_uptime = current_time;
    
    // Update error system
    IQ32_Error_Update();
    
    // Check heartbeat
    if((current_time - g_system_status.last_heartbeat) > IQ32_HEARTBEAT_INTERVAL) {
        IQ32_Heartbeat();
    }
    
    // Update PID status
    g_system_status.pid_running = pidController.isRunning;
}

void IQ32_Heartbeat(void)
{
    IQ32_UPDATE_HEARTBEAT();
    
    // Brief LED flash to show system is alive
    if(IQ32_GetSystemState() == IQ32_STATE_READY || 
       IQ32_GetSystemState() == IQ32_STATE_RUNNING) {
        LED_On(LED1);
        HAL_Delay(10);
        LED_Off(LED1);
    }
}

uint32_t IQ32_GetUptime(void)
{
    return g_system_status.system_uptime;
}

const char* IQ32_GetVersion(void)
{
    return IQ32_FIRMWARE_VERSION;
}

const char* IQ32_GetBuildInfo(void)
{
    static char build_info[64];
    snprintf(build_info, sizeof(build_info), "Built: %s %s", IQ32_BUILD_DATE, IQ32_BUILD_TIME);
    return build_info;
}

// === SWITCH FUNCTIONS ===
void Wait_SW1(void)
{
    while(HAL_GPIO_ReadPin(SW1_PORT, SW1_PIN) == GPIO_PIN_SET) {}
    HAL_Delay(DEBOUNCE_DELAY);
    LED_Blink(LED2, 2, 100);
}

void Wait_SW2(void)
{
    while(HAL_GPIO_ReadPin(SW2_PORT, SW2_PIN) == GPIO_PIN_SET) {}
    HAL_Delay(DEBOUNCE_DELAY);
    LED_Blink(LED1, 2, 100);
}

uint8_t Read_SW1(void)
{
    if(HAL_GPIO_ReadPin(SW1_PORT, SW1_PIN) == GPIO_PIN_RESET) {
        LED_On(LED2);
        HAL_Delay(100);
        LED_Off(LED2);
        return 1;
    }
    return 0;
}

uint8_t Read_SW2(void)
{
    if(HAL_GPIO_ReadPin(SW2_PORT, SW2_PIN) == GPIO_PIN_RESET) {
        LED_On(LED1);
        HAL_Delay(100);
        LED_Off(LED1);
        return 1;
    }
    return 0;
}

IQ32_Result_t IQ32_WaitForSwitch(uint8_t switch_num, uint32_t timeout_ms)
{
    if(!VALIDATE_RANGE(switch_num, 1, 2)) {
        return IQ32_INVALID_PARAM;
    }
    
    uint32_t start_time = HAL_GetTick();
    GPIO_TypeDef* port = (switch_num == 1) ? SW1_PORT : SW2_PORT;
    uint16_t pin = (switch_num == 1) ? SW1_PIN : SW2_PIN;
    
    while(HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET) {
        if((HAL_GetTick() - start_time) > timeout_ms) {
            return IQ32_TIMEOUT;
        }
        HAL_Delay(10);
    }
    
    HAL_Delay(DEBOUNCE_DELAY);
    return IQ32_OK;
}

// === EMERGENCY FUNCTIONS ===
IQ32_Result_t IQ32_EmergencyStop(void)
{
    IQ32_SetSystemState(IQ32_STATE_EMERGENCY);
    
    // Stop all motion
    PID_EmergencyStop();
    Motor_Stop();
    Fan_SetSpeed(0);
    
    // Visual indication
    LED_Blink(LED_ALL, 10, 100);
    return IQ32_OK;
}

IQ32_Result_t IQ32_EmergencyRecovery(void)
{
    if(IQ32_GetSystemState() != IQ32_STATE_EMERGENCY) {
        return IQ32_ERROR;
    }
    
    // Clear emergency conditions
    PID_ClearEmergencyStop();
    
    // Return to ready state
    IQ32_SetSystemState(IQ32_STATE_READY);
    LED_Off(LED_ALL);
    
    return IQ32_OK;
}

bool IQ32_IsEmergencyState(void)
{
    return (IQ32_GetSystemState() == IQ32_STATE_EMERGENCY) || PID_IsEmergencyStop();
}

// === DIAGNOSTIC FUNCTIONS ===
IQ32_Result_t IQ32_SelfTest(void)
{
    IQ32_SYSTEM_CHECK();
    
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 0, Font_6x8);
    ssd1306_WriteString("Self Test...", Font_6x8, White);
    ssd1306_UpdateScreen();
    
    // Test LEDs
    LED_Blink(LED1, 3, 200);
    LED_Blink(LED2, 3, 200);
    
    // Test ADC
    uint16_t adc_value;
    if(IQ32_ADC_Read(IQ32_ADC_CHANNEL_BATTERY, &adc_value) != IQ32_OK) {
        return IQ32_ERROR;
    }
    
    // Test IMU
    uint8_t whoami;
    if(MPU6500_WhoAmI(&whoami) != IQ32_OK || whoami != MPU6500_WHO_AM_I_VALUE) {
        return IQ32_ERROR;
    }
    
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 0, Font_6x8);
    ssd1306_WriteString("Self Test OK!", Font_6x8, White);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);
    
    return IQ32_OK;
}

IQ32_Result_t IQ32_RunDiagnostics(void)
{
    return IQ32_SelfTest();
}

void IQ32_DisplaySystemInfo(void)
{
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 0, Font_6x8);
    ssd1306_WriteString("IQ32 Robot", Font_6x8, White);
    
    char version_str[32];
    snprintf(version_str, sizeof(version_str), "FW: %s", IQ32_GetVersion());
    ssd1306_SetCursorLine(0, 1, Font_6x8);
    ssd1306_WriteString(version_str, Font_6x8, White);
    
    char uptime_str[32];
    snprintf(uptime_str, sizeof(uptime_str), "Up: %lu ms", IQ32_GetUptime());
    ssd1306_SetCursorLine(0, 2, Font_6x8);
    ssd1306_WriteString(uptime_str, Font_6x8, White);
    
    ssd1306_SetCursorLine(0, 3, Font_6x8);
    ssd1306_WriteString(IQ32_SystemStateToString(IQ32_GetSystemState()), Font_6x8, White);
    
    ssd1306_UpdateScreen();
}

void IQ32_DisplayErrorStatus(void)
{
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 0, Font_6x8);
    ssd1306_WriteString("System Status", Font_6x8, White);
    
    ssd1306_SetCursorLine(0, 1, Font_6x8);
    ssd1306_WriteString(IQ32_SystemStateToString(IQ32_GetSystemState()), Font_6x8, White);
    
    char adc_str[32];
    snprintf(adc_str, sizeof(adc_str), "ADC: %s", g_system_status.adc_ready ? "OK" : "FAIL");
    ssd1306_SetCursorLine(0, 2, Font_6x8);
    ssd1306_WriteString(adc_str, Font_6x8, White);
    
    char cal_str[32];
    snprintf(cal_str, sizeof(cal_str), "Cal: %s", g_system_status.sensors_calibrated ? "OK" : "NO");
    ssd1306_SetCursorLine(0, 3, Font_6x8);
    ssd1306_WriteString(cal_str, Font_6x8, White);
    
    ssd1306_UpdateScreen();
}

// === CALIBRATION FUNCTIONS ===
IQ32_Result_t IQ32_CalibrateAll(void)
{
    IQ32_SYSTEM_CHECK();
    IQ32_SetSystemState(IQ32_STATE_CALIBRATING);
    
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 0, Font_6x8);
    ssd1306_WriteString("Calibrating...", Font_6x8, White);
    ssd1306_UpdateScreen();
    
    // Calibrate sensors
    if(IQ32_CalibrateSensors(5000) != IQ32_OK) {
        return IQ32_ERROR;
    }
    
    // Calibrate IMU
    if(IQ32_CalibrateIMU() != IQ32_OK) {
        return IQ32_ERROR;
    }
    
    g_system_status.sensors_calibrated = true;
    IQ32_SetSystemState(IQ32_STATE_READY);
    
    ssd1306_Fill(Black);
    ssd1306_SetCursorLine(0, 0, Font_6x8);
    ssd1306_WriteString("Calibration OK!", Font_6x8, White);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);
    
    return IQ32_OK;
}

IQ32_Result_t IQ32_CalibrateSensors(uint32_t duration_ms)
{
    return LineSensor_Calibrate(duration_ms);
}

IQ32_Result_t IQ32_CalibrateIMU(void)
{
    // Basic IMU calibration - read samples and establish baseline
    float ax, ay, az, gx, gy, gz;
    
    for(int i = 0; i < 100; i++) {
        if(MPU6500_ReadAccelGyro(&ax, &ay, &az, &gx, &gy, &gz) != IQ32_OK) {
            return IQ32_ERROR;
        }
        HAL_Delay(10);
    }
    
    return IQ32_OK;
}

IQ32_Result_t IQ32_SaveCalibration(void)
{
    return PID_SaveSettings();
}

IQ32_Result_t IQ32_LoadCalibration(void)
{
    return PID_LoadSettings();
}
    __HAL_RCC_I2C1_CLK_ENABLE();
    
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = I2C_CLOCK_SPEED;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    
    return (HAL_I2C_Init(&hi2c1) == HAL_OK) ? IQ32_OK : IQ32_ERROR;
}

// === GPIO INITIALIZATION ===
IQ32_Result_t MX_GPIO_Init(void)
{
    // Enable all required clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Motor direction pins (PA8, PA9, PA10, PA11)
    GPIO_InitStruct.Pin = MOTOR1_INA_PIN | MOTOR1_INB_PIN | MOTOR2_INA_PIN | MOTOR2_INB_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PWM pins (PA0, PA1, PA2)
    GPIO_InitStruct.Pin = PWM_MOTOR1_PIN | PWM_MOTOR2_PIN | PWM_FAN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // LED pins
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = LED1_PIN;
    HAL_GPIO_Init(LED1_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = LED2_PIN;
    HAL_GPIO_Init(LED2_PORT, &GPIO_InitStruct);

    // Switch pins with pull-up
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Pin = SW1_PIN;
    HAL_GPIO_Init(SW1_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = SW2_PIN;
    HAL_GPIO_Init(SW2_PORT, &GPIO_InitStruct);

    // I2C pins
    GPIO_InitStruct.Pin = I2C_SCL_PIN | I2C_SDA_PIN;
=======
    // LED = PB4, PA15
   
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // SW = PA12 (Active-Low)
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // ใช้ Pull-up
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    // SW2 = PC9 (Active-Low)
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; 
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // ใช้ Pull-up
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // I2C1 = PB8(SCL), PB9(SDA)
    __HAL_RCC_I2C1_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
>>>>>>> 39340a1 (v1)
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

<<<<<<< HEAD
    return IQ32_OK;
}

// === MAIN INITIALIZATION ===
IQ32_Result_t IQ32_Init(void)
{
    if(HAL_Init() != HAL_OK) {
        return IQ32_ERROR;
    }

    // Initialize peripherals
    if(MX_GPIO_Init() != IQ32_OK ||
       MX_TIM2_Init() != IQ32_OK ||
       MX_I2C1_Init() != IQ32_OK) {
        return IQ32_ERROR;
    }

    // Initialize subsystems
    MUX_Init();
    Battery_Init();
    MPU6500_Init();
    ssd1306_Init();

    // Startup LED indication
    for(int i = 0; i < STARTUP_BLINK_COUNT; i++) {
        LED_On(LED1);
        LED_On(LED2);
        HAL_Delay(STARTUP_BLINK_DELAY);
        LED_Off(LED1);
        LED_Off(LED2);
        HAL_Delay(STARTUP_BLINK_DELAY);
    }

    return IQ32_OK;
}

// === SWITCH FUNCTIONS ===
void Wait_SW1(void)
{
    while(HAL_GPIO_ReadPin(SW1_PORT, SW1_PIN) == GPIO_PIN_SET) {}
    HAL_Delay(DEBOUNCE_DELAY);
    
    // Visual feedback
    for(int i = 0; i < 2; i++) {
        LED_On(LED2);
        HAL_Delay(100);
        LED_Off(LED2);
        HAL_Delay(100);
    }
}

void Wait_SW2(void)
{
    while(HAL_GPIO_ReadPin(SW2_PORT, SW2_PIN) == GPIO_PIN_SET) {}
    HAL_Delay(DEBOUNCE_DELAY);
    
    // Visual feedback
    for(int i = 0; i < 2; i++) {
        LED_On(LED1);
        HAL_Delay(100);
        LED_Off(LED1);
        HAL_Delay(100);
    }
}

uint8_t Read_SW1(void)
{
    if(HAL_GPIO_ReadPin(SW1_PORT, SW1_PIN) == GPIO_PIN_RESET) {
        LED_On(LED2);
        HAL_Delay(100);
        LED_Off(LED2);
=======
   

    // เริ่มต้นอุปกรณ์เสริมอื่นๆ
    MUX_Init(); // เริ่มต้น MUX
    Battery_Init(); // เริ่มต้น ADC  สำหรับวัดแรงดันแบตเตอรี่
    MPU6500_Init(); // เริ่มต้น MPU6500 (IMU)

   

}
// --- ฟังก์ชันเริ่มต้นบอร์ด IQ32 ---

void IQ32_Init(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_I2C1_Init();
    ssd1306_Init();
    for (int i = 0; i < 5; i++) {
        LED_On(LED1);
        LED_On(LED2);
        HAL_Delay(50);
        LED_Off(LED1);
        LED_Off(LED2);
        HAL_Delay(50);
    }
   
    // อนาคตถ้ามี UART, I2C, SPI → ใส่เพิ่ม
}



// Handle


void Wait_SW1(void){
    while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_SET){} // รอจนกว่าจะกด SW1
    HAL_Delay(50); // ดีเลย์ตัดเด้ง
    LED_On(LED2);
    HAL_Delay(100);
    LED_Off(LED2);
    HAL_Delay(100);
    LED_On(LED2); 
    HAL_Delay(100);
    LED_Off(LED2);
    HAL_Delay(100);
}
void Wait_SW2(void){
    while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == GPIO_PIN_SET){} // รอจนกว่าจะกด SW2
    HAL_Delay(50); // ดีเลย์ตัดเด้ง
    LED_On(LED1);
    HAL_Delay(100);
    LED_Off(LED1);
    HAL_Delay(100);
    LED_On(LED1);
    HAL_Delay(100);
    LED_Off(LED1);

}
uint8_t Read_SW1(void){
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET){
         LED_On(LED2);
         HAL_Delay(100);
         LED_Off(LED2);
>>>>>>> 39340a1 (v1)
        return 1;
    }
    return 0;
}
<<<<<<< HEAD

uint8_t Read_SW2(void)
{
    if(HAL_GPIO_ReadPin(SW2_PORT, SW2_PIN) == GPIO_PIN_RESET) {
        LED_On(LED1);
        HAL_Delay(100);
        LED_Off(LED1);
=======
uint8_t Read_SW2(void){
    if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9) == GPIO_PIN_RESET){
         LED_On(LED1);
         HAL_Delay(100);
         LED_Off(LED1);
        
>>>>>>> 39340a1 (v1)
        return 1;
    }
    return 0;
}