#pragma once
/*
mqtt functions
*/

#include "config.h"
#include "variables.h"

// mqtt common functions

// mqtt reconnect
void mqtt_reconnect()
{
  if (mqttc.connected())
  {
    Serial.printf("[%s]: MQTT connected\n",__func__);
    mqtt_connected = true;
    return;
  }

  long sm = millis();
  int mqtt_error = 0;
  mqtt_connected=false;
  if (!wifi_connected) {
    Serial.printf("[%s]: WiFi NOT connected\n",__func__);
    return;
  }

  while (!mqttc.connected()) {
    ++mqtt_error;
    Serial.printf("[%s]: Attempting MQTT connection for %d time\n",__func__,mqtt_error);
    long sm1=millis(); while(millis() < sm1 + 1000) {}

    if (mqttc.connect(HOSTNAME,HA_MQTT_USER, HA_MQTT_PASSWORD))
    {
      mqtt_connected = true;
      Serial.printf("[%s]: MQTT connected\n",__func__);
    } else
    {
      if (mqtt_error >= MAX_MQTT_ERROR)
      {
        Serial.printf("[%s]: FATAL MQTT ERROR !!!!!!!!\n",__func__);
        Serial.println("restart ESP in 3s\n\n");
        delay(3000);
        ESP.restart();
      }
    }
  }

  // restart
  char restart_cmd_topic[60];
  snprintf(restart_cmd_topic,sizeof(restart_cmd_topic),"%s/cmd/restart",HOSTNAME);
  mqttc.subscribe(restart_cmd_topic);

  // update
  char update_cmd_topic[60];
  snprintf(update_cmd_topic,sizeof(update_cmd_topic),"%s/cmd/update",HOSTNAME);
  mqttc.subscribe(update_cmd_topic);

  // publish
  char publish_cmd_topic[30];
  snprintf(publish_cmd_topic,sizeof(publish_cmd_topic),"%s/cmd/publish",HOSTNAME);
  mqttc.subscribe(publish_cmd_topic);

  #ifdef MOTION_SENSOR_GPIO
    // motion delay seconds
    char motion_delay_cmd_topic[30];
    snprintf(motion_delay_cmd_topic,sizeof(motion_delay_cmd_topic),"%s/cmd/motion_delay",HOSTNAME);
    mqttc.subscribe(motion_delay_cmd_topic);
  #endif

  Serial.printf("[%s]: GW subscribed to:\n",__func__);
  {
    Serial.printf("\t%s\n",restart_cmd_topic);
    Serial.printf("\t%s\n",update_cmd_topic);
    Serial.printf("\t%s\n",publish_cmd_topic);
    #ifdef MOTION_SENSOR_GPIO
      Serial.printf("\t%s\n",motion_delay_cmd_topic);
    #endif
  }

  long em = millis();
  long mt=em-sm;
  Serial.printf("[%s]: MQTT RECONNECT TIME: %dms\n",__func__,mt);
}
// mqtt reconnect END


