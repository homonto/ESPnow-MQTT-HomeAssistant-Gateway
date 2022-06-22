#pragma once
/*
mqtt functions for sensors
*/

#include "config.h"
#include "variables.h"

// sensor device definition
  #define CREATE_SENSOR_MQTT_DEVICE \
  dev = config.createNestedObject("device"); \
  dev["ids"]=macStr;  \
  dev["name"]="ESPnow_" + String(myData.name) + "_" + String(myData.host); \
  dev["mdl"]="ESPnow sensor"; \
  dev["mf"]="ZH"; \
  dev["sw"]= myData.ver;
// sensor device definition END

bool mqtt_publish_sensors_config(const char* hostname)
{
// config topics
  char temp_conf_topic[60];
  snprintf(temp_conf_topic,sizeof(temp_conf_topic),"homeassistant/sensor/%s/temperature/config",hostname);
  if (debug_mode) Serial.println("temp_conf_topic="+String(temp_conf_topic));

  char hum_conf_topic[60];
  snprintf(hum_conf_topic,sizeof(hum_conf_topic),"homeassistant/sensor/%s/humidity/config",hostname);
  if (debug_mode) Serial.println("hum_conf_topic="+String(hum_conf_topic));

  char lux_conf_topic[60];
  snprintf(lux_conf_topic,sizeof(lux_conf_topic),"homeassistant/sensor/%s/light/config",hostname);
  if (debug_mode) Serial.println("lux_conf_topic="+String(lux_conf_topic));

  char bat_conf_topic[60];
  snprintf(bat_conf_topic,sizeof(bat_conf_topic),"homeassistant/sensor/%s/battery/config",hostname);
  if (debug_mode) Serial.println("bat_conf_topic="+String(bat_conf_topic));

  char batpct_conf_topic[60];
  snprintf(batpct_conf_topic,sizeof(batpct_conf_topic),"homeassistant/sensor/%s/batterypercent/config",hostname);
  if (debug_mode) Serial.println("batpct_conf_topic="+String(batpct_conf_topic));

  char rssi_conf_topic[60];
  snprintf(rssi_conf_topic,sizeof(rssi_conf_topic),"homeassistant/sensor/%s/rssi/config",hostname);
  if (debug_mode) Serial.println("rssi_conf_topic="+String(rssi_conf_topic));

  char version_conf_topic[60];
  snprintf(version_conf_topic,sizeof(version_conf_topic),"homeassistant/sensor/%s/version/config",hostname);
  if (debug_mode) Serial.println("version_conf_topic="+String(version_conf_topic));

  char charging_conf_topic[60];
  snprintf(charging_conf_topic,sizeof(charging_conf_topic),"homeassistant/sensor/%s/charging/config",hostname);
  if (debug_mode) Serial.println("charging_conf_topic="+String(charging_conf_topic));

  char name_conf_topic[60];
  snprintf(name_conf_topic,sizeof(name_conf_topic),"homeassistant/sensor/%s/name/config",hostname);
  if (debug_mode) Serial.println("name_conf_topic="+String(name_conf_topic));

  char boot_conf_topic[60];
  snprintf(boot_conf_topic,sizeof(boot_conf_topic),"homeassistant/sensor/%s/boot/config",hostname);
  if (debug_mode) Serial.println("boot_conf_topic="+String(boot_conf_topic));
  
  char ontime_conf_topic[60];
  snprintf(ontime_conf_topic,sizeof(ontime_conf_topic),"homeassistant/sensor/%s/ontime/config",hostname);
  if (debug_mode) Serial.println("ontime_conf_topic="+String(ontime_conf_topic));

// sensors/entities names
  char temp_name[30];
  snprintf(temp_name,sizeof(temp_name),"%s_temperature",hostname);
  if (debug_mode) Serial.println("temp_name="+String(temp_name));

  char hum_name[30];
  snprintf(hum_name,sizeof(hum_name),"%s_humidity",hostname);
  if (debug_mode) Serial.println("hum_name="+String(hum_name));

  char lux_name[30];
  snprintf(lux_name,sizeof(lux_name),"%s_lux",hostname);
  if (debug_mode) Serial.println("lux_name="+String(lux_name));

  char bat_name[30];
  snprintf(bat_name,sizeof(bat_name),"%s_battery",hostname);
  if (debug_mode) Serial.println("bat_name="+String(bat_name));

  char batpct_name[30];
  snprintf(batpct_name,sizeof(batpct_name),"%s_batterypercent",hostname);
  if (debug_mode) Serial.println("batpct_name="+String(batpct_name));

  char rssi_name[30];
  snprintf(rssi_name,sizeof(rssi_name),"%s_rssi",hostname);
  if (debug_mode) Serial.println("rssi_name="+String(rssi_name));

  char version_name[30];
  snprintf(version_name,sizeof(version_name),"%s_version",hostname);
  if (debug_mode) Serial.println("version_name="+String(version_name));

  char charging_name[30];
  snprintf(charging_name,sizeof(charging_name),"%s_charging",hostname);
  if (debug_mode) Serial.println("charging_name="+String(charging_name));

  char name_name[30];
  snprintf(name_name,sizeof(name_name),"%s_name",hostname);
  if (debug_mode) Serial.println("name_name="+String(name_name));

  char boot_name[30];
  snprintf(boot_name,sizeof(boot_name),"%s_boot",hostname);
  if (debug_mode) Serial.println("boot_name="+String(boot_name));
  
  char ontime_name[30];
  snprintf(ontime_name,sizeof(ontime_name),"%s_ontime",hostname);
  if (debug_mode) Serial.println("ontime_name="+String(ontime_name));

// values/state topic
  char sensors_topic_state[60];
  snprintf(sensors_topic_state,sizeof(sensors_topic_state),"%s/sensor/state",hostname);
  if (debug_mode) Serial.println("sensors_topic_state="+String(sensors_topic_state));

  bool publish_status = true;
  StaticJsonDocument<JSON_CONFIG_SIZE> config;
  JsonObject dev;
  int size_c;
  char config_json[JSON_CONFIG_SIZE];

// temperature config
  config.clear();
  config["name"] = temp_name;
  config["dev_cla"] = "temperature";
  config["stat_cla"] = "measurement";
  config["stat_t"] = sensors_topic_state;
  config["unit_of_meas"] = "°C";
  config["val_tpl"] = "{{value_json.temperature}}";
  config["uniq_id"] = temp_name;
  config["frc_upd"] = "true";
  config["exp_aft"] = 600;

  CREATE_SENSOR_MQTT_DEVICE

  size_c = serializeJson(config, config_json);
  if (mqtt_connected){ if (!mqttc.publish(temp_conf_topic,(uint8_t*)config_json,strlen(config_json), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}
  if (debug_mode) {
    Serial.println("\n============ DEBUG CONFIG TEMP ============");
    Serial.println("Size of temperature config_json="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty(config, Serial)");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n TEMP CONFIG OK");
    } else
    {
      Serial.println("\n TEMP CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG CONFIG TEMP END ========\n");
  }

// humidity config
  config.clear();
  config["name"] = hum_name;
  config["dev_cla"] = "humidity";
  config["stat_cla"] = "measurement";
  config["stat_t"] = sensors_topic_state;
  config["unit_of_meas"] = "%H";
  config["val_tpl"] = "{{value_json.humidity}}";
  config["uniq_id"] = hum_name;
  config["frc_upd"] = "true";
  config["exp_aft"] = 600;

  CREATE_SENSOR_MQTT_DEVICE

  size_c = serializeJson(config, config_json);
  if (mqtt_connected){ if (!mqttc.publish(hum_conf_topic,(uint8_t*)config_json,strlen(config_json), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}
  if (debug_mode) {
    Serial.println("\n============ DEBUG CONFIG HUM ============");
    Serial.println("Size of humidity config="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty(config, Serial)");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n HUM CONFIG OK");
    } else
    {
      Serial.println("\n HUM CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG CONFIG HUM END ========\n");
  }

// lux config
  config.clear();
  config["name"]=lux_name;
  config["dev_cla"] = "illuminance";
  config["stat_cla"] = "measurement";
  config["stat_t"]=sensors_topic_state;
  config["unique_id"]=lux_name;
  config["unit_of_meas"] = "lx";
  config["val_tpl"] = "{{value_json.lux}}";
  config["frc_upd"] = "true";
  config["exp_aft"] = 600;

  CREATE_SENSOR_MQTT_DEVICE

  size_c = serializeJson(config, config_json);
  if (mqtt_connected){ if (!mqttc.publish(lux_conf_topic,(uint8_t*)config_json,strlen(config_json), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}

  if (debug_mode) {
    Serial.println("\n============ DEBUG CONFIG LUX ============");
    Serial.println("Size of lux config_json="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty(config, Serial)");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n LUX CONFIG OK");
    } else
    {
      Serial.println("\n LUX CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG CONFIG LUX END ========\n");
  }

// battery volts config
  config.clear();
  config["name"] = bat_name;
  config["dev_cla"] = "voltage";
  config["stat_cla"] = "measurement";
  config["stat_t"] = sensors_topic_state;
  config["unit_of_meas"] = "V";
  config["val_tpl"] = "{{value_json.battery}}";
  config["uniq_id"] = bat_name;
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  config["exp_aft"] = 600;

  CREATE_SENSOR_MQTT_DEVICE

  size_c = serializeJson(config, config_json);
  if (mqtt_connected){ if (!mqttc.publish(bat_conf_topic,(uint8_t*)config_json,strlen(config_json), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}
  if (debug_mode) {
    Serial.println("\n============ DEBUG CONFIG BATTERY ============");
    Serial.println("Size of battery config="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n BATTERY CONFIG OK");
    } else
    {
      Serial.println("\n BATTERY CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG CONFIG BATTERY END ========\n");
  }

// batterypercent config
  config.clear();
  config["name"] = batpct_name;
  config["dev_cla"] = "battery";
  config["stat_cla"] = "measurement";
  config["stat_t"] = sensors_topic_state;
  config["unit_of_meas"] = "%";
  config["val_tpl"] = "{{value_json.batterypercent}}";
  config["uniq_id"] = batpct_name;
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  config["exp_aft"] = 600;

  CREATE_SENSOR_MQTT_DEVICE

  size_c = serializeJson(config, config_json);
  if (mqtt_connected){ if (!mqttc.publish(batpct_conf_topic,(uint8_t*)config_json,strlen(config_json), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}
  if (debug_mode) {
    Serial.println("\n============ DEBUG CONFIG BATTERY PERCENT============");
    Serial.println("Size of battery percent config="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n BATTERY PERCENT CONFIG OK");
    } else
    {
      Serial.println("\n BATTERY PERCENT CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG CONFIG BATTERY PERCENT END ========\n");
  }

// rssi config
  config.clear();
  config["name"] = rssi_name;
  config["dev_cla"] = "signal_strength";
  config["stat_cla"] = "measurement";
  config["stat_t"] = sensors_topic_state;
  config["unit_of_meas"] = "dBm";
  config["val_tpl"] = "{{value_json.rssi}}";
  config["uniq_id"] = rssi_name;
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  config["exp_aft"] = 600;

  CREATE_SENSOR_MQTT_DEVICE

  size_c = serializeJson(config, config_json);
  if (mqtt_connected){ if (!mqttc.publish(rssi_conf_topic,(uint8_t*)config_json,strlen(config_json), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}
  if (debug_mode) {
    Serial.println("\n============ DEBUG CONFIG RSSI ============");
    Serial.println("Size of rssi config="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n RSSI CONFIG OK");
    } else
    {
      Serial.println("\n RSSI CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG CONFIG RSSI END ========\n");
  }

// version config
  config.clear();
  config["name"] = version_name;
  config["stat_t"] = sensors_topic_state;
  config["val_tpl"] = "{{value_json.version}}";
  config["uniq_id"] = version_name;
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  config["exp_aft"] = 600;

  CREATE_SENSOR_MQTT_DEVICE

  size_c = serializeJson(config, config_json);
  if (mqtt_connected){ if (!mqttc.publish(version_conf_topic,(uint8_t*)config_json,strlen(config_json), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}
  if (debug_mode) {
    Serial.println("\n============ DEBUG CONFIG VERSION ============");
    Serial.println("Size of version config="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n VERSION CONFIG OK");
    } else
    {
      Serial.println("\n VERSION CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG CONFIG VERSION END ========\n");
  }

// charging config
  config.clear();
  config["name"] = charging_name;
  config["stat_t"] = sensors_topic_state;
  config["val_tpl"] = "{{value_json.charging}}";
  config["uniq_id"] = charging_name;
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  config["exp_aft"] = 600;

  CREATE_SENSOR_MQTT_DEVICE

  size_c = serializeJson(config, config_json);
  if (mqtt_connected){ if (!mqttc.publish(charging_conf_topic,(uint8_t*)config_json,strlen(config_json), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}
  if (debug_mode) {
    Serial.println("\n============ DEBUG CONFIG CHARGING ============");
    Serial.println("Size of charging config="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n CHARGING CONFIG OK");
    } else
    {
      Serial.println("\n CHARGING CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG CONFIG CHARGING END ========\n");
  }

// sensor name config
  config.clear();
  config["name"] = name_name;
  config["stat_t"] = sensors_topic_state;
  config["val_tpl"] = "{{value_json.name}}";
  config["uniq_id"] = name_name;
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  config["exp_aft"] = 600;

  CREATE_SENSOR_MQTT_DEVICE

  size_c = serializeJson(config, config_json);
  if (mqtt_connected){ if (!mqttc.publish(name_conf_topic,(uint8_t*)config_json,strlen(config_json), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}
  if (debug_mode) {
    Serial.println("\n============ DEBUG CONFIG NAME ============");
    Serial.println("Size of name config="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n NAME CONFIG OK");
    } else
    {
      Serial.println("\n NAME CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG CONFIG NAME END ========\n");
  }


// sensor bootCount/boot config
  config.clear();
  config["name"] = boot_name;
  config["stat_t"] = sensors_topic_state;
  config["val_tpl"] = "{{value_json.boot}}";
  config["uniq_id"] = boot_name;
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  config["exp_aft"] = 600;

  CREATE_SENSOR_MQTT_DEVICE

  size_c = serializeJson(config, config_json);
  if (mqtt_connected){ if (!mqttc.publish(boot_conf_topic,(uint8_t*)config_json,strlen(config_json), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}
  if (debug_mode) {
    Serial.println("\n============ DEBUG CONFIG BOOT ============");
    Serial.println("Size of boot config="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n BOOT CONFIG OK");
    } else
    {
      Serial.println("\n BOOT CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG CONFIG BOOT END ========\n");
  }
  
// sensor ontime config
  config.clear();
  config["name"] = ontime_name;
  config["stat_t"] = sensors_topic_state;
  config["val_tpl"] = "{{value_json.ontime}}";
  config["uniq_id"] = ontime_name;
  config["unit_of_meas"] = "s";
  config["frc_upd"] = "true";
  config["entity_category"] = "diagnostic";
  config["exp_aft"] = 600;

  CREATE_SENSOR_MQTT_DEVICE

  size_c = serializeJson(config, config_json);
  if (mqtt_connected){ if (!mqttc.publish(ontime_conf_topic,(uint8_t*)config_json,strlen(config_json), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}
  if (debug_mode) {
    Serial.println("\n============ DEBUG CONFIG ONTIME ============");
    Serial.println("Size of ontime config="+String(size_c)+" bytes");
    Serial.println("Serialised config_json:");
    Serial.println(config_json);
    Serial.println("serializeJsonPretty");
    serializeJsonPretty(config, Serial);
    if (publish_status) {
      Serial.println("\n ONTIME CONFIG OK");
    } else
    {
      Serial.println("\n ONTIME CONFIG UNSUCCESSFULL");
    }
    Serial.println("============ DEBUG CONFIG ONTIME END ========\n");
  }
  
  return publish_status;
}


bool mqtt_publish_sensors_values(const char* hostname)
{
// values topic
  char sensors_topic_state[60];
  snprintf(sensors_topic_state,sizeof(sensors_topic_state),"%s/sensor/state",hostname);
  if (debug_mode)Serial.println("sensors_topic_state="+String(sensors_topic_state));

  bool publish_status = true;
  if (!mqtt_publish_sensors_config(hostname)){
    Serial.println("\n SENSORS CONFIG NOT published");
    return false;
  }

  StaticJsonDocument<JSON_PAYLOAD_SIZE> payload;

  payload["temperature"]        = myData.temp;
  payload["humidity"]           = myData.hum;
  payload["lux"]                = myData.lux;
  payload["battery"]            = myData.bat;
  payload["batterypercent"]     = myData.batpct;
  payload["rssi"]               = rssi;
  payload["version"]            = myData.ver;
  payload["charging"]           = myData.charg;
  payload["name"]               = myData.name;
  payload["boot"]               = myData.boot;
  payload["ontime"]             = myData.ontime;

  char payload_json[JSON_PAYLOAD_SIZE];
  int size_pl = serializeJson(payload, payload_json);

  if (mqtt_connected){ if (!mqttc.publish(sensors_topic_state,(uint8_t*)payload_json,strlen(payload_json), true)) { publish_status = false; Serial.println("PUBLISH FAILED");}}

  if (debug_mode) {
    Serial.println("\n============ DEBUG PAYLOAD SENSORS============");
    Serial.println("Size of sensors payload="+String(size_pl)+" bytes");
    Serial.println("Serialised payload_json:");
    Serial.println(payload_json);
    Serial.println("serializeJsonPretty");
    serializeJsonPretty(payload, Serial);
    if (publish_status) {
      Serial.println("\n SENSORS VALUES published OK");
    } else
    {
      Serial.println("\n SENSORS VALUES NOT published");
    }
    Serial.println("============ DEBUG PAYLOAD SENSORS END ========\n");
  }
  if (!publish_status) {Serial.println("\n SENSORS VALUES NOT published");}

  return publish_status;
}
