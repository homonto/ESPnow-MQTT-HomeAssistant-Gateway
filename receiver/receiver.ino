/*
receiver.ino
*/
#define VERSION "1.6.1"
/*
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


// fuctions declarations
// wifi.h
void setup_wifi();
// espnow.h
void espnow_start();
void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type);
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
// mqtt.h
void mqtt_reconnect();
bool mqtt_publish_data();
void mqtt_callback(char* topic, byte* message, unsigned int length);
bool mqtt_publish_button_update_config();

// mqtt_publish_gw_data.h - UPDATE_INTERVAL
bool mqtt_publish_gw_status_config();
bool mqtt_publish_gw_status_values(const char* status);

// on message arrival from sensors
bool mqtt_publish_gw_last_updated_sensor_config();
bool mqtt_publish_gw_last_updated_sensor_values(const char* status);

// mqtt_publish_sensors_data.h - all at once
bool mqtt_publish_sensors_config(const char* hostname);
bool mqtt_publish_sensors_values(const char* hostname);
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
  if (debug_mode) Serial.printf("\n\nontime for %s=%s\n\n",myData.host,pretty_ontime);
}


void hearbeat()
{
  bool publish_status = true;
  digitalWrite(STATUS_LED_GPIO,HIGH);
  Serial.print("[heartbeat] - sending to HA "+String((millis()-aux_update_interval)/1000)+"s later......");

  publish_status = publish_status && mqtt_publish_gw_status_values("online");
  publish_status = publish_status && mqtt_publish_button_update_config();
  publish_status = publish_status && mqtt_publish_button_restart_config();
  publish_status = publish_status && mqtt_publish_gw_last_updated_sensor_config();
  publish_status = publish_status && mqtt_publish_switch_publish_values();

  if (publish_status)
  {
    Serial.println("SUCCESSFULL");
  } else
  {
    Serial.println("FAILED");
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
  pinMode(STATUS_LED_GPIO,OUTPUT);

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

  if (WiFi.status() != WL_CONNECTED)
  {
    setup_wifi();
  }

  if (!mqttc.connected())
  {
    mqtt_reconnect();
  }
  mqttc.loop();

  do_update();

  if (start_loop_time > (aux_update_interval + UPDATE_INTERVAL))
  {
    hearbeat();
    aux_update_interval = start_loop_time;
  }

  if (data_to_send)
  {
    if (publish_sensors_to_ha)
    {
      mqtt_publish_data();
    } else
    {
      Serial.print("Not publishing to HA as publish_sensors_to_ha=");Serial.println(publish_sensors_to_ha);
    }
    // protect the vunerable ones ;-)
    portENTER_CRITICAL(&receive_cb_mutex);
    data_to_send = false;
    portEXIT_CRITICAL(&receive_cb_mutex);
  }
}
