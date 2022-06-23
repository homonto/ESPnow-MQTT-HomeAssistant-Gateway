/*
sender.ino
*/

// ******************************* DEBUG ***************************************
// #define DEBUG
#define PPK2_GPIO 23          // comment out if not used - GPIO to test power and timings on PPK2

// to see which board is compiled
#define PRINT_COMPILER_MESSAGES

// ********************* choose device - ONLY ONE! *****************************
// detailed config in the file devices_config.h

// #define DEVICE_ID  1           // "esp32028" - S,  Garage
// #define DEVICE_ID  2           // "esp32086" - S2, Lilygo1
// #define DEVICE_ID  3           // "esp32091" - S,  test
// #define DEVICE_ID  4           // "esp32100" - S2, Table
// #define DEVICE_ID  5           // "esp32101" - S,  Dining
// #define DEVICE_ID  6           // "esp32102" - S,  Toilet
// #define DEVICE_ID  7           // "esp32104" - S,  Milena
// #define DEVICE_ID  8           // "esp32090" - S2,  test
// #define DEVICE_ID  9           // "esp32105" - S2,  Garden
#define DEVICE_ID  10           // "esp32087" - S1,  S

// **** reset MAX17048 on first deployment only, then comment it out ***********
// #define RESET_MAX17048

// **** format FS on first deployment only, then change to 0 or comment out ****
#define FORMAT_FS   0

// version description in changelog.txt
#define VERSION "1.10.0"

// configure device in this file, choose which one you are compiling for on top of this script: #define DEVICE_ID x
#include "devices_config.h"

// ****************  ALL BELOW ALL IS COMMON FOR ANY ESP32 *********************
#define HIBERNATE           // hibernate or deep sleep - for deep sleep just comment it out
#define WIFI_CHANNEL  8     // in my house
#define MINIMUM_VOLTS 3.0   // this might go to every device section
#define WAIT_FOR_WIFI 5     // in seconds, for upgrade firmware
#define PERIODIC_FW_CHECK   480 // ((24 * 60 * 60) / (SLEEP_TIME)) // how often to check for FW - for SLEEP_TIME=180s it would be around PERIODIC_FW_CHECK=480 for every 24 hours check

// ******************************  some consistency checks *************************
#if ((USE_TSL2561 == 1) and (USE_TEPT4400 == 1))
  #error "use either USE_TSL2561 or USE_TEPT4400 for measuring lux - not both"
#endif

#if ((USE_TEPT4400 == 1) and (!defined(LUX_ADC_GPIO) or !defined(LUX_MAX_RAW_READING)))
  #error "for measuring lux via TEPT4400 define LUX_ADC_GPIO and LUX_MAX_RAW_READING"
#endif

#if (!defined (HOSTNAME) or !defined(DEVICE_NAME) or !defined(BOARD_TYPE) or !defined(SLEEP_TIME))
  #error "HOSTNAME, DEVICE_NAME, BOARD_TYPE and SLEEP_TIME are obligatory"
#endif


// ****************  LOAD libraries and initiate variables *************************
#include <passwords.h>          // passwords inside my library - for my personal usage
// #include "passwords.h"          // passwords in current folder - for github - see the structure inside the file

// DRD config
// These defines must be put before #include <ESP_DoubleResetDetector.h>
// to select where to store DoubleResetDetector's variable.
#define ESP_DRD_USE_LITTLEFS        true
#ifdef DEBUG
  #define DOUBLERESETDETECTOR_DEBUG true
#endif
#include <ESP_DoubleResetDetector.h>      //https://github.com/khoih-prog/ESP_DoubleResetDetector
//timeout  between 2 resets in seconds
#define DRD_TIMEOUT 1
DoubleResetDetector* drd;
bool DRD_Detected = false;
// DRD config END

// Firmware update
#include <HTTPClient.h>
#include <Update.h>
#if (BOARD_TYPE == 1)
  #define FW_BIN_FILE "/sender.ino.esp32.bin"
#elif (BOARD_TYPE == 2)
  #define FW_BIN_FILE "/sender.ino.esp32s2.bin"
#else
  #error "FW update defined only for ESP32 and ESP32-S2 boards"
#endif
HTTPClient firmware_update_client;
int fw_totalLength = 0;
int fw_currentLength = 0;
bool perform_update_firmware=false;
int update_progress=0;
int old_update_progress=0;
bool blink_led_status=false;
// Firmware update END

// other libraries and variables
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Wire.h>

// filesystem
#define FORMAT_LITTLEFS_IF_FAILED true
#include "FS.h"
#include <LittleFS.h>

// sht31 - temperature and humidity, SDA
#if (USE_SHT31 == 1)
  #include "Adafruit_SHT31.h"
  Adafruit_SHT31 sht31 = Adafruit_SHT31();
#endif

