#pragma once
/*
espnow functions
*/

#include "config.h"
#include "variables.h"


// rssi thingis - to see the rssi of the sender - gateway measures it
typedef struct {
  unsigned frame_ctrl: 16;
  unsigned duration_id: 16;
  uint8_t addr1[6]; /* receiver address */
  uint8_t addr2[6]; /* sender address */
  uint8_t addr3[6]; /* filtering address */
  unsigned sequence_ctrl: 16;
  uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct {
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;


void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type) {
  // All espnow traffic uses action frames which are a subtype of the mgmnt frames so filter out everything else.
  if (type != WIFI_PKT_MGMT)
    return;
  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

  rssi = ppkt->rx_ctrl.rssi;
}
// rssi thingis END


void espnow_start()
{
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESPNow NOT initialized, REBOTTING in 3s");
    delay(3000);
    ESP.restart();
  }

  Serial.println("ESPNow started - waiting for messages...");
  esp_now_register_recv_cb(OnDataRecv);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);
}


// OnDataRecv callback
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
  // data_to_send has to be protected while being changed by this function and read during publish to HA
  portENTER_CRITICAL(&receive_cb_mutex);
  data_to_send = true;
  memcpy(&myData, incomingData, sizeof(myData));
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  portEXIT_CRITICAL(&receive_cb_mutex);
}
// OnDataRecv callback END
