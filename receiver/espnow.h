#pragma once
/*
espnow functions
*/

#include "config.h"
#include "variables.h"


// rssi thingis - to see the rssi of the sender - gateway measures always the last rssi: being from GW to AP or GW to sender
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
  // const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  // const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

  // push rssi into myData_aux.rssi
  myData_aux.rssi = ppkt->rx_ctrl.rssi;
  if (debug_mode)
    Serial.printf("-->[%s]: RSSI: %ddBm\n",__func__,ppkt->rx_ctrl.rssi);
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
  int queue_count = uxQueueMessagesWaiting(queue);
  int queue_aux_count = uxQueueMessagesWaiting(queue_aux);
  if ((queue_count >= MAX_QUEUE_COUNT) or (queue_aux_count >= MAX_QUEUE_COUNT))
  {
    Serial.println("max queue capacity reached, not queuing");
    return;
  }

  portENTER_CRITICAL(&receive_cb_mutex);
    // push incoming data to myData
    memcpy(&myData, incomingData, sizeof(myData));
    xQueueSendToFront(queue, &myData, portMAX_DELAY);
    // push MAC to myData_aux (rssi, MAC)
    snprintf(myData_aux.macStr, sizeof(myData_aux.macStr), "%02x:%02x:%02x:%02x:%02x:%02x",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    if (debug_mode) Serial.printf("-->[%s]: macStr: %s\n",__func__,myData_aux.macStr);
    xQueueSendToFront(queue_aux, &myData_aux, portMAX_DELAY);
  portEXIT_CRITICAL(&receive_cb_mutex);

  if (debug_mode) Serial.printf("queued data for %s\n",myData.host);
}
// OnDataRecv callback END
