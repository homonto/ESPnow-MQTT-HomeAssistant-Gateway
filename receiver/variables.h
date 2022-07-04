#pragma once
/*
variables
*/
#include "config.h"

// data structure for sensors
// typedef struct struct_message {
//   char host[10];
//   char temp[6];
//   char hum[6];
//   char lux[6];
//   char bat[6];
//   char batpct[6];
//   char ver[10];
//   char charg[5];
//   char name[11];
//   char boot[5];
//   unsigned long ontime;
// } struct_message;
typedef struct struct_message
{
  char host[10];        // esp32123 [9]
  char temp[7];         // 123.56   [7]
  char hum[7];          // 123.56   [7]
  char lux[6];          // 12345    [6]
  char bat[5];          // 1.34     [5]
  char batpct[8];       // 123.56   [7]
  char ver[10];         // 123.56.89[10]
  char charg[5];        // FULL     [5]
  char name[11];        //          [11]
  char boot[6];         // 12345    [6]
  unsigned long ontime;
  char batchr[10];       // -234.6789 [10]
} struct_message;
// espnow data structure END

// data structure for sensors - auxiliary with RSSI and MAC (of sender)
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

// global others
bool debug_mode = false;  // change to true to see tones of messages
long aux_update_interval = 0;
bool publish_sensors_to_ha = true;
long tt, program_start_time;
// global others END
