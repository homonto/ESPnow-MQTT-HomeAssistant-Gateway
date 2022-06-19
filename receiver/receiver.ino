/* flow:
1- start wifi (on gateway towards AP) and espnow (on gateway and sensor) - both configured accordingly - DONE
2- when receive a message from new device - send the data to HA - DONE
  - each sensor on its own device - DONE
  - DONE:
    - UPDATE_INTERVAL = 180s
    - battery, batterypercent (MAX17048), temperature, humidity (SHT31), lux (TSL2561), lux ADC (used with ENABLE_3V_GPIO)
    - sensor's rssi (from gateway) - DONE
    - gatewa rss - not yet
    - optional: if BLINK_LED_GPIO defined, this LED blinks when sensor is working - DONE
    - charging status (but connecting TP4056 adds 2.5uA) - check how long battery last, maybe better to change battery?
    - IDF variable with ESP model: - DONE
        CONFIG_IDF_TARGET_ESP32
        CONFIG_IDF_TARGET_ESP32S2
  - TODO:
    - sensor's/gateway MAC publish to HA - but is it needed?
    - since no OTA on sensor, add connector with RST and DTR - which one? 5 pins: RX, TX, GND, RST, DTR + Vcc (so 6 pins)
  - DON'T DO:
    - battery via ADC - too much leaking
    - lux via TEPT - too much leaking - DONE with ENABLE_3V_GPIO
3- send heartbeat on gateway topic - DONE
4- OTA for gateway
5- UPDATE_FW for gateway from web server
6- reconfigure gateway from HA
7- reconfigure sensors from HA - how?
8- OTA for sensor - how? DONE periodic and DRD (double reset detector)
9- UPDATE_FW for sensor from web server - how?
10- more sensors/info from gateway
  - MAC
  - battery/percent
  - charging
  - rssi
  - ntp
  - last update (time) both gw and sensors
*/


#define VERSION "1.4.1"
/*
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

// various, here - UPDATE_INTERVAL
void hearbeat();


// custom files
#include "config.h"
#include "variables.h"
#include "wifi.h"
#include "espnow.h"
#include "mqtt.h"
#include "mqtt_publish_gw_data.h"
#include "mqtt_publish_sensors_data.h"


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
