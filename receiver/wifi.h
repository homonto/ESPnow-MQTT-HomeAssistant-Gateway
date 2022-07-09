#pragma once
/*
wifi functions
*/
#include "config.h"
#include "variables.h"


// functions
void stop_wifi()
{
  WiFi.disconnect();
  delay(100);
}

void setup_wifi()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    wifi_connected =  true;
    return;
  }

  long ttc, sm1;
  long wifi_start_time = millis();
  IPAddress ipaddress;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress primarydns;

  ipaddress.fromString(IP_ADDRESS);
  gateway.fromString(IP_GATEWAY);
  subnet.fromString(IP_SUBNET);
  primarydns.fromString(IP_DNS);

  Serial.printf("[%s]:\n",__func__);
  WiFi.disconnect();
  WiFi.mode(WIFI_MODE_STA);
  // check if WIFI_PROTOCOL_LR works
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR);
  // check if WIFI_PROTOCOL_LR works END

  WiFi.config(ipaddress, gateway, subnet, primarydns);
  WiFi.setHostname(HOSTNAME);
  Serial.print("\tOLD MAC: "); Serial.println(WiFi.macAddress());

  //change MAC address to fixed one - needed only in case UNICAST is used plus for router/AP MAC filtering
  esp_wifi_set_mac(WIFI_IF_STA, &FixedMACAddress[0]);

  WiFi.begin(BT_SSID, BT_PASSWORD,WIFI_CHANNEL);
  Serial.printf("\tConnecting to %s ...", BT_SSID);

  while (WiFi.status() != WL_CONNECTED)
  {
    ttc = millis() - wifi_start_time;
    sm1=millis(); while(millis() < sm1 + 20) {}
    if (ttc > (WAIT_FOR_WIFI * 1000)) {
      Serial.printf("NOT connected after %dms\n",ttc);
      // wifi_connected =  false;
      // mqtt_connected = false;
      // return;
      Serial.printf("[%s]: FATAL WiFi ERROR !!!!!!!!\n",__func__);
      Serial.println("restart ESP in 3s\n\n");
      delay(3000);
      ESP.restart();
    }
  }
  Serial.printf("CONNECTED after %dms\n",ttc);

  Serial.print("\tChannel: "); Serial.println(WiFi.channel());
  Serial.print("\tIP: "); Serial.println(WiFi.localIP());
  Serial.print("\tNEW MAC: "); Serial.println(WiFi.macAddress());
  #define MQTT_DEVICE_IDENTIFIER String(WiFi.macAddress())
  wifi_connected =  true;
  Serial.printf("[%s]: SUCCESSFULL\n",__func__);
}