// mqtt callback
void mqtt_callback(char* topic, byte* message, unsigned int length)
{

  long sm = micros();

  if ((!wifi_connected) or (!mqtt_connected)) {
    // if (debug_mode)
    // Serial.println("mqtt_callback: WiFi or MQTT not connected");
    Serial.printf("[%s]: WiFi or MQTT not connected, leaving...\n",__func__);
    return;
  }
  bool publish_status = true;

  char restart_cmd_topic[30];
  snprintf(restart_cmd_topic,sizeof(restart_cmd_topic),"%s/cmd/restart",HOSTNAME);

  char update_cmd_topic[60];
  snprintf(update_cmd_topic,sizeof(update_cmd_topic),"%s/cmd/update",HOSTNAME);

  char publish_cmd_topic[30];
  snprintf(publish_cmd_topic,sizeof(publish_cmd_topic),"%s/cmd/publish",HOSTNAME);

  #ifdef MOTION_SENSOR_GPIO
    char motion_delay_cmd_topic[30];
    snprintf(motion_delay_cmd_topic,sizeof(motion_delay_cmd_topic),"%s/cmd/motion_delay",HOSTNAME);
  #endif

  String messageTemp;
  for (int i = 0; i < length; i++)
  {
    messageTemp += (char)toupper(message[i]);
  }
  //
  // if (debug_mode){
    Serial.printf("\n[%s]: MQTT message received:\n\t%s\n\tmessage:\t%s\n",__func__,topic,messageTemp);
  // }

  String topic_str = String(topic);

  //restart
  if (String(topic) == restart_cmd_topic)
  {
    if ((messageTemp == "ON") or (messageTemp == "1"))
    {
      const char* restart_char="";  //cleared   // clean restart command before actual restart to avoid the restart loop
      if (mqtt_connected){ if (!mqttc.publish(restart_cmd_topic,(uint8_t*)restart_char,strlen(restart_char), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}
      Serial.flush();
      long sm2 = millis(); while(millis() < sm2 + 100) {};
      if (publish_status)
      {
        Serial.printf("\n\n[%s]: RESTARTING on MQTT message\n\n\n",__func__);
        ESP.restart();
      }
    }
  }

  else
  //update
  if (String(topic) == update_cmd_topic)
  {
    if ((messageTemp == "ON") or (messageTemp == "1"))
    {
      const char* update_char="";   //cleared  // clean update
      if (mqtt_connected){ if (!mqttc.publish(update_cmd_topic,(uint8_t*)update_char,strlen(update_char), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}
      long sm2 = millis(); while(millis() < sm2 + 100) {};
      if (publish_status)
      {
        Serial.printf("\n\n[%s]: UPDATING FIRMWARE on MQTT message\n\n\n",__func__);
        perform_update_firmware=true;
      }
    }
  }

  else
  //publish
  if (String(topic) == publish_cmd_topic)
  {
    bool old_publish_sensors_to_ha = publish_sensors_to_ha;
    if ((messageTemp == "ON") or (messageTemp == "1"))
    {
      publish_sensors_to_ha = true;
    }
    else
    {
      if ((messageTemp == "OFF") or (messageTemp == "0"))
      {
        publish_sensors_to_ha = false;
      }
    }
    if (publish_sensors_to_ha != old_publish_sensors_to_ha)
    {
      mqtt_publish_switch_publish_values();
      Serial.printf("[%s]: switch [publish] changed on MQTT message from: %d to %d\n",__func__,old_publish_sensors_to_ha,publish_sensors_to_ha);
    } else
    {
      Serial.printf("[%s]: switch publish NOT changed\n",__func__);
    }
  }

  #ifdef MOTION_SENSOR_GPIO
    else
    //motion delay
    if (String(topic) == motion_delay_cmd_topic)
    {
      int old_motion_delay_s = motion_delay_s;
      motion_delay_s = messageTemp.toInt();
      if (motion_delay_s != old_motion_delay_s)
      {
        if ((motion_delay_s < MIN_MOTION_DELAY_S) or (motion_delay_s > MAX_MOTION_DELAY_S))
        {
          motion_delay_s = old_motion_delay_s;
          Serial.printf("[%s]: motion delay beyond limits - NOT changed\n",__func__);
        } else
        {
          mqtt_publish_number_motion_delay_values();
          {
            // Serial.println("motion delay changed from: " + String(old_motion_delay_s) +" to: " + String(motion_delay_s));
            Serial.printf("[%s]: motion delay changed from: %ds to: %ds\n",__func__,old_motion_delay_s,motion_delay_s);
          }
        }
      } else
      {
        Serial.printf("[%s]: motion delay the same - NOT changed\n",__func__);
      }
    }
  #endif

  // if (debug_mode){
    long em = micros(); //END measurement time
    long mt=em-sm;
    // Serial.println("mqtt_callback MEASUREMENT TIME:"+String(mt)+"us");
    Serial.printf("[%s]: MQTT CALLBACK TIME: %dus\n",__func__,mt);
  // }

}
// mqtt callback END
