// #define DEBUG

#define DEVICE_ID  29           // "esp32029" - S2, MAC: FF, odd bootCount or all if BROADCAST
// #define DEVICE_ID  30           // "esp32030" - S2, MAC: EE, even bootCount or all if BROADCAST
// #define DEVICE_ID  91           // "esp32031" - S,  MAC: FE, even bootCount or all if BROADCAST

/*
receiver.ino
*/
#define VERSION "1.11.b1"


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
// bool mqtt_publish_data();
void mqtt_callback(char* topic, byte* message, unsigned int length);
bool mqtt_publish_button_update_config();

// mqtt_publish_gw_data.h - UPDATE_INTERVAL
bool mqtt_publish_gw_status_config();
bool mqtt_publish_gw_status_values(const char* status);

// #ifdef MOTION_SENSOR_GPIO
//   // motion sensor delay in seconds
//   bool mqtt_publish_number_motion_delay_config();
//   bool mqtt_publish_number_motion_delay_values();
// #endif

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

// various other
void hearbeat();
void ConvertSectoDay(unsigned long n, char * pretty_ontime);
void uptime(char *uptime);
// #ifdef MOTION_SENSOR_GPIO
//   void IRAM_ATTR detected_f();
// #endif

// custom files
#include "config.h"
#include "variables.h"
#include "wifi.h"
#include "espnow.h"
#include "mqtt.h"
#include "mqtt_publish_gw_data.h"
#include "mqtt_publish_sensors_data.h"
#include "fw_update.h"

// #ifdef MOTION_SENSOR_GPIO
//   void IRAM_ATTR motion_detected()
//   {
//     portENTER_CRITICAL_ISR(&motion_mutex);
//     digitalWrite(STATUS_GW_LED_GPIO_RED,HIGH);
//     // start_motion_ms - first time motion detected - set only if no motion before
//     if (!motion) start_motion_ms = millis();
//     // last_motion_ms - last time motion detected - reset on each motion to keep the "Detected" status for motion_delay_s since the last occurence
//     last_motion_ms = millis();
//     motion = true;
//     portEXIT_CRITICAL_ISR(&motion_mutex);
//   }
// #endif
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
  if (debug_mode) Serial.printf("[%s]: pretty_ontime=%s\n",__func__,pretty_ontime);
}


void hearbeat()
{
  bool publish_status = true;
  #ifdef STATUS_GW_LED_GPIO_RED
    digitalWrite(STATUS_GW_LED_GPIO_RED,HIGH);
  #endif
  publish_status = publish_status && mqtt_publish_gw_status_config();
  // #ifdef DEBUG
    Serial.printf("[%s]: updating GW status %ds later...",__func__,((millis()-aux_update_interval)/1000));
  // #endif

  publish_status = publish_status && mqtt_publish_gw_status_values("online");
  publish_status = publish_status && mqtt_publish_button_update_config();
  publish_status = publish_status && mqtt_publish_button_restart_config();
  publish_status = publish_status && mqtt_publish_gw_last_updated_sensor_config();
  publish_status = publish_status && mqtt_publish_switch_publish_values();
  // #ifdef MOTION_SENSOR_GPIO
  //   publish_status = publish_status && mqtt_publish_number_motion_delay_values();
  // #endif

  int queue_count = uxQueueMessagesWaiting(queue);
  if (queue_count == MAX_QUEUE_COUNT)
  {
    publish_status = publish_status && mqtt_publish_gw_last_updated_sensor_values("queue FULL");
  }

  // #ifdef DEBUG
    if (publish_status)
    {
      Serial.printf("SUCCESSFULL\n");
    } else
    {
      Serial.printf("FAILED\n");
    }
  // #endif
  if (queue_count == MAX_QUEUE_COUNT)
  {
    Serial.printf("[%s]: ERROR: queue is full!\n",__func__);
  }

  #ifdef STATUS_GW_LED_GPIO_RED
    digitalWrite(STATUS_GW_LED_GPIO_RED,LOW);
    // #ifdef MOTION_SENSOR_GPIO
    //   // keep ON the led if motion detected
    //   if (motion) digitalWrite(STATUS_GW_LED_GPIO_RED,HIGH);
    // #endif
  #endif




}


void uptime(char *uptime)
{
  byte days = 0;
  byte hours = 0;
  byte minutes = 0;
  byte seconds = 0;

  unsigned long sec2minutes = 60;
  unsigned long sec2hours = (sec2minutes * 60);
  unsigned long sec2days = (sec2hours * 24);

  unsigned long time_delta = (millis()) / 1000UL;

  days = (int)(time_delta / sec2days);
  hours = (int)((time_delta - days * sec2days) / sec2hours);
  minutes = (int)((time_delta - days * sec2days - hours * sec2hours) / sec2minutes);
  seconds = (int)(time_delta - days * sec2days - hours * sec2hours - minutes * sec2minutes);

  if ((minutes == 0) and (hours == 0) and (days == 0)){
    sprintf(uptime, "%01ds", seconds);
  }
  else {
    if ((hours == 0) and (days == 0)){
      sprintf(uptime, "%01dm %01ds", minutes, seconds);
    }
    else {
      if (days == 0){
        sprintf(uptime, "%01dh %01dm", hours, minutes);
      } else {
          sprintf(uptime, "%01dd %01dh %01dm", days, hours, minutes);
      }
    }
  }

}