// lux from TETP4400/ADC
#if (USE_TEPT4400 == 1)
  // TETP440 can measure up to 1000 lux so anything above is meaningless
  #define LUX_MAX_RAW_MAPPED_READING 1000
#endif

// lux from TSL2561 - light sensor, SDA
#if (USE_TSL2561 == 1)
  #include <SparkFunTSL2561.h>
  SFE_TSL2561 light;
#endif

// MAX17048 - battery fuel gauge, SDA
#if (USE_MAX17048 == 1)
  #include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>
  SFE_MAX1704X lipo(MAX1704X_MAX17048);
#endif
// MAX17048 END

// ESPnow:

// UNICAST:
// status = 1 when delivered with ack
// it will try to retransmit few times (10?) before giving status = 0
// REPLACE WITH YOUR RECEIVER MAC Address
// receiver might also use arbitrary MAC
// lilygo 3: (BROKEN chip so can be reused as arbitrary): 7c:df:a1:0b:d9:7c
uint8_t broadcastAddress[] = {0x7c, 0xdF, 0xa1, 0x0b, 0xd9, 0x7c};

// BROADCAST:
// uint8_t broadcastAddress[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
// sends to all devices - it works!
// but if broadcast used esp_now_send_status_t status is not received, however communication is faster (no ack needed)
// status = 1 when sent, without waiting for received
// no retransmission for broadcast


typedef struct struct_message
{
  char host[10];
  char temp[6];
  char hum[6];
  char lux[6];
  char bat[6];
  char batpct[6];
  char ver[10];
  char charg[5];
  char name[11];
  char boot[5];
  unsigned long ontime;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// auxuliary variables
#define uS_TO_S_FACTOR 1000000ULL
// become false if sensor is not initiated, it sends then "N/A"
bool sht31ok = true;
bool max17ok = true;
bool tslok   = true;
long program_start_time,em,tt,start_espnow_time;
int bootCount = 1;
unsigned long saved_ontime_l = 0;
unsigned long current_ontime_l = 0;

// files to store some data
#define BOOT_COUNT_FILE     "/bootcount.dat"
#define ONTIME_FILE         "/ontime.dat"


// charging constants
#define CHARGING_NC   "NC"
#define CHARGING_ON   "ON"
#define CHARGING_FULL "FULL"
#define CHARGING_OFF  "OFF"

// ****************  FUNCTIONS *************************

// declaration
void lux_with_tept(char* lux);
void charging_state(char *ch_state);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
bool readFile(fs::FS &fs, const char * path, char * data);
bool writeFile(fs::FS &fs, const char * path, const char * message);
void load_ontime();
void save_ontime();
void hibernate();
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
bool gather_data();
void send_data();
void disable_espnow();
void setup_wifi();
void sos(int led);
void do_update();
void updateFirmware(uint8_t *data, size_t len);
int update_firmware_prepare();
void do_esp_restart();


// implementation
// lux from ADC
#if (USE_TEPT4400 == 1)
  void lux_with_tept(char* lux)
  {
    // it takes 5ms max
    #ifdef DEBUG
      long sm = millis();
    #endif

    uint32_t read_digital = 0;
    size_t nbytes;

    analogSetPinAttenuation(LUX_ADC_GPIO, ADC_11db);
    adcAttachPin(LUX_ADC_GPIO);
    for ( byte i = 0; i < 10; i++) {
        read_digital += analogRead(LUX_ADC_GPIO);
    }
    read_digital /= 10;

    #ifdef DEBUG
      Serial.printf("[%s]: read_digital=%d\n",__func__,read_digital);
    #endif

    if (read_digital > LUX_MAX_RAW_READING) read_digital = LUX_MAX_RAW_READING;
    nbytes = snprintf(NULL,0,"%d",map(read_digital, 0, LUX_MAX_RAW_READING, 0, LUX_MAX_RAW_MAPPED_READING)) +1;
    snprintf(lux,nbytes,"%d",map(read_digital, 0, LUX_MAX_RAW_READING, 0, LUX_MAX_RAW_MAPPED_READING));
    #ifdef DEBUG
      long mt=millis()-sm;
      Serial.printf("\t[%s]: lux_with_tept TIME: %dms\n",__func__,mt);
    #endif
  }
#endif


// check charging
#if defined(CHARGING_GPIO) and defined(POWER_GPIO)
  void charging_state(char *ch_state)
  {
    /*
    TP4056, connect 2 GPIOs to the pins on the chip:
    POWER_GPIO = RED
    CHARGING_GPIO = GREEN
    states are reversed:
    POWER_GPIO = 0 -> RED LED is ON
    CHARGING_GPIO = 0 -> GREEN/BLUE LED is ON
    // truth table:
    no power: L,L
    charging: L,H
    charged: H,L
    existing LEDs on TP4506 must be still connected to pins of TP4506 otherwise pins are floating, or rewire them to new LEDs on the box
    */
    if ((digitalRead(POWER_GPIO) == 0) and (digitalRead(CHARGING_GPIO) == 0)) {snprintf(ch_state,sizeof(CHARGING_NC), "%s", CHARGING_NC);}
    if ((digitalRead(POWER_GPIO) == 0) and (digitalRead(CHARGING_GPIO) == 1)) {snprintf(ch_state,sizeof(CHARGING_ON), "%s", CHARGING_ON);}
    if ((digitalRead(POWER_GPIO) == 1) and (digitalRead(CHARGING_GPIO) == 0)) {snprintf(ch_state,sizeof(CHARGING_FULL), "%s", CHARGING_FULL);}
    if ((digitalRead(POWER_GPIO) == 1) and (digitalRead(CHARGING_GPIO) == 1)) {snprintf(ch_state,sizeof(CHARGING_OFF), "%s", CHARGING_OFF);}
  }
#endif


// list directory
void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
  Serial.printf("[%s]: Listing directory: %s\r\n",__func__,dirname);
  File root = fs.open(dirname);
  if(!root)
  {
    Serial.printf("[%s]: - failed to open directory\n",__func__);
    return;
  }
  if(!root.isDirectory())
  {
    Serial.printf("[%s]: - not a directory\n",__func__);
    return;
  }
  File file = root.openNextFile();
  while(file)
  {
    if(file.isDirectory())
    {
      Serial.printf("[%s]:  DIR : %s\n",__func__,file.name());
      if(levels)
      {
          listDir(fs, file.name(), levels -1);
      }
    } else
    {
      Serial.printf("[%s]:  FILE: %s\tSIZE: %d\n",__func__,file.name(),file.size());
    }
    file = root.openNextFile();
  }
}


