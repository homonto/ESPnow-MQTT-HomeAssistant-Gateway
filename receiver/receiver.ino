/*
receiver.ino
*/
#define VERSION "1.7.0"
/*
2022-06-26:
  1.7.0 - incoming messages in the queue to avoid losing data

2022-06-24:
  1.6.2 - #define GND_GPIO_FOR_LED      13    // if not equipped comment out - GND for ACTIVITY_LED_GPIO
        - testing esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR);
          to extend the range of radio (no conclusion yet)
2022-06-22:
  1.6.1 - pretty_ontime implemented as string

2022-06-22:
  1.6.0 - ontime implemented in seconds

2022-06-21:
  1.5.0 - gateway firmware update implemented

2022-06-19:
  1.4.1 - cleaning for github

2022-06-18:
  1.4.0 - bootCount added

2022-06-15:
  1.3.0 - switch "publish" to enable/disable publishing to HA
        - buttons: update/restart
        - update FW on gateway not yet implemented

2022-06-11:
  1.2.0 - added device name on sender, myData.name on receiver, it goes to name of the device on HA

2022-06-11:
  1.1.0 - added lux from TEPT/ADC, added CHARGING

2022-06-10:
  1.0.0 - journey started ;-)
*/

// libraries
#include <esp_now.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include <PubSubClient.h>   // MQTT client
#include <ArduinoJson.h>
// Firmware update
#include <HTTPClient.h>
#include <Update.h>

// macros
#define GND_GPIO_FOR_LED    13    // if not equipped comment out - GND for ACTIVITY_LED_GPIO on some Lilygo boards

// fuctions declarations
// wifi.h
void setup_wifi();
// espnow.h
void espnow_start();
void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type);
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
// mqtt.h
void mqtt_reconnect();
// bool mqtt_publish_data();
void mqtt_callback(char* topic, byte* message, unsigned int length);
bool mqtt_publish_button_update_config();

// mqtt_publish_gw_data.h - UPDATE_INTERVAL
bool mqtt_publish_gw_status_config();
bool mqtt_publish_gw_status_values(const char* status);

// on message arrival from sensors
bool mqtt_publish_gw_last_updated_sensor_config();
bool mqtt_publish_gw_last_updated_sensor_values(const char* status);

// mqtt_publish_sensors_data.h - all at once
bool mqtt_publish_sensors_config(const char* hostname, const char* name, const char* mac, const char* fw);
bool mqtt_publish_sensors_values();
bool mqtt_publish_button_restart_config();
bool mqtt_publish_button_update_config();
bool mqtt_publish_switch_publish_config();
bool mqtt_publish_switch_publish_values();

// firmware update
void do_update();
void updateFirmware(uint8_t *data, size_t len);
int update_firmware_prepare();

// various, here - UPDATE_INTERVAL
void hearbeat();
void ConvertSectoDay(unsigned long n, char * pretty_ontime);


// custom files
#include "config.h"
#include "variables.h"
#include "wifi.h"
#include "espnow.h"
#include "mqtt.h"
#include "mqtt_publish_gw_data.h"
#include "mqtt_publish_sensors_data.h"
#include "fw_update.h"

void ConvertSectoDay(unsigned long n, char *pretty_ontime)
{
  size_t nbytes;
  int day = n / (24 * 3600);
  n = n % (24 * 3600);
  int hour = n / 3600;
  n %= 3600;
  int minutes = n / 60 ;
  n %= 60;
  int seconds = n;
  if (day>0)
  {
    nbytes = snprintf(NULL,0,"%0dd %0dh %0dm %0ds",day,hour,minutes,seconds)+1;
    snprintf(pretty_ontime,nbytes,"%0dd %0dh %0dm %0ds",day,hour,minutes,seconds);
  } else
  {
    if (hour>0)
    {
      nbytes = snprintf(NULL,0,"%0dh %0dm %0ds",hour,minutes,seconds)+1;
      snprintf(pretty_ontime,nbytes,"%0dh %0dm %0ds",hour,minutes,seconds);
    } else
    {
      if (minutes>0)
      {
        nbytes = snprintf(NULL,0,"%0dm %0ds",minutes,seconds)+1;
        snprintf(pretty_ontime,nbytes,"%0dm %0ds",minutes,seconds);
      } else
      {
        nbytes = snprintf(NULL,0,"%0ds",seconds)+1;
        snprintf(pretty_ontime,nbytes,"%0ds",seconds);
      }
    }
  }
  if (debug_mode)
    Serial.printf("\n\npretty_ontime=%s\n\n",pretty_ontime);
}


