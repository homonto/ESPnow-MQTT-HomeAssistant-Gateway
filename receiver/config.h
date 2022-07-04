#pragma once
/*
config
*/

#include <passwords.h>          // passwords inside my library - for my personal usage
// #include "passwords.h"          // passwords in current folder - for github - see the structure inside the file

#include "variables.h"

// custom for this device - Gateway - different boards
#if DEVICE_ID == 29
  #define BOARD_TYPE          2 // 1=ESP32 2=ESP32S2 3=ESP32S3 4=ESP32C3
  #define HOSTNAME            "esp32029"
  #define IP_ADDRESS          "192.168.1.29"
  #define ROLE_NAME           "gw1"
  #define SENSORS_LED_GPIO_BLUE     3         // blinking during sending sensors data to HA - blue?
  #define STATUS_GW_LED_GPIO_RED    2         // blinking during sending gateway data to HA - red?
  #define POWER_ON_LED_GPIO_GREEN   5         // status of device - green?
  #define POWER_ON_LED_USE_PWM      1         // set to 1 if PWM to be used, 0 for fixed brightness
  #define POWER_ON_LED_DC           30        // DC for Green LED
  // fixed MAC address assigned to ther receiver, to be able to use any ESP32 without changing the code of sender
  // chose any free one [i.e. from broken ESP] and match with sender.ino
  uint8_t FixedMACAddress[] = {0x7c, 0xdF, 0xa1, 0x0b, 0xd9, 0xff};
  #pragma message "compilation for: ESPnow_esp32029_gw1"

#elif DEVICE_ID == 30
  #define BOARD_TYPE          2 // 1=ESP32 2=ESP32S2 3=ESP32S3 4=ESP32C3
  #define HOSTNAME            "esp32030"
  #define IP_ADDRESS          "192.168.1.30"
  #define ROLE_NAME           "gw2"
  #define SENSORS_LED_GPIO_BLUE     2         // blinking during sending sensors data to HA - blue?
  #define STATUS_GW_LED_GPIO_RED    3         // blinking during sending gateway data to HA - red?
  #define POWER_ON_LED_GPIO_GREEN   5         // status of device - green?
  #define POWER_ON_LED_USE_PWM      1         // set to 1 if PWM to be used, 0 for fixed brightness
  #define POWER_ON_LED_DC           20        // DC for Green LED
  // fixed MAC address assigned to ther receiver, to be able to use any ESP32 without changing the code of sender
  // chose any free one [i.e. from broken ESP] and match with sender.ino
  uint8_t FixedMACAddress[] = {0x7c, 0xdF, 0xa1, 0x0b, 0xd9, 0xee};
  #pragma message "compilation for: ESPnow_esp32030_gw2"

#elif DEVICE_ID == 91
  #define BOARD_TYPE          1 // 1=ESP32 2=ESP32S2 3=ESP32S3 4=ESP32C3
  #define HOSTNAME            "esp32091"
  #define IP_ADDRESS          "192.168.1.91"
  #define ROLE_NAME           "gw3"
  #define SENSORS_LED_GPIO_BLUE     2         // blinking during sending sensors data to HA - blue?
  #define STATUS_GW_LED_GPIO_RED    3         // blinking during sending gateway data to HA - red?
  #define POWER_ON_LED_GPIO_GREEN   5         // status of device - green?
  #define POWER_ON_LED_USE_PWM      1         // set to 1 if PWM to be used, 0 for fixed brightness
  #define POWER_ON_LED_DC           20        // DC for Green LED
  // fixed MAC address assigned to ther receiver, to be able to use any ESP32 without changing the code of sender
  // chose any free one [i.e. from broken ESP] and match with sender.ino
  uint8_t FixedMACAddress[] = {0x7c, 0xdF, 0xa1, 0x0b, 0xd9, 0xfe};
  #pragma message "compilation for: ESPnow_esp32091_gw3"

#else
  #error "Wrong DEVICE_ID chosen"
#endif

// wifi common settings - often changed
// channel must be the same as for senders!
#define WIFI_CHANNEL        8
#define IP_GATEWAY          "192.168.1.1"
#define IP_SUBNET           "255.255.255.0"
#define IP_DNS              "192.168.1.1"

// for PubSubClient:
// it is used in: mqttc.setBufferSize(MQTT_PAYLOAD_MAX_SIZE); to increase max packet size
//default is 256 but it is not enough in this program - check debug in case of issue
#define MQTT_PAYLOAD_MAX_SIZE   1024
// for ArduinoJson
#define JSON_CONFIG_SIZE        1024 // config is bigger than payload due to device information
#define JSON_PAYLOAD_SIZE       512

// loop update interval ms
#define UPDATE_INTERVAL         (10 * 1000) //10000

// mqtt - how many attempts to connect to MQTT broker before restarting
#define MAX_MQTT_ERROR          10

// queue size for incomming data from sensors, whe queue is full no more data is gathered until there is free space in the queue
// set it to as much as many sensor devices you have so each one gets into the queue
#define MAX_QUEUE_COUNT         6

// LED PWM settings
#ifdef POWER_ON_LED_GPIO_GREEN
  #define POWER_ON_LED_PWM_CHANNEL    1
  #define POWER_ON_LED_PWM_RESOLUTION 8
  #define POWER_ON_LED_PWM_FREQ       5000
  #define POWER_ON_LED_MIN_DC         2
  #define POWER_ON_LED_MAX_DC         255
#endif

// assigning MODEL and checking if proper board is selected
#define PRINT_COMPILER_MESSAGES // comment out to disable messages in precompiler
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
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32"
    #endif
  #elif (BOARD_TYPE == 2)
    #define MODEL "ESP32S2"
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32S2"
    #endif
  #elif (BOARD_TYPE == 3)
    #define MODEL "ESP32S3"
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32S3"
    #endif
  #elif (BOARD_TYPE == 4)
    #define MODEL "ESP32C3"
    #ifdef PRINT_COMPILER_MESSAGES
      #pragma message "chosen BOARD_TYPE = ESP32C3"
    #endif
  #else
    #error BOARD_TYPE not defined
  #endif


#endif
// assigning MODEL and checking if proper board is selected END