// read file
bool readFile(fs::FS &fs, const char * path, char * data)
{
  File file = fs.open(path);
  if (!file)
  {
    #ifdef DEBUG
      Serial.printf("[%s]: Failed to open file: %s\n",__func__,path);
    #endif
    return false;
  }
  for(int j = 0; j < file.size(); j++)
  {
    data[j] = (char)file.read();
  }
  return true;
}


// write file
bool writeFile(fs::FS &fs, const char * path, const char * message)
{
  #ifdef DEBUG
    Serial.printf("[%s]: Writing file: %s\r",__func__,path);
  #endif
  File file = fs.open(path, FILE_WRITE);
  if(!file){
    #ifdef DEBUG
      Serial.printf(" - failed to open file for writing\n",__func__);
    #endif
    return false;
  }
  if(file.print(message)){
    #ifdef DEBUG
      Serial.printf(" - file written\n",__func__);
    #endif
  } else {
    #ifdef DEBUG
      Serial.printf(" - write failed\n",__func__);
    #endif
  }
  file.close();
  return true;
}


void load_ontime()
{
  char saved_ontime_ch[12];
  if (readFile(LittleFS, ONTIME_FILE, saved_ontime_ch))
  {
    saved_ontime_l = atol(saved_ontime_ch);
    #ifdef DEBUG
      Serial.printf("[%s]: saved_ontime_l=%lums\n",__func__,saved_ontime_l);
      #endif
  } else
  {
    #ifdef DEBUG
      Serial.printf("[%s]: CANNOT READ SAVED ONTIME!\n",__func__);
    #endif
  }
}


void save_ontime()
{
  char total_ontime_ch[12];

  // add new running time to saved_ontime_l
  current_ontime_l = millis() + ESP32_IS_CHEATING;
  saved_ontime_l = saved_ontime_l + current_ontime_l;

  // reset saved_ontime_l if device is charging
  #if defined(CHARGING_GPIO) and defined(POWER_GPIO)
    char charging[5];
    charging_state(charging);
    // testing charging
    // snprintf(charging,sizeof(charging),"%s",CHARGING_FULL);
    #ifdef DEBUG
      // Serial.printf("charging=%s\n",charging);
    #endif
    // strcmp = 0 when strings ARE equal
    if ( (strcmp(charging, CHARGING_ON) == 0) or (strcmp(charging, CHARGING_FULL) == 0) )
    {
      saved_ontime_l = 0;
      #ifdef DEBUG
        Serial.printf("[%s]: Device is CHARGING, reset: saved_ontime_l=%lums\n",__func__,saved_ontime_l);
      #endif
    } else
    {
      #ifdef DEBUG
        Serial.printf("[%s]: Device is NOT CHARGING, new saved_ontime_l=%lums\n",__func__,saved_ontime_l);
      #endif
    }
  #endif

  int nbytes = snprintf(NULL,0,"%lu",saved_ontime_l) + 1;
  snprintf(total_ontime_ch,nbytes,"%lu",saved_ontime_l);
  // write to file
  if (writeFile(LittleFS, ONTIME_FILE, total_ontime_ch))
  {
    #ifdef DEBUG
      Serial.printf("[%s]: total_ontime saved to file=%sms\n",__func__,total_ontime_ch);
    #endif
  } else
  {
    #ifdef DEBUG
      Serial.printf("[%s]: total_ontime NOT saved to file\n",__func__);
    #endif
  }

  // testing PPK2
  #ifdef PPK2_GPIO
    digitalWrite(PPK2_GPIO,LOW);
  #endif

  Serial.printf("[%s]: Program finished after %lums. Bye...\n",__func__,current_ontime_l);
}


