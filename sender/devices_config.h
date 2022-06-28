#pragma once
/*
device configuration template:

#define HOSTNAME              "esp32028"   // obligatory, max 9 characters
#define DEVICE_NAME           "Garage"     // obligatory, max 10 characters
#define BOARD_TYPE            1     // obligatory, 1=ESP32/WROOM,WROVER 2=ESP32S2 3=ESP32S3 4=ESP32C3
#define ENABLE_3V_GPIO        21    // if not equipped comment out - power for sensors from Vcc
#define ACTIVITY_LED_GPIO     25    // if not equipped comment out - it blinks when sensor is ON/ if not defined FW_UPGRADE_LED_GPIO then it blinks when upgrade firmware is ongoing
#define FW_UPGRADE_LED_GPIO   26    // if not equipped comment out - blinks when upgrade firmware is ongoing
#define POWER_GPIO            39    // pin 6, green LED on charger TP4056, HIGH on charging, LOW on full
#define CHARGING_GPIO         38    // pin 7, red   LED on charger TP4056, HIGH on full, LOW on charging
#define SLEEP_TIME            180   // seconds
#define USE_MAX17048          1     // fuel gauge - optional, 0 if not in use
#define USE_SHT31             1     // temp/hum - optional, 0 if not in use
#define USE_TSL2561           0     // lux SDA - optional, 0 if not in use
#define USE_TEPT4400          1     // lux ADC - optional, 0 if not in use
#define LUX_ADC_GPIO          36    // relevant only if USE_TEPT4400=1
#define LUX_MAX_RAW_READING   2828  // relevant only if USE_TEPT4400=1
#define USE_CUSTOM_I2C_GPIO   1     // instead of standard ones: 21,22 for ESP32S and 8,9 for ESP32S2), 0 if not in use
#define SDA_GPIO              18    // relevant only if USE_CUSTOM_I2C_GPIO=1
#define SCL_GPIO              19    // relevant only if USE_CUSTOM_I2C_GPIO=1
#define GND_GPIO_FOR_LED      13    // if not equipped comment out - GND for ACTIVITY_LED_GPIO
#define PERIODIC_FW_CHECK_HRS 24    // check FW update every hours

test devices: esp32080 - 099
*/


// *************** "esp32028" - S,  production - Garage ************************
#if DEVICE_ID == 28
  #define HOSTNAME              "esp32028"
  #define DEVICE_NAME           "Garage"
  #define BOARD_TYPE            1
  #define ENABLE_3V_GPIO        21
  #define ACTIVITY_LED_GPIO     25
  #define FW_UPGRADE_LED_GPIO   26
  #define CHARGING_GPIO         32
  #define POWER_GPIO            35
  #define SLEEP_TIME            180
  #define USE_MAX17048          1
  #define USE_SHT31             1
  #define USE_TSL2561           0
  #define USE_TEPT4400          1
  #define LUX_ADC_GPIO          36
  #define LUX_MAX_RAW_READING   2828
  #define USE_CUSTOM_I2C_GPIO   1
  #define SDA_GPIO              18
  #define SCL_GPIO              19
  // #define GND_GPIO_FOR_LED      13
  // #define PERIODIC_FW_CHECK_HRS
  #pragma message "compilation for: esp32028-Garage"

// *************** "esp32101" - S,  production - Dining ************************
#elif DEVICE_ID == 101
  #define HOSTNAME              "esp32101"
  #define DEVICE_NAME           "Dining"
  #define BOARD_TYPE            1
  #define ENABLE_3V_GPIO        19
  #define ACTIVITY_LED_GPIO     25
  #define FW_UPGRADE_LED_GPIO   26
  #define CHARGING_GPIO         17
  #define POWER_GPIO            16
  #define SLEEP_TIME            180
  #define USE_MAX17048          1
  #define USE_SHT31             1
  #define USE_TSL2561           1
  #define USE_TEPT4400          0
  #define LUX_ADC_GPIO          4
  #define LUX_MAX_RAW_READING   7500
  #define USE_CUSTOM_I2C_GPIO   0
  #define SDA_GPIO              18
  #define SCL_GPIO              19
  // #define GND_GPIO_FOR_LED      13
  // #define PERIODIC_FW_CHECK_HRS
  #pragma message "compilation for: esp32101-Dining"

