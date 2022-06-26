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
    Serial.println("MQTT connected");
    mqtt_connected = true;
    return;
  }

  long sm = millis();
  int mqtt_error = 0;
  mqtt_connected=false;
  if (!wifi_connected) {
    Serial.println("mqtt_callback: WiFi not connected");
    Serial.print("mqtt_connected=");Serial.println(mqtt_connected);
    return;
  }

  while (!mqttc.connected()) {
    ++mqtt_error;
    Serial.println("Attempting MQTT connection for " + String(mqtt_error)+" time");
    long sm1=millis(); while(millis() < sm1 + 1000) {}

    if (mqttc.connect(HOSTNAME,HA_MQTT_USER, HA_MQTT_PASSWORD))
    {
      mqtt_connected = true;
      Serial.println("MQTT connected");
    } else
    {
      if (mqtt_error >= MAX_MQTT_ERROR)
      {
        Serial.println("FATAL MQTT ERROR !!!!!!!!");
        Serial.println("restart ESP in 3s");
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

  Serial.println("[mqtt_reconnect] GW subscribed to:");
  {
    Serial.print(" ");Serial.println(restart_cmd_topic);
    Serial.print(" ");Serial.println(update_cmd_topic);
    Serial.print(" ");Serial.println(publish_cmd_topic);
  }

  long em = millis();
  long mt=em-sm;
  Serial.println("MQTT RECONNECT TIME:"+String(mt)+"ms");
}
// mqtt reconnect END


// mqtt callback
void mqtt_callback(char* topic, byte* message, unsigned int length)
{

  long sm = micros();

  if ((!wifi_connected) or (!mqtt_connected)) {
    if (debug_mode)
      Serial.println("mqtt_callback: WiFi or MQTT not connected");
    return;
  }
  bool publish_status = true;

  char restart_cmd_topic[30];
  snprintf(restart_cmd_topic,sizeof(restart_cmd_topic),"%s/cmd/restart",HOSTNAME);

  char update_cmd_topic[60];
  snprintf(update_cmd_topic,sizeof(update_cmd_topic),"%s/cmd/update",HOSTNAME);

  char publish_cmd_topic[30];
  snprintf(publish_cmd_topic,sizeof(publish_cmd_topic),"%s/cmd/publish",HOSTNAME);


  String messageTemp;
  for (int i = 0; i < length; i++)
  {
    messageTemp += (char)toupper(message[i]);
  }
  //
  // if (debug_mode){
    Serial.println("\nMQTT message received:\n\ttopic:\t" +String(topic)+"\n\tmessage:\t"+String(messageTemp));
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
        Serial.println("\n\nRESTARTING on MQTT message\n\n");
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
        Serial.println("UPDATING FIRMWARE on MQTT message");
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
      Serial.println("switch publish changed");
    } else
    {
      Serial.println("switch publish NOT changed");
    }
  }

  if (debug_mode){
    long em = micros(); //END measurement time
    long mt=em-sm;
    Serial.println("mqtt_callback MEASUREMENT TIME:"+String(mt)+"us");
  }

}
// mqtt callback END