void hearbeat()
{
  bool publish_status = true;
  digitalWrite(STATUS_LED_GPIO,HIGH);
  publish_status = publish_status && mqtt_publish_gw_status_config();
  Serial.print("[heartbeat] - sending to HA "+String((millis()-aux_update_interval)/1000)+"s later......");

  publish_status = publish_status && mqtt_publish_gw_status_values("online");
  publish_status = publish_status && mqtt_publish_button_update_config();
  publish_status = publish_status && mqtt_publish_button_restart_config();
  publish_status = publish_status && mqtt_publish_gw_last_updated_sensor_config();
  publish_status = publish_status && mqtt_publish_switch_publish_values();

  int queue_count = uxQueueMessagesWaiting(queue);
  if (queue_count == MAX_QUEUE_COUNT)
  {
    publish_status = publish_status && mqtt_publish_gw_last_updated_sensor_values("queue FULL");
  }

  if (publish_status)
  {
    Serial.println("SUCCESSFULL");
  } else
  {
    Serial.println("FAILED");
  }

  if (queue_count == MAX_QUEUE_COUNT)
  {
    Serial.println("ERROR: queue is full!");
  }

  digitalWrite(STATUS_LED_GPIO,LOW);
}


void setup()
{
  program_start_time = millis();
  Serial.begin(115200);
  delay(100);
  Serial.println("\n\n=============================================================");
  Serial.println("GATEWAY started, version: "+String(VERSION));
  #ifdef GND_GPIO_FOR_LED
    Serial.println("Enabling GND for LED");
    pinMode(GND_GPIO_FOR_LED, OUTPUT);
    digitalWrite(GND_GPIO_FOR_LED, LOW);
  #endif
  pinMode(STATUS_LED_GPIO,OUTPUT);

  queue = xQueueCreate( MAX_QUEUE_COUNT, sizeof( struct struct_message ) );
  if(queue == NULL)
  {
    Serial.println("Error creating the queue");
    delay(10000000);
  } else
  {
    Serial.println("Created the queue");
  }

  queue_aux = xQueueCreate( MAX_QUEUE_COUNT, sizeof( struct struct_message_aux ) );
  if(queue_aux == NULL)
  {
    Serial.println("Error creating the queue_aux");
    delay(10000000);
  } else
  {
    Serial.println("Created the queue_aux");
  }

  setup_wifi();
  if (!wifi_connected)
  {
    Serial.println("WiFi NOT connected, REBOTTING in 3s");
    delay(3000);
    ESP.restart();
  }

  //start MQTT service
  mqttc.setServer(HA_MQTT_SERVER, 1883);
  mqttc.setBufferSize(MQTT_PAYLOAD_MAX_SIZE);
  mqttc.setCallback(mqtt_callback);
  mqtt_reconnect();
  if (!mqtt_connected)
  {
    Serial.println("MQTT NOT connected, REBOTTING in 3s");
    delay(3000);
    ESP.restart();
  }

  Serial.print("configuring GATEWAY status in HA:...");
  if (mqtt_publish_gw_status_config())
  {
    Serial.println("done");
  } else
  {
    Serial.println("FAILED");
    Serial.println("configuring GATEWAY FAILED, REBOTTING in 3s");
    delay(3000);
    ESP.restart();
  }
  hearbeat();
  espnow_start();
}


void loop()
{
  long start_loop_time = millis();

  if (WiFi.status() != WL_CONNECTED) setup_wifi();
  if (!mqttc.connected()) mqtt_reconnect();
  mqttc.loop();
  do_update();

  if (start_loop_time > (aux_update_interval + UPDATE_INTERVAL))
  {
    hearbeat();
    aux_update_interval = start_loop_time;
  }

  int queue_count = uxQueueMessagesWaiting(queue);
  if (queue_count > 0)
  {
    if (debug_mode) Serial.printf("queue size=%d\n",queue_count);
    if (publish_sensors_to_ha)
    {
      mqtt_publish_sensors_values();
    }
  }
}