// *************** "esp32102" - S,  production - Toilet ************************
#elif DEVICE_ID == 102
  #define HOSTNAME              "esp32102"
  #define DEVICE_NAME           "ToiletUp"
  #define BOARD_TYPE            1
  #define ENABLE_3V_GPIO        32
  #define ACTIVITY_LED_GPIO     25
  #define FW_UPGRADE_LED_GPIO   26
  #define CHARGING_GPIO         17
  #define POWER_GPIO            16
  #define SLEEP_TIME            180
  #define USE_MAX17048          1
  #define USE_SHT31             1
  #define USE_TSL2561           1
  #define USE_TEPT4400          0
  #define LUX_ADC_GPIO          4
  #define LUX_MAX_RAW_READING   7500
  #define USE_CUSTOM_I2C_GPIO   0
  #define SDA_GPIO              18
  #define SCL_GPIO              19
  // #define GND_GPIO_FOR_LED      13
  // #define PERIODIC_FW_CHECK_HRS
  #pragma message "compilation for: esp32102-ToiletUp"

// *************** "esp32104" - S,  production - Milena ************************
#elif DEVICE_ID == 104
  #define HOSTNAME              "esp32104"
  #define DEVICE_NAME           "Milena"
  #define BOARD_TYPE            1
  #define ENABLE_3V_GPIO        33
  #define ACTIVITY_LED_GPIO     25
  #define FW_UPGRADE_LED_GPIO   26
  #define CHARGING_GPIO         17
  #define POWER_GPIO            16
  #define SLEEP_TIME            180
  #define USE_MAX17048          1
  #define USE_SHT31             1
  #define USE_TSL2561           1
  #define USE_TEPT4400          0
  #define LUX_ADC_GPIO          4
  #define LUX_MAX_RAW_READING   7500
  #define USE_CUSTOM_I2C_GPIO   0
  #define SDA_GPIO              18
  #define SCL_GPIO              19
  // #define GND_GPIO_FOR_LED      13
  // #define PERIODIC_FW_CHECK_HRS
  #pragma message "compilation for: esp32104-Milena"

// *************** "esp32105" - S2, production - Garden ************************
#elif DEVICE_ID == 105
  #define HOSTNAME              "esp32105"
  #define DEVICE_NAME           "Garden"
  #define BOARD_TYPE            2
  #define ENABLE_3V_GPIO        5
  #define ACTIVITY_LED_GPIO     1
  // #define FW_UPGRADE_LED_GPIO   26
  #define CHARGING_GPIO         39
  #define POWER_GPIO            38
  #define SLEEP_TIME            180
  #define USE_MAX17048          1
  #define USE_SHT31             1
  #define USE_TSL2561           0
  #define USE_TEPT4400          1
  #define LUX_ADC_GPIO          4
  #define LUX_MAX_RAW_READING   6767
  #define USE_CUSTOM_I2C_GPIO   0
  #define SDA_GPIO              18
  #define SCL_GPIO              19
  // #define GND_GPIO_FOR_LED      13
  // #define PERIODIC_FW_CHECK_HRS
  // #pragma message "compilation for: esp32100-Table"
  #pragma message "compilation for: esp32105-Garden"
// =======================================================================================================

