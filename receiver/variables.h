#pragma once
/*
variables
*/
#include "config.h"

// data structure for sensors
/* TODO:

*/

typedef struct struct_message          // 88 bytes
{
  char host[10];        // esp32123   - 9 characters maximum (esp32123=8)
  char name[16];        // 15 characters maximum
  char ver[10];         // 123.56.89  - 9 characters maximum (123.56.89=9)
  char sender_type[10]; // "motion" "env", space for others as well in the future
  char charg[5];        // "FULL","ON","NC" - 4 characters maximum
  float temp;
  float hum;
  float lux;
  float bat;
  float batpct;
  float batchr;
  byte motion;          // 0 - no motion, 1 - motion
  unsigned int boot;
  unsigned long ontime; // seconds, probably unsigned int would be enough - check it
} struct_message;
// espnow data structure END

// auxiliary data structure for sensors - with RSSI and MAC (of sender)
typedef struct struct_message_aux {
  int rssi;
  char macStr[18];
} struct_message_aux;
// espnow data structure END

// queue for messages comming - buffer
QueueHandle_t queue;

// queue_aux for aux messages comming - buffer
QueueHandle_t queue_aux;

// wifi
bool wifi_connected = false;
WiFiClient espClient;

// mqtt
bool mqtt_connected = false;
PubSubClient mqttc(espClient);

// data from sensors
struct_message myData;

// aux_data from/for sensors: rssi and MAC
struct_message_aux myData_aux;

// local copies after receiving to avoid overwriting
struct_message myLocalData;
struct_message_aux myLocalData_aux;

// critical for OnDataRecv
portMUX_TYPE receive_cb_mutex    = portMUX_INITIALIZER_UNLOCKED;

// firmware update
HTTPClient firmware_update_client;
int fw_totalLength = 0;
int fw_currentLength = 0;
bool perform_update_firmware=false;
int update_progress=0;
int old_update_progress=0;
bool blink_led_status=false;

#ifdef MOTION_SENSOR_GPIO
  // motion
  volatile bool motion = false;
  portMUX_TYPE motion_mutex    = portMUX_INITIALIZER_UNLOCKED;
  volatile unsigned long start_motion_ms = 0;
  volatile unsigned long last_motion_ms = 0;
  bool motion_printed = false;
  int motion_delay_s = 10;
#endif

// global others
bool debug_mode = false;  // change to true to see tones of messages
unsigned long aux_update_interval = 0;
bool publish_sensors_to_ha = true;
unsigned long tt, program_start_time;

// global others END