void do_esp_restart()
{
  save_ontime();
  ESP.restart();
}


// deep sleep
void hibernate()
{
  // wifi
  esp_wifi_stop();
  // all gpio isolated
  esp_sleep_config_gpio_isolate();
  // it does not display welcome message
  esp_deep_sleep_disable_rom_logging();

  #ifdef HIBERNATE
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,   ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL,         ESP_PD_OPTION_OFF);
    // added extra, check if no issue
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC8M,         ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO,       ESP_PD_OPTION_OFF);
    #ifdef DEBUG
      Serial.printf("[%s]: sleep mode: hibernate\n",__func__);
    #endif
  #else
    #ifdef DEBUG
      Serial.printf("[%s]: sleep mode: deep sleep\n",__func__);
    #endif
  #endif
  save_ontime();
  esp_deep_sleep_start();
}


// on sent callback
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  {
    if (status == ESP_NOW_SEND_SUCCESS)
    {
      Serial.printf("[%s]: ESPnow SUCCESSFULL\n",__func__);
    } else
    {
      Serial.printf("[%s]: ESPnow FAILED\n",__func__);
    }
    tt=millis() - start_espnow_time;
    Serial.printf("[%s]: ESPnow took:....%dms\n",__func__,tt);
  }
}


// gather data from sensors, returns false if (bat_volts < MINIMUM_VOLTS) so ESPnow will not start - goes to sleep instead of sending
bool gather_data()
{
  #ifdef DEBUG
    Serial.printf("[%s]:\n",__func__);
  #endif
  // hostname
  strcpy(myData.host, HOSTNAME);
  #ifdef DEBUG
    Serial.printf(" Data gatehered:\n\thost=%s\n",myData.host);
  #endif

  // name
  strcpy(myData.name, DEVICE_NAME);
  #ifdef DEBUG
    Serial.printf("\tname=%s\n",myData.name);
  #endif

  // sht31
  snprintf(myData.temp,sizeof(myData.temp),"%s","N/A");
  snprintf(myData.hum,sizeof(myData.hum),"%s","N/A");
  #if (USE_SHT31 == 1)
    if (sht31ok)
    {
      snprintf(myData.temp,sizeof(myData.temp),"%0.2f",sht31.readTemperature());
      snprintf(myData.hum,sizeof(myData.hum),"%0.2f",sht31.readHumidity());
    }
  #endif
  #ifdef DEBUG
    Serial.printf("\ttemp=%s\n",myData.temp);
    Serial.printf("\thum=%s\n",myData.hum);
  #endif

  // lux
  snprintf(myData.lux,sizeof(myData.lux),"%s","N/A");
  #if (USE_TEPT4400 == 1)
    lux_with_tept(myData.lux);
  #elif (USE_TSL2561 == 1)
    unsigned int data0, data1; //data0=infrared, data1=visible light
    double lux;              // Resulting lux value
    boolean good;            // True if neither sensor is saturated
    boolean gain = false;     // Gain setting, 0 = X1, 1 = X16; if with gain and overshoots it goes from 0 again - better false
    unsigned int ms;         // Integration ("shutter") time in milliseconds
    // If time = 0, integration will be 13.7ms
    // If time = 1, integration will be 101ms
    // If time = 2, integration will be 402ms
    // If time = 3, use manual start / stop to perform your own integration
    unsigned char time = 0;

    light.begin();
    light.setTiming(gain,time,ms);
    if (light.setPowerUp()) tslok = true; else tslok = false;

    light.manualStart();
    delay(ms);
    light.manualStop();

    if (tslok)
    {
      if (light.getData(data0,data1))
      {
        good = light.getLux(gain,ms,data0,data1,lux);
        byte nbytes = snprintf(NULL,0,"%0f",lux) +1;
        snprintf(myData.lux,nbytes,"%0.0f",lux);
      }
      else
      {
        // measurement not ok
        snprintf(myData.lux,sizeof(myData.lux),"%s","N/A");
      }
    } else
    {
      // tslok (sensor initialize) not ok
      snprintf(myData.lux,sizeof(myData.lux),"%s","N/A");
    }
  #endif
  #ifdef DEBUG
    Serial.printf("\tlux=%s\n",myData.lux);
  #endif

  // battery
  snprintf(myData.bat,sizeof(myData.bat),"%s","N/A");
  snprintf(myData.batpct,sizeof(myData.batpct),"%s","N/A");
  #if (USE_MAX17048 == 1)
    if (max17ok)
    {
      float bat_volts = 0.0f;
      float bat_pct = 0.0f;
      bat_volts = lipo.getVoltage();
      bat_pct = lipo.getSOC();
      snprintf(myData.bat,sizeof(myData.bat),"%0.2f",bat_volts);
      snprintf(myData.batpct,sizeof(myData.batpct),"%0.2f",bat_pct);

      if (bat_volts < MINIMUM_VOLTS)
      {
        #ifdef DEBUG
          Serial.printf("[%s]: battery volts=%0.2fV, that is below minimum [%0.2fV]\n",__func__,bat_volts,MINIMUM_VOLTS);
        #endif

        if (DRD_Detected)
        {
          #ifdef DEBUG
            tt = millis() - program_start_time;
            Serial.printf("[%s]: NOT hibernating as DRD detected %dms after boot\n",__func__,tt);
          #endif
        } else
        {
          #ifdef DEBUG
            tt = millis() - program_start_time;
            Serial.printf("[%s]: NOT sending data! leaving [gather_data=false] %dms after boot\n",__func__,tt);
          #endif
          return false;
        }
      } else {
        #ifdef DEBUG
          Serial.printf("[%s]: battery level OK\n",__func__);
        #endif
      }
    }
    // send MAX17048 to sleep to disable measurements while sleeping, reduce current by 3uA more
    // it does not measure battery during sleep so continuity is lost - not enabling it
    // anyway, if ESP32S measures only 4.5uA during hibernate, it means it is irrelevant for the sleep - it only disables periodic (every 45s) measuring of the battery
    // lipo.sleep();
  #endif
  #ifdef DEBUG
    Serial.printf("\tbat=%s\n",myData.bat);
    Serial.printf("\tbatpct=%s\n",myData.batpct);
  #endif

  // version
  if (DRD_Detected)
  {
    // send to HA "FW UPG..." instead of version during FW upgrade to indicate the process
    snprintf(myData.ver,sizeof(myData.ver),"%s","FW UPG...");
  } else
  {
    strcpy(myData.ver, VERSION);
  }
  #ifdef DEBUG
    Serial.printf("\tver=%s\n",myData.ver);
  #endif

  // charging
  #if defined(CHARGING_GPIO) and defined(POWER_GPIO)
    charging_state(myData.charg);
  #else
    snprintf(myData.charg,4,"%s","N/A");
  #endif
  #ifdef DEBUG
    Serial.printf("\tcharg=%s\n",myData.charg);
  #endif

  // bootCount
  snprintf(myData.boot,sizeof(myData.boot),"%d",bootCount);
  #ifdef DEBUG
    Serial.printf("\tboot=%s\n",myData.boot);
  #endif

  // ontime in seconds rather than ms
  myData.ontime = saved_ontime_l/1000;
  #ifdef DEBUG
    Serial.printf("\tontime=%lu\n",myData.ontime);
  #endif

  return true;
}


