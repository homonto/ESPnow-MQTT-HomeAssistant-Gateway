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

  WiFi.disconnect();
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.config(ipaddress, gateway, subnet, primarydns);
  WiFi.setHostname(HOSTNAME);
  Serial.print(" OLD MAC: "); Serial.println(WiFi.macAddress());

  //change MAC address to fixed one - currently as lilygo3
  esp_wifi_set_mac(WIFI_IF_STA, &FixedMACAddress[0]);
  /* try also this method:
  uint8_t mac[] = {0x82, 0x88, 0x88, 0x88, 0x88, 0x88};
  WiFi.mode(WIFI_AP);
  wifi_set_macaddr(SOFTAP_IF, &mac[0]);
  WiFi.disconnect();
  */


  WiFi.begin(BT_SSID, BT_PASSWORD,WIFI_CHANNEL);
  Serial.printf("Connecting to %s ...", BT_SSID);

  while (WiFi.status() != WL_CONNECTED)
  {
    ttc = millis() - wifi_start_time;
    sm1=millis(); while(millis() < sm1 + 20) {}
    if (ttc > (2 * 1000)) {
      Serial.println("still NOT connected after " + String(ttc)+"ms");
      wifi_connected =  false;
      mqtt_connected = false;
      return;
    }
  }
  Serial.println("connected after " + String(ttc)+"ms");

  Serial.print("Channel: "); Serial.println(WiFi.channel());
  Serial.print("IP: "); Serial.println(WiFi.localIP());
  Serial.print(" NEW MAC: "); Serial.println(WiFi.macAddress());
  #define MQTT_DEVICE_IDENTIFIER String(WiFi.macAddress())
  wifi_connected =  true;
}