// *************** "esp32086" - S2, test - Lilygo1a ****************************
#elif DEVICE_ID == 86
  #define HOSTNAME              "esp32086"
  #define DEVICE_NAME           "Lilygo1a"
  #define BOARD_TYPE            2
  // #define ENABLE_3V_GPIO        32
  #define ACTIVITY_LED_GPIO     14
  // #define FW_UPGRADE_LED_GPIO   33
  // #define CHARGING_GPIO         38
  // #define POWER_GPIO            39
  #define SLEEP_TIME            5
  #define USE_MAX17048          0
  #define USE_SHT31             0
  #define USE_TSL2561           0
  #define USE_TEPT4400          0
  #define LUX_ADC_GPIO          36
  #define LUX_MAX_RAW_READING   2900
  #define USE_CUSTOM_I2C_GPIO   0
  #define SDA_GPIO              18
  #define SCL_GPIO              19
  #define GND_GPIO_FOR_LED      13
  // #define PERIODIC_FW_CHECK_HRS
  #pragma message "compilation for: esp32086-Lilygo1a"

// *************** "esp32087" - S,  test - S ***********************************
#elif DEVICE_ID == 87
  #define HOSTNAME              "esp32087"
  #define DEVICE_NAME           "S"
  #define BOARD_TYPE            1
  // #define ENABLE_3V_GPIO        32
  // #define ACTIVITY_LED_GPIO     33
  #define FW_UPGRADE_LED_GPIO   32
  // #define CHARGING_GPIO         38
  // #define POWER_GPIO            39
  #define SLEEP_TIME            5
  #define USE_MAX17048          0
  #define USE_SHT31             0
  #define USE_TSL2561           0
  #define USE_TEPT4400          0
  #define LUX_ADC_GPIO          36
  #define LUX_MAX_RAW_READING   2900
  #define USE_CUSTOM_I2C_GPIO   0
  #define SDA_GPIO              18
  #define SCL_GPIO              19
  // #define GND_GPIO_FOR_LED      13
  // #define PERIODIC_FW_CHECK_HRS
  #pragma message "compilation for: esp32087-S"

// *************** "esp32088" - S2, test - Lilygo2 *****************************
#elif DEVICE_ID == 88
  #define HOSTNAME              "esp32088"
  #define DEVICE_NAME           "Lilygo2"
  #define BOARD_TYPE            2
  // #define ENABLE_3V_GPIO        32
  #define ACTIVITY_LED_GPIO     14
  // #define FW_UPGRADE_LED_GPIO   33
  // #define CHARGING_GPIO         38
  // #define POWER_GPIO            39
  #define SLEEP_TIME            5
  #define USE_MAX17048          0
  #define USE_SHT31             0
  #define USE_TSL2561           0
  #define USE_TEPT4400          0
  #define LUX_ADC_GPIO          36
  #define LUX_MAX_RAW_READING   2900
  #define USE_CUSTOM_I2C_GPIO   0
  #define SDA_GPIO              18
  #define SCL_GPIO              19
  // #define GND_GPIO_FOR_LED      13
  #define PERIODIC_FW_CHECK_HRS 1
  #pragma message "compilation for: esp32088-Lilygo2"

// *************** "esp32089" - S2, test - Lilygo3a ****************************
#elif DEVICE_ID == 89
  #define HOSTNAME              "esp32089"
  #define DEVICE_NAME           "Lilygo3a"
  #define BOARD_TYPE            2
  // #define ENABLE_3V_GPIO        32
  #define ACTIVITY_LED_GPIO     14
  // #define FW_UPGRADE_LED_GPIO   33
  // #define CHARGING_GPIO         38
  // #define POWER_GPIO            39
  #define SLEEP_TIME            5
  #define USE_MAX17048          0
  #define USE_SHT31             0
  #define USE_TSL2561           0
  #define USE_TEPT4400          0
  #define LUX_ADC_GPIO          36
  #define LUX_MAX_RAW_READING   2900
  #define USE_CUSTOM_I2C_GPIO   0
  #define SDA_GPIO              18
  #define SCL_GPIO              19
  #define GND_GPIO_FOR_LED      13
  // #define PERIODIC_FW_CHECK_HRS
  #pragma message "compilation for: esp32089-Lilygo3a"