// send data to gateway over ESPnow
void send_data()
{
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  if (result != ESP_OK) {
    #ifdef DEBUG
      Serial.printf("[%s]: Error sending the data\n",__func__);
    #endif
  } else
  {
    #ifdef DEBUG
      Serial.printf("[%s]: data sent\n",__func__);
    #endif
  }
}


// update firmware
void disable_espnow()
{
  esp_now_unregister_send_cb();
  esp_now_deinit();
}


// connect to AP for FW upgrade
void setup_wifi()
{
  long ttc, sm1, wifi_start_time;
  WiFi.mode(WIFI_MODE_STA);
  wifi_start_time = millis();
  WiFi.begin(BT_SSID, BT_PASSWORD,WIFI_CHANNEL);
  Serial.printf("\n[%s]: Connecting to %s ...\n",__func__,BT_SSID);

  while (WiFi.status() != WL_CONNECTED)
  {
    ttc = millis() - wifi_start_time;
    sm1=millis(); while(millis() < sm1 + 20) {}
    if (ttc > (WAIT_FOR_WIFI * 1000)) {
      Serial.printf("[%s]: still NOT connected after %dms\nRESTARTING - WiFi not connected\n",__func__,ttc);
      do_esp_restart();
    }
  }
  Serial.printf("[%s]: connected after %dms\n",__func__,ttc);
}


// blink nicely - sos for upgrade failure
void sos(int led)
{
  #define DIT_MS 75;
  int dit = DIT_MS;
  int dah = 3 * dit;
  int inter_dit = dit;
  int inter_letter = 3 * dit;
  int inter_word = 7 * dit;

  digitalWrite(led,LOW);
  delay(inter_word);

  // s
  for (byte i=0; i<3; i++)
  {
    digitalWrite(led,HIGH);
    delay(dit);
    digitalWrite(led,LOW);
    delay(dit);
  }
  delay(inter_letter);
  // 0
  for (byte i=0; i<3; i++)
  {
    digitalWrite(led,HIGH);
    delay(dah);
    digitalWrite(led,LOW);
    delay(dit);
  }
  delay(inter_letter);
  // s
  for (byte i=0; i<3; i++)
  {
    digitalWrite(led,HIGH);
    delay(dit);
    digitalWrite(led,LOW);
    delay(dit);
  }
  delay(inter_word);
}