void setup()
{
  program_start_time = millis();
  Serial.begin(115200);
  delay(100);

  Serial.printf("\n\n[%s]: =============================================================\n",__func__);
  Serial.printf("[%s]: GATEWAY started, DEVICE_ID=%d, version=%s\n",__func__,DEVICE_ID,VERSION);

  #ifdef STATUS_GW_LED_GPIO_RED
    pinMode(STATUS_GW_LED_GPIO_RED, OUTPUT);
  #endif

  #ifdef SENSORS_LED_GPIO_BLUE
    pinMode(SENSORS_LED_GPIO_BLUE, OUTPUT);
  #endif

  #ifdef POWER_ON_LED_GPIO_GREEN
    // PWM
    #if (POWER_ON_LED_USE_PWM == 1)
      ledcSetup(POWER_ON_LED_PWM_CHANNEL, POWER_ON_LED_PWM_FREQ, POWER_ON_LED_PWM_RESOLUTION);
      ledcAttachPin(POWER_ON_LED_GPIO_GREEN, POWER_ON_LED_PWM_CHANNEL);
      // set brightness of GREEN LED (0-255)
      ledcWrite(POWER_ON_LED_PWM_CHANNEL, POWER_ON_LED_DC);
    // or fixed
    #else
      pinMode(POWER_ON_LED_GPIO_GREEN, OUTPUT);
      digitalWrite(POWER_ON_LED_GPIO_GREEN, HIGH);
    #endif
  #endif

  // #ifdef MOTION_SENSOR_GPIO
  //   Serial.printf("[%s]: MOTION_SENSOR_GPIO=%d activated\n",__func__,MOTION_SENSOR_GPIO);
  //   attachInterrupt(digitalPinToInterrupt(MOTION_SENSOR_GPIO), motion_detected, HIGH);
  // #endif

  queue = xQueueCreate( MAX_QUEUE_COUNT, sizeof( struct struct_message ) );
  if(queue == NULL)
  {
    Serial.printf("[%s]: Error creating the queue, restarting in 3s\n",__func__);
    delay(3000);
    ESP.restart();
  }

  queue_aux = xQueueCreate( MAX_QUEUE_COUNT, sizeof( struct struct_message_aux ) );
  if(queue_aux == NULL)
  {
    Serial.printf("[%s]: Error creating the queue_aux, restarting in 3s\n",__func__);
    delay(3000);
    ESP.restart();
  }

  setup_wifi();
  if (!wifi_connected)
  {
    Serial.printf("[%s]: WiFi NOT connected, restarting in 3s\n",__func__);
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
    Serial.printf("[%s]: MQTT NOT connected, restarting in 3s\n",__func__);
    delay(3000);
    ESP.restart();
  }

  Serial.printf("[%s]: configuring GATEWAY status in HA:...",__func__);
  if (mqtt_publish_gw_status_config())
  {
    Serial.printf("done\n");
  } else
  {
    Serial.printf("FAILED\n");
    Serial.printf("[%s]: configuring GATEWAY FAILED, restarting in 3s\n",__func__);
    delay(3000);
    ESP.restart();
  }
  hearbeat();
  espnow_start();
  Serial.printf("[%s]: Setup finished\n",__func__);
}


void loop()
// Serial.printf("[%s]: configuring GATEWAY FAILED, restarting in 3s\n",__func__);
{
  unsigned long start_loop_time = millis();

  if (WiFi.status() != WL_CONNECTED) setup_wifi();
  if (!mqttc.connected()) mqtt_reconnect();
  mqttc.loop();
  do_update();

  // hearbeat
  if (start_loop_time > (aux_update_interval + UPDATE_INTERVAL))
  {
    hearbeat();
    aux_update_interval = start_loop_time;
  }

  // // motion
  // #ifdef MOTION_SENSOR_GPIO
  //   unsigned long now = millis();
  //   if (motion)
  //   {
  //     if (!motion_printed)
  //     {
  //       mqtt_publish_gw_status_values("online");
  //       Serial.printf("[%s]: motion DETECTED at: %lus\n",__func__,(now/1000));
  //       motion_printed = true;
  //     }
  //     // keep motion "Detected" for entire motion_delay_s time since the last detection (not the first detection)
  //     // last_motion_ms + (motion_delay_s * 1000)
  //     if ((now - last_motion_ms) > (motion_delay_s * 1000))
  //     {
  //       Serial.printf("[%s]: motion stopped at %lus, after %lus of total active time, (%lus after last occurence).\n",__func__,(now/1000), ((now - start_motion_ms)/1000),((now - last_motion_ms)/1000));
  //       portENTER_CRITICAL(&motion_mutex);
  //       motion = false;
  //       portEXIT_CRITICAL(&motion_mutex);
  //       digitalWrite(STATUS_GW_LED_GPIO_RED,LOW);
  //       motion_printed = false;
  //       mqtt_publish_gw_status_values("online");
  //     }
  //   }
  // #endif

  int queue_count = uxQueueMessagesWaiting(queue);
  if (queue_count > 0)
  {
    // if (debug_mode) Serial.printf("queue size=%d\n",queue_count);
    if (publish_sensors_to_ha)
    {
      mqtt_publish_sensors_values();
    }
  }
}