// *************** "esp32090" - S2, test - S2 **********************************
#elif DEVICE_ID == 90
  #define HOSTNAME              "esp32090"
  #define DEVICE_NAME           "testS2"
  #define BOARD_TYPE            2
  #define ENABLE_3V_GPIO        3
  // #define ACTIVITY_LED_GPIO     33
  // #define FW_UPGRADE_LED_GPIO   33
  #define CHARGING_GPIO         38
  #define POWER_GPIO            39
  #define SLEEP_TIME            6
  #define USE_MAX17048          1
  #define USE_SHT31             1
  #define USE_TSL2561           1
  #define USE_TEPT4400          0
  #define LUX_ADC_GPIO          36
  #define LUX_MAX_RAW_READING   2900
  #define USE_CUSTOM_I2C_GPIO   0
  #define SDA_GPIO              18
  #define SCL_GPIO              19
  // #define GND_GPIO_FOR_LED      13
  // #define PERIODIC_FW_CHECK_HRS
  #pragma message "compilation for: esp32090-testS2"

// *************** "esp32091" - S,  test - S ***********************************
#elif DEVICE_ID == 91
  #define HOSTNAME              "esp32091"
  #define DEVICE_NAME           "testS"
  #define BOARD_TYPE            1
  // #define ENABLE_3V_GPIO        32
  // #define ACTIVITY_LED_GPIO     33
  // #define FW_UPGRADE_LED_GPIO   33
  // #define CHARGING_GPIO         38
  // #define POWER_GPIO            39
  #define SLEEP_TIME            5
  #define USE_MAX17048          1
  #define USE_SHT31             0
  #define USE_TSL2561           0
  #define USE_TEPT4400          0
  #define LUX_ADC_GPIO          36
  #define LUX_MAX_RAW_READING   2900
  #define USE_CUSTOM_I2C_GPIO   0
  #define SDA_GPIO              18
  #define SCL_GPIO              19
  // #define GND_GPIO_FOR_LED      13
  // #define PERIODIC_FW_CHECK_HRS
  #pragma message "compilation for: esp32091-testS"

// ---------------------------------------------------------------------------------------------------

#else
  #error "Wrong DEVICE_ID chosen"
#endif


// assigning MODEL and checking if proper board is selected
#ifndef BOARD_TYPE
  #error BOARD_TYPE not defined
#else
  #if (BOARD_TYPE == 1) and (!defined(CONFIG_IDF_TARGET_ESP32))
    #error wrong board selected in Arduino - choose ESP32DEV
  #endif
  #if (BOARD_TYPE == 2) and (!defined(CONFIG_IDF_TARGET_ESP32S2))
    #error wrong board selected in Arduino - choose S2
  #endif
  #if (BOARD_TYPE == 3) and (!defined(CONFIG_IDF_TARGET_ESP32S3))
    #error wrong board selected in Arduino - choose S3
  #endif
  #if (BOARD_TYPE == 4) and (!defined(CONFIG_IDF_TARGET_ESP32C3))
    #error wrong board selected in Arduino - choose C3
  #endif

  #if (BOARD_TYPE == 1)
    #define MODEL "ESP32"
    #define ESP32_IS_CHEATING (325) // +310 head, +12 tail, +3ms delay between saved and save_ontime() function start
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32"
    #endif
  #elif (BOARD_TYPE == 2)
    #define MODEL "ESP32S2"
    #define ESP32_IS_CHEATING (-228)  // -244 head, +12 tail, +4ms delay between saved and save_ontime() function start
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32S2"
    #endif
  #elif (BOARD_TYPE == 3)
    #define MODEL "ESP32S3"
    #define ESP32_IS_CHEATING 0
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32S3"
    #endif
  #elif (BOARD_TYPE == 4)
    #define MODEL "ESP32C3"
    #define ESP32_IS_CHEATING 0
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32C3"
    #endif
  #else
    #error BOARD_TYPE not defined
  #endif
#endif
// assigning MODEL and checking if proper board is selected END