// FW upgrade wrapper
void do_update()
{
  int update_firmware_status = -1;
  update_firmware_status=update_firmware_prepare();
  if (update_firmware_status == 0)
  {
    Serial.printf("[%s]: RESTARTING - FW update SUCCESSFULL\n\n",__func__);
    // blink nicely when FW upgrade successfull
    for (int i=0;i<3;i++)
    {
      #ifdef FW_UPGRADE_LED_GPIO
        digitalWrite(FW_UPGRADE_LED_GPIO,LOW);
        delay(500);
        digitalWrite(FW_UPGRADE_LED_GPIO,HIGH);
        delay(100);
      #elif defined(ACTIVITY_LED_GPIO)
        digitalWrite(ACTIVITY_LED_GPIO,LOW);
        delay(500);
        digitalWrite(ACTIVITY_LED_GPIO,HIGH);
        delay(100);
      #endif
    }

  } else
  {
    Serial.printf("[%s]: FW update failed - reason: %d\nRESTARTING - FW update failed\n\n",__func__,update_firmware_status);
    #ifdef FW_UPGRADE_LED_GPIO
      sos(FW_UPGRADE_LED_GPIO);
    #elif defined(ACTIVITY_LED_GPIO)
      sos(ACTIVITY_LED_GPIO);
    #endif
  }
  do_esp_restart();
}


// real upgrade
void updateFirmware(uint8_t *data, size_t len)
{
  // blink FW_UPGRADE_LED_GPIO or...
  #ifdef FW_UPGRADE_LED_GPIO
    if (blink_led_status) {
      blink_led_status=LOW;
      digitalWrite(FW_UPGRADE_LED_GPIO,blink_led_status);
    } else {
      blink_led_status=HIGH;
      digitalWrite(FW_UPGRADE_LED_GPIO,blink_led_status);
    }
  #else
    // ...blink ACTIVITY_LED_GPIO
    #ifdef ACTIVITY_LED_GPIO
      if (blink_led_status) {
        blink_led_status=LOW;
        digitalWrite(ACTIVITY_LED_GPIO,blink_led_status);
      } else {
        blink_led_status=HIGH;
        digitalWrite(ACTIVITY_LED_GPIO,blink_led_status);
      }
    #endif
  #endif

  Update.write(data, len);
  fw_currentLength += len;
  old_update_progress=update_progress;
  update_progress=(fw_currentLength * 100) / fw_totalLength;
  if (update_progress>old_update_progress){
    if (update_progress % 5 == 0){ //if uncomment it prints every 2%, otherwise every 5%
      Serial.printf("[%s]: FW update: %d%%\n",__func__,update_progress);
    }
  }
  // if current length of written firmware is not equal to total firmware size, repeat
  if(fw_currentLength != fw_totalLength) return;
  Update.end(true);
  Serial.printf("\n[%s]: Update Success, Total Size: %d\n",__func__,fw_currentLength);
}


// download from webserver
int update_firmware_prepare()
{
  char firmware_file[255];
  snprintf(firmware_file,sizeof(firmware_file),"%s/01-Production/0-ESPnow/%s/%s",UPDATE_FIRMWARE_HOST,HOSTNAME,FW_BIN_FILE);

  fw_totalLength=0;
  fw_currentLength=0;
  Serial.printf("[%s]: uploading file: %s\n",__func__,firmware_file);
  firmware_update_client.begin(firmware_file);
  int resp = firmware_update_client.GET();
  Serial.printf("[%s]: Response: %d\n",__func__,resp);
  // If file is reachable, start downloading
  if(resp == 200){
    // get length of document (is -1 when Server sends no Content-Length header)
    fw_totalLength = firmware_update_client.getSize();
    // transfer to local variable
    int len = fw_totalLength;
    // this is required to start firmware update process
    Update.begin(UPDATE_SIZE_UNKNOWN);
    Serial.printf("[%s]: FW Size: %lubytes\n",__func__,fw_totalLength);

    // create buffer for read
    uint8_t buff[128] = { 0 };
    // get tcp stream
    WiFiClient * stream = firmware_update_client.getStreamPtr();
    // read all data from server
    Serial.printf("[%s]: Updating firmware progress:\n",__func__);
    while(firmware_update_client.connected() && (len > 0 || len == -1))
    {
      // get available data size
      size_t size = stream->available();
      if(size) {
        // read up to 128 byte
        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
        // pass to function
        updateFirmware(buff, c);
        if(len > 0) {
           len -= c;
        }
      }
      delay(1);
      }
  }else
  {
    Serial.printf("[%s]: Cannot download firmware file. Only HTTP response 200: OK is supported. Double check firmware location.\n",__func__);
    Serial.printf("[%s]: firmware update prepare UNSUCESSFUL\n",__func__);
    return resp;
  }
  firmware_update_client.end();
  Serial.printf("[%s]: firmware update prepare SUCESSFUL\n",__func__);
  return 0;
}
// update firmware END


