#pragma once
/*
firmware update functions
*/
#include "config.h"
#include "variables.h"

// preparation
#if (BOARD_TYPE == 1)
  #define FW_BIN_FILE "/receiver.ino.esp32.bin"
#elif (BOARD_TYPE == 2)
  #define FW_BIN_FILE "/receiver.ino.esp32s2.bin"
#else
  #error "FW update defined only for ESP32 and ESP32-S2 boards"
#endif
#define UPDATE_FIRMWARE_FILE (String(UPDATE_FIRMWARE_HOST) + "/01-Production/0-ESPnow/" + String(HOSTNAME) + String(FW_BIN_FILE))


// functions
void do_update()
{
  bool publish_status_update_firmware = true;
  int update_firmware_status = -1;
  if (perform_update_firmware)
  {
    perform_update_firmware=false;
    publish_status_update_firmware=mqtt_publish_gw_status_values("Updating FW");
    long sm2 = millis(); while(millis() < sm2 + 1000) {};
    if (publish_status_update_firmware)
    {
      update_firmware_status=update_firmware_prepare();
      if (update_firmware_status == 0) {
        Serial.println("FW updated - RESTARTING");
        mqtt_publish_gw_status_values("FW updated");
        sm2 = millis(); while(millis() < sm2 + 1000) {};
        espnow_start();
      } else {
        Serial.println("FW update failed - reason: " + String(update_firmware_status));
        mqtt_publish_gw_status_values("FW update failed");
        sm2 = millis(); while(millis() < sm2 + 1000) {};
        digitalWrite(STATUS_LED_GPIO,LOW);
        mqtt_publish_gw_status_values("online");
        sm2 = millis(); while(millis() < sm2 + 1000) {};
        espnow_start();
      }
    }
  }
}


void updateFirmware(uint8_t *data, size_t len)
{
  if (blink_led_status) {
    blink_led_status=LOW;
    digitalWrite(STATUS_LED_GPIO,blink_led_status);
  } else {
    blink_led_status=HIGH;
    digitalWrite(STATUS_LED_GPIO,blink_led_status);
  }

  char update_progress_char[20];
  Update.write(data, len);
  fw_currentLength += len;
  old_update_progress=update_progress;

  update_progress=(fw_currentLength * 100) / fw_totalLength;
  if (update_progress>old_update_progress)
  {
    sprintf(update_progress_char, "FW update: %d%%", update_progress);
    mqtt_publish_gw_status_values(update_progress_char);
    if (update_progress % 5 == 0)
    {
      Serial.printf("FW update: %d%%\n",update_progress);
    }
  }

  // if current length of written firmware is not equal to total firmware size, repeat
  if(fw_currentLength != fw_totalLength)
  {
    return;
  }
  Update.end(true);
  Serial.printf("\nUpdate Success, Total Size: %d\n", fw_currentLength);
}


int update_firmware_prepare()
{
  long start_upgrade_time = millis();
  String firmware_file = UPDATE_FIRMWARE_FILE;
  fw_totalLength=0;
  fw_currentLength=0;

  Serial.println("uploading file: "+firmware_file);
  firmware_update_client.begin(firmware_file);
  int resp = firmware_update_client.GET();
  Serial.print("Response: ");
  Serial.println(resp);
  // If file is reachable, start downloading
  if(resp == 200)
  {
    // get length of document (is -1 when Server sends no Content-Length header)
    fw_totalLength = firmware_update_client.getSize();
    // transfer to local variable
    int len = fw_totalLength;
    // this is required to start firmware update process
    Update.begin(UPDATE_SIZE_UNKNOWN);
    Serial.printf("FW Size: %u\n",fw_totalLength);
    // create buffer for read
    uint8_t buff[128] = { 0 };
    // get tcp stream
    WiFiClient * stream = firmware_update_client.getStreamPtr();
    // read all data from server
    Serial.println("Updating firmware progress:");
    while(firmware_update_client.connected() && (len > 0 || len == -1))
    {
       // get available data size
       size_t size = stream->available();
       if(size)
       {
          // read up to 128 byte
          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          // pass to function
          updateFirmware(buff, c);
          if(len > 0)
          {
             len -= c;
          }
       }
       delay(1);
    }
  } else
  {
    Serial.println("Cannot download firmware file. Only HTTP response 200: OK is supported. Double check firmware location #defined in UPDATE_FIRMWARE_FILE.");
    Serial.println("update_firmware_prepare UNSUCESSFUL - time: "+String(millis()-start_upgrade_time)+"ms");
    return resp;
  }
  firmware_update_client.end();
  Serial.println("update_firmware_prepare SUCESSFUL - time: "+String(millis()-start_upgrade_time)+"ms");
  return 0;
}
