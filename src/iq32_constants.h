#ifndef __IQ32_CONSTANTS_H
#define __IQ32_CONSTANTS_H

#include <stdint.h>
#include <stdbool.h>

// === ERROR CODES ===
typedef enum {
    IQ32_OK = 0,
    IQ32_ERROR = -1,
    IQ32_INVALID_PARAM = -2,
    IQ32_TIMEOUT = -3,
    IQ32_NOT_INITIALIZED = -4,
    IQ32_OUT_OF_RANGE = -5
} IQ32_Result_t;

// === GPIO PIN DEFINITIONS ===
// Motor Control Pins
#define MOTOR1_INA_PORT    GPIOA
#define MOTOR1_INA_PIN     GPIO_PIN_8
#define MOTOR1_INB_PORT    GPIOA
#define MOTOR1_INB_PIN     GPIO_PIN_9
#define MOTOR2_INA_PORT    GPIOA
#define MOTOR2_INA_PIN     GPIO_PIN_10
#define MOTOR2_INB_PORT    GPIOA
#define MOTOR2_INB_PIN     GPIO_PIN_11

// PWM Pins (Timer2)
#define PWM_MOTOR1_PIN     GPIO_PIN_0  // PA0 - TIM2_CH1
#define PWM_MOTOR2_PIN     GPIO_PIN_1  // PA1 - TIM2_CH2
#define PWM_FAN_PIN        GPIO_PIN_2  // PA2 - TIM2_CH3

// LED Pins
#define LED1_PORT          GPIOB
#define LED1_PIN           GPIO_PIN_4
#define LED2_PORT          GPIOA
#define LED2_PIN           GPIO_PIN_15

// Switch Pins
#define SW1_PORT           GPIOA
#define SW1_PIN            GPIO_PIN_12
#define SW2_PORT           GPIOC
#define SW2_PIN            GPIO_PIN_9

// MUX Control Pins
#define MUX_S0_PORT        GPIOC
#define MUX_S0_PIN         GPIO_PIN_7
#define MUX_S1_PORT        GPIOC
#define MUX_S1_PIN         GPIO_PIN_6
#define MUX_S2_PORT        GPIOB
#define MUX_S2_PIN         GPIO_PIN_10
#define MUX_S3_PORT        GPIOB
#define MUX_S3_PIN         GPIO_PIN_2

// ADC Pins
#define MUX_SIGNAL_PIN     GPIO_PIN_3  // PA3
#define MUX_SIGNAL_CHANNEL ADC_CHANNEL_3
#define BATTERY_ADC_PIN    GPIO_PIN_4  // PC4
#define BATTERY_ADC_CHANNEL ADC_CHANNEL_14

// I2C Pins
#define I2C_SCL_PORT       GPIOB
#define I2C_SCL_PIN        GPIO_PIN_8
#define I2C_SDA_PORT       GPIOB
#define I2C_SDA_PIN        GPIO_PIN_9

// SPI Pins (MPU6500)
#define SPI_SCK_PORT       GPIOA
#define SPI_SCK_PIN        GPIO_PIN_5
#define SPI_MISO_PORT      GPIOA
#define SPI_MISO_PIN       GPIO_PIN_6
#define SPI_MOSI_PORT      GPIOA
#define SPI_MOSI_PIN       GPIO_PIN_7
#define SPI_CS_PORT        GPIOA
#define SPI_CS_PIN         GPIO_PIN_4

// === TIMING CONSTANTS ===
#define TIMER_PRESCALER    83          // 84MHz -> 1MHz
#define PWM_PERIOD         999         // 1kHz PWM
#define PWM_MAX_DUTY       1000
#define DEBOUNCE_DELAY     50          // ms
#define MUX_SETTLE_TIME    5           // ms
#define STARTUP_BLINK_COUNT 5
#define STARTUP_BLINK_DELAY 50         // ms

// === ADC CONSTANTS ===
#define ADC_RESOLUTION     4095        // 12-bit ADC
#define ADC_VREF           3.3f        // Reference voltage
#define ADC_SAMPLE_TIME    ADC_SAMPLETIME_480CYCLES