// setup
void setup()
{
  // testing PPK2
  #ifdef PPK2_GPIO
    pinMode(PPK2_GPIO,OUTPUT);
    digitalWrite(PPK2_GPIO,HIGH);
  #endif

  program_start_time = millis();

  Serial.begin(115200);
  Serial.printf("\n[%s]: Device: %s (%s)\n",__func__,DEVICE_NAME,HOSTNAME);
  esp_sleep_enable_timer_wakeup(SLEEP_TIME * uS_TO_S_FACTOR);

// custom SDA & SCL
  #if (USE_CUSTOM_I2C_GPIO == 1)
    Wire.setPins(SDA_GPIO,SCL_GPIO);
  #endif

// turn on LED
  #ifdef ACTIVITY_LED_GPIO
    pinMode(ACTIVITY_LED_GPIO, OUTPUT);
    digitalWrite(ACTIVITY_LED_GPIO, HIGH);
  #endif

// power for sensors from GPIO - MUST be before any SDA sensor is in use obviously!
  #ifdef ENABLE_3V_GPIO
    #ifdef DEBUG
      Serial.printf("[%s]: enabling ENABLE_3V_GPIO\n",__func__);
    #endif
    pinMode(ENABLE_3V_GPIO, OUTPUT);
    digitalWrite(ENABLE_3V_GPIO, HIGH);
    delay(10);
  #endif

// start Wire after enabling 3.3V
  Wire.begin(); // it takes 0.6ms

// MAX17048 - fuel gauge
  #if (USE_MAX17048 == 1)
    #ifdef DEBUG
      lipo.enableDebugging();
      Serial.printf("[%s]: start USE_MAX17048\n",__func__);
    #endif
    if (! lipo.begin())
    {
      // #ifdef DEBUG
        Serial.printf("[%s]: MAX17048 NOT detected ... Check your wiring or I2C ADDR!\n",__func__);
      // #endif
      max17ok = false;
    } else
    {
      // delay(1000);
      #ifdef DEBUG
        Serial.printf("[%s]: start MAX17048 OK\n",__func__);
      #endif
      lipo.quickStart();
      delay(10);
      #ifdef RESET_MAX17048
        lipo.reset();
      #endif
    }
  #else
    #ifdef DEBUG
      Serial.printf("[%s]: DONT USE_MAX17048\n",__func__);
    #endif
  #endif

// Firmware update
  #ifdef DEBUG
    char firmware_file[255];
    snprintf(firmware_file,sizeof(firmware_file),"%s/01-Production/0-ESPnow/%s/%s",UPDATE_FIRMWARE_HOST,HOSTNAME,FW_BIN_FILE);
    Serial.printf("[%s]: firmware file:\n %s\n",__func__,firmware_file);
  #endif

// DRD
  drd = new DoubleResetDetector(DRD_TIMEOUT, 0);
  if (drd->detectDoubleReset())
  {
    drd->stop(); //needed here? I did not figure it out yet
    // #ifdef DEBUG
      Serial.printf("\n[%s]: SETUP: Double Reset Detected\n",__func__);
    // #endif
    DRD_Detected = true;
    #ifdef FW_UPGRADE_LED_GPIO
      pinMode(FW_UPGRADE_LED_GPIO,OUTPUT);
      digitalWrite(FW_UPGRADE_LED_GPIO, HIGH);
    #else
      #ifdef ACTIVITY_LED_GPIO
        pinMode(ACTIVITY_LED_GPIO,OUTPUT);
        digitalWrite(ACTIVITY_LED_GPIO, HIGH);
      #endif
    #endif

  }
// DRD END

// read/increase/save bootCount
  char data[5];
  if(!LittleFS.begin(true))
  {
    Serial.printf("[%s]: LittleFS Mount Failed\n",__func__);
  } else
  {
    // format FS on first deployment
    #if (FORMAT_FS == 1)
      Serial.printf("[%s]: formatting FS...",__func__);
      // Serial.println(LittleFS.format() ? "SUCCESSFULL" : "FAILED");
      if (LittleFS.format())
      {
        Serial.printf("[%s]: SUCCESSFULL\n",__func__);
      } else
      {
        Serial.printf("[%s]: FAILED\n",__func__);
      }
    #endif
    // list files in DEBUT mode only
    #ifdef DEBUG
      listDir(LittleFS,"/",1);
    #endif
    // read bootCount from file
    if (readFile(LittleFS, BOOT_COUNT_FILE, data))
    {
      bootCount = atoi(data) + 1;
      #ifdef DEBUG
        Serial.printf("[%s]: bootCount=%d\n",__func__,bootCount);
      #endif
      if (bootCount >= PERIODIC_FW_CHECK)
      {
        Serial.printf("[%s]: time to check FW\n",__func__);
        // cheating here: reusing DRD_Detected to initiate the FW upgrade
        DRD_Detected = true;
        bootCount = 1;
      }
    }
    // reset bootCount on DRD_Detected
    if (DRD_Detected) bootCount = 1;
    // convert int to char array
    int nbytes = snprintf(NULL,0,"%d",bootCount) + 1;
    snprintf(data,nbytes,"%d",bootCount);

    // write to file
    if (writeFile(LittleFS, BOOT_COUNT_FILE, data))
    {
      #ifdef DEBUG
        Serial.printf("[%s]: bootCount saved\n",__func__);
      #endif
    } else
    {
      #ifdef DEBUG
        Serial.printf("[%s]: bootCount NOT saved\n",__func__);
      #endif
    }
  }
// save bootCount END

// load saved ontime
  load_ontime();

// check if charging
  #if (defined(CHARGING_GPIO) and defined(POWER_GPIO))
    #ifdef DEBUG
      Serial.printf("[%s]: CHARGING_GPIO and POWER_GPIO enabled\n",__func__);
    #endif
    pinMode(CHARGING_GPIO, INPUT_PULLDOWN);  //both down: NC initially, will be changed when checked
    pinMode(POWER_GPIO, INPUT_PULLDOWN);
  #else
    #ifdef DEBUG
      Serial.printf("[%s]: CHARGING_GPIO and POWER_GPIO DISABLED\n",__func__);
    #endif
  #endif

//lux
  #if (USE_TSL2561 == 1)
    #ifdef DEBUG
      Serial.printf("[%s]: start USE_TSL2561\n",__func__);
    #endif
    if (! light.begin())  // it does NOT return false so this checking is meaningless I think
    {
      #ifdef DEBUG
        Serial.printf("[%s]: TSL2561  NOT detected ... Check your wiring or I2C ADDR!\n",__func__);
      #endif
      tslok = false;
    }
    else
    {
      if (light.setPowerUp())
      {
        #ifdef DEBUG
          Serial.printf("[%s]: tslok =%d\n",__func__,tslok);
        #endif
      } else
      {
        Serial.printf("[%s]: TSL2561  NOT detected ... Check your wiring or I2C ADDR!\n",__func__);
      }
    }
  #else
    #ifdef DEBUG
      Serial.printf("[%s]: DONT USE_TSL2561\n",__func__);
    #endif
  #endif

//sht31
  #if (USE_SHT31 == 1)
    #ifdef DEBUG
      Serial.printf("[%s]: start USE_SHT31\n",__func__);
    #endif
    if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
      // #ifdef DEBUG
        Serial.printf("[%s]: SHT31    NOT detected ... Check your wiring or I2C ADDR!\n",__func__);
      // #endif
      sht31ok = false;
    } else
    {
      #ifdef DEBUG
        Serial.printf("[%s]: sht31ok =%d\n",__func__,sht31ok);
      #endif
    }
  #else
    #ifdef DEBUG
      Serial.printf("[%s]: DONT USE_SHT31\n",__func__);
    #endif
  #endif

