#pragma once
/*
variables
*/
#include "config.h"

// data structure for sensors
typedef struct struct_message {
  char host[10];
  char temp[6];
  char hum[6];
  char lux[6];
  char bat[6];
  char batpct[6];
  char ver[10];
  char charg[5];
  char name[11];
  char boot[5];
  unsigned long ontime;
} struct_message;
// espnow data structure END

// MAC of the sender
char macStr[18];

// wifi
bool wifi_connected = false;
WiFiClient espClient;

// mqtt
bool mqtt_connected = false;
PubSubClient mqttc(espClient);

// data from sensors
struct_message myData;

// critical for OnDataRecv
portMUX_TYPE receive_cb_mutex    = portMUX_INITIALIZER_UNLOCKED;
volatile bool data_to_send  = false;
// critical for OnDataRecv END

// measured rssi level by receiver between receiver and sender (not from router!)
int rssi;

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