// Battery voltage divider
#define BATTERY_R1         1000.0f     // Ohm
#define BATTERY_R2         220.0f      // Ohm
#define BATTERY_DIVIDER    ((BATTERY_R1 + BATTERY_R2) / BATTERY_R2)
#define BATTERY_CAL_FACTOR 1.0131f
#define BATTERY_AVG_SAMPLES 100

// === LINE SENSOR CONSTANTS ===
#define MAX_SENSORS        16
#define DEFAULT_THRESHOLD  2000
#define SENSOR_CENTER      ((MAX_SENSORS - 1) * 1000 / 2)

// === PID CONSTANTS ===
#define DEFAULT_KP         0.5f
#define DEFAULT_KD         0.1f
#define DEFAULT_ALPHA      0.8f
#define DEFAULT_BASE_SPEED 200
#define DEFAULT_MAX_SPEED  800
#define MAX_CONTROL_SIGNAL 800
#define MIN_CONTROL_SIGNAL -800

// Out of line handling
#define DEFAULT_OUT_LEFT_SPEED  300
#define DEFAULT_OUT_RIGHT_SPEED -300
#define DEFAULT_MAX_OUT_TIME    500    // ms

// === MOTOR LIMITS ===
#define MOTOR_MAX_SPEED    1000
#define MOTOR_MIN_SPEED    -1000

// === MPU6500 CONSTANTS ===
#define MPU6500_WHO_AM_I_REG    0x75
#define MPU6500_WHO_AM_I_VALUE  0x70
#define MPU6500_PWR_MGMT_1      0x6B
#define MPU6500_ACCEL_XOUT_H    0x3B
#define MPU6500_GYRO_XOUT_H     0x43

// Conversion factors
#define ACCEL_SCALE_2G     16384.0f    // LSB/g for ±2g
#define GYRO_SCALE_250DPS  131.0f      // LSB/(°/s) for ±250°/s

// === I2C CONSTANTS ===
#define I2C_CLOCK_SPEED    400000      // 400kHz
#define I2C_TIMEOUT        1000        // ms

// === CALIBRATION CONSTANTS ===
#define DEFAULT_CALIB_TIME 5000        // ms
#define BLINK_INTERVAL     500         // ms for calibration indication

// === PERFORMANCE CONSTANTS ===
#define PERF_UPDATE_INTERVAL 10        // ms
#define MAX_ERROR_THRESHOLD  500       // for oscillation detection

// === AUTO-TUNE CONSTANTS ===
#define AUTOTUNE_MAX_ATTEMPTS  20
#define AUTOTUNE_TEST_DURATION 5000    // ms
#define AUTOTUNE_MIN_OSC_COUNT 8
#define ZIEGLER_NICHOLS_KP     0.6f
#define ZIEGLER_NICHOLS_KD_DIV 8000.0f

// === ADAPTIVE PID CONSTANTS ===
#define HIGH_ACCEL_THRESHOLD   1.5f
#define LOW_ACCEL_THRESHOLD    0.7f
#define HIGH_GYRO_THRESHOLD    150.0f
#define ADAPTIVE_KP_HIGH_ACCEL 0.6f
#define ADAPTIVE_KD_HIGH_ACCEL 1.4f
#define ADAPTIVE_KP_LOW_ACCEL  1.3f
#define ADAPTIVE_KD_LOW_ACCEL  0.7f
#define ADAPTIVE_KD_HIGH_GYRO  0.4f

// === EMERGENCY STOP CONSTANTS ===
#define EMERGENCY_BLINK_COUNT  20
#define EMERGENCY_BLINK_DELAY  50      // ms

// === HELPER MACROS ===
#define CONSTRAIN(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define SIGN(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

// === VALIDATION MACROS ===
#define VALIDATE_RANGE(x, min, max) ((x) >= (min) && (x) <= (max))
#define VALIDATE_NOT_NULL(ptr) ((ptr) != NULL)

// === BIT MANIPULATION MACROS ===
#define SET_BIT(reg, bit) ((reg) |= (1U << (bit)))
#define CLEAR_BIT(reg, bit) ((reg) &= ~(1U << (bit)))
#define TOGGLE_BIT(reg, bit) ((reg) ^= (1U << (bit)))
#define GET_BIT(reg, bit) (((reg) >> (bit)) & 1U)

#endif // __IQ32_CONSTANTS_H