// gather data
  if (!gather_data())
  {
    #ifdef DEBUG
      Serial.printf("[%s]: NOT sending ANY data - gethering data FAILED!\n",__func__);
    #endif
    return;
  } else
  {
    #ifdef DEBUG
      em = millis(); tt = em - program_start_time;
      Serial.printf("[%s]: took: %dms\n",__func__,tt);
    #endif
  }
// gather data END

// turn off power for sensors - not needed anymore
  #ifdef ENABLE_3V_GPIO
    digitalWrite(ENABLE_3V_GPIO, LOW);
  #endif

// ESPNow preparation
  start_espnow_time = millis();

  WiFi.mode(WIFI_MODE_STA);
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  #ifdef DEBUG
    // WiFi.printDiag(Serial); - it shows even password ;-)
  #endif
  if (esp_now_init() != ESP_OK) {
    #ifdef DEBUG
      Serial.printf("[%s]: ESP NOW failed to initialize\n",__func__);
    #endif
  }
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.ifidx   = WIFI_IF_STA;
  peerInfo.encrypt = false;
  esp_now_register_send_cb(OnDataSent);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    #ifdef DEBUG
      Serial.printf("[%s]: ESP NOW pairing failure\n",__func__);
    #endif
  } else
  {
    #ifdef DEBUG
      Serial.printf("[%s]: over ESPnow...",__func__);
    #endif
    send_data();
  }
}


// loop
void loop()
{
  drd->loop();
  if (!DRD_Detected)
  {
    drd->stop();
    hibernate();
  }
  else
  {
    Serial.printf("[%s]: Update firmware scheduled\n",__func__);
    drd->stop();
    disable_espnow();
    setup_wifi();
    do_update();
  }
}
