#pragma once
/*
last updated to the new version: esp32100

device configuration template:

#define HOSTNAME              "esp32028"   // obligatory, max 9 characters
#define DEVICE_NAME           "Garage"     // obligatory, max 15 characters
#define BOARD_TYPE            1     // obligatory, 1=ESP32/WROOM,WROVER 2=ESP32S2 3=ESP32S3 4=ESP32C3
#define ENABLE_3V_GPIO        21    // if not equipped comment out - power for sensors from Vcc
#define ACT_BLUE_LED_GPIO     25    // if not equipped comment out - it blinks when sensor is ON/ if not defined ERROR_RED_LED_GPIO then it blinks when upgrade firmware is ongoing or on error
#define ERROR_RED_LED_GPIO    26    // if not equipped comment out - blinks when upgrade firmware is ongoing and or error
#define POWER_GPIO            38    // pin 6, green LED on charger TP4056, HIGH on charging, LOW on full
#define CHARGING_GPIO         39    // pin 7, red   LED on charger TP4056, HIGH on full, LOW on charging
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
#define GND_GPIO_FOR_LED      13    // if not equipped comment out - GND for ACT_BLUE_LED_GPIO
#define PERIODIC_FW_CHECK_HRS 24    // check FW update every hours

test devices: esp32080 - 099
*/


// *************** "esp32028" - S,  production - Garage ************************
#if   DEVICE_ID == 28
  #define HOSTNAME              "esp32028"
  #define DEVICE_NAME           "Garage"
  #define BOARD_TYPE            1
  #define ENABLE_3V_GPIO        21
  #define ACT_BLUE_LED_GPIO     25
  #define ERROR_RED_LED_GPIO    26
  #define CHARGING_GPIO         35
  #define POWER_GPIO            32
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

// *************** "esp32100" - S2, production - Papa ************************
#elif DEVICE_ID == 100
  #define HOSTNAME              "esp32100"
  #define DEVICE_NAME           "Papa"
  #define BOARD_TYPE            2
  #define ENABLE_3V_GPIO        3
  #define ACT_BLUE_LED_GPIO     6
  #define ERROR_RED_LED_GPIO    5
  #define CHARGING_GPIO         39
  #define POWER_GPIO            38
  #define SLEEP_TIME            180
  #define USE_MAX17048          1
  #define USE_SHT31             1
  #define USE_TSL2561           1
  #define USE_TEPT4400          0
  #define LUX_ADC_GPIO          13
  #define LUX_MAX_RAW_READING   2900
  // #define USE_CUSTOM_I2C_GPIO   0
  // #define SDA_GPIO              18
  // #define SCL_GPIO              19
  // #define GND_GPIO_FOR_LED      13
  // #define PERIODIC_FW_CHECK_HRS
  #pragma message "compilation for: esp32100-Papa-NEW device"

// *************** "esp32101" - S,  production - Dining ************************
#elif DEVICE_ID == 101
  #define HOSTNAME              "esp32101"
  #define DEVICE_NAME           "Dining"
  #define BOARD_TYPE            1
  #define ENABLE_3V_GPIO        19
  #define ACT_BLUE_LED_GPIO     25
  #define ERROR_RED_LED_GPIO    26
  #define CHARGING_GPIO         16
  #define POWER_GPIO            17
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
  #define ACT_BLUE_LED_GPIO     25
  #define ERROR_RED_LED_GPIO    26
  #define CHARGING_GPIO         16
  #define POWER_GPIO            17
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
  #define ACT_BLUE_LED_GPIO     25
  #define ERROR_RED_LED_GPIO    26
  #define CHARGING_GPIO         16
  #define POWER_GPIO            17
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
  #define ENABLE_3V_GPIO        3
  #define ACT_BLUE_LED_GPIO     6
  #define ERROR_RED_LED_GPIO    5
  #define CHARGING_GPIO         38
  #define POWER_GPIO            39
  #define SLEEP_TIME            180
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
  #pragma message "compilation for: esp32105-Garden"

// =======================================================================================================

// *************** "esp32086" - S2, test -  ****************************
#elif DEVICE_ID == 86
  #define HOSTNAME              "esp32086"
  #define DEVICE_NAME           "new_dev"
  #define BOARD_TYPE            2
  #define ENABLE_3V_GPIO        3
  #define ACT_BLUE_LED_GPIO     6
  #define ERROR_RED_LED_GPIO    5
  #define CHARGING_GPIO         39
  #define POWER_GPIO            38
  #define SLEEP_TIME            5
  #define USE_MAX17048          1
  #define USE_SHT31             1
  #define USE_TSL2561           1
  #define USE_TEPT4400          0
  // #define LUX_ADC_GPIO          36
  // #define LUX_MAX_RAW_READING   2900
  // #define USE_CUSTOM_I2C_GPIO   0
  // #define SDA_GPIO              18
  // #define SCL_GPIO              19
  // #define GND_GPIO_FOR_LED      13
  // #define PERIODIC_FW_CHECK_HRS
  #pragma message "compilation for: esp32086-new_device"
//

// *************** "esp32089" - S2, test - Lilygo3 ****************************
#elif DEVICE_ID == 89
  #define HOSTNAME              "esp32089"
  #define DEVICE_NAME           "Lilygo3"
  #define BOARD_TYPE            2
  // #define ENABLE_3V_GPIO        32
  #define ACT_BLUE_LED_GPIO     14
  #define ERROR_RED_LED_GPIO    14
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
  #pragma message "compilation for: esp32089-Lilygo3"


// *************** "esp32091" - S,  test - S ***********************************
#elif DEVICE_ID == 91
  #define HOSTNAME              "esp32091"
  #define DEVICE_NAME           "testS"
  #define BOARD_TYPE            1
  #define ENABLE_3V_GPIO        32
  // #define ACT_BLUE_LED_GPIO     33
  // #define ERROR_RED_LED_GPIO   33
  // #define CHARGING_GPIO         38
  // #define POWER_GPIO            39
  #define SLEEP_TIME            65
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
  #pragma message "compilation for: esp32091-testS"

// *************** "esp32092" - S3,  test - S3 ***********************************

#elif DEVICE_ID == 92
  #define HOSTNAME              "esp32092"
  #define DEVICE_NAME           "testS3"
  #define BOARD_TYPE            3
  // #define ENABLE_3V_GPIO        32
  #define ACT_BLUE_LED_GPIO     7
  #define ERROR_RED_LED_GPIO    5
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
  #pragma message "compilation for: esp32092-testS3"


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
    #define ESP32_IS_CHEATING 0       // not tested yet
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32S3"
    #endif
  #elif (BOARD_TYPE == 4)
    #define MODEL "ESP32C3"
    #define ESP32_IS_CHEATING 0       // not tested yet
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32C3"
    #endif
  #else
    #error BOARD_TYPE not defined
  #endif
#endif
// assigning MODEL and checking if proper board is selected END
