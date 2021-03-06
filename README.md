<h1> ESPnow MQTT Home Assistant Gateway </h1>

<h2> Idea:</h2>
<ul>
  <li>Build universal sensor device (ESP32 based) equipped with temperature, humidity and light sensors, that can live long on 1 battery charge</li>
  <li>Clone/deploy it to as many sensors as needed (indoor, outdoor etc.) without changing the software or reconfiguration (beyond what is specific to the device i.e. GPIO connection etc.)
  <li>Build gateway device (that is always powered ON) that transfers data from sensor devices to Home Assistant and automatically creates devices/entities on Home Assistant (no configuration needed) - <b>EDIT:</b> gateway device is now "devices" and every sensor device sends ESPnow message to BROADCAST address. Building multiple gateway devices (and locating them in various places) allows widening the range, without loosing the data when RSSI is very low (extending the range).
</ul>
<br>
<h3>Tasks for the devices:</h3>
<ul>
  <li>Sensor devices: (battery powered) wake up, measure the environment and battery, send to gateway over ESPnow, go to sleep, wake up after specified period and repeat
  <li>Gateway device: (always ON) receive message over ESPnow from sensors, convert to Home Assistant auto discovery code and send to Home Assistant over MQTT/WiFi
  <s><p><b>EDIT:</b>Gateway device, since it is always powered ON, got also motion detector (microwave) so it can be also a part of the home alarm system</s> - REMOVED due to interferences with WiFi
</ul>
<br>
<h3> To satisfy the requirements I've chosen: </h3>
<ul>
  <li>SHT31 temperature and humidity sensor, SDA, around 1.5$ on Aliexpress (breakout)
  <li>TSL2561 light sensor, SDA, around 1.5$ on Aliexpress (breakout)
  <li>MAX17048 LiPo battery sensor, SDA 1-5$ (depending if chip only or breakout)
  <li>ESP32S-WROOM or ESP32-S2 WROOM, around 2$ on Aliexpress (bare metal - <b>do NOT use the development board with additional components! - they cannot go to uA due to the on board elements, i.e. LDO, LED etc.)</b>
  <li>few additional components as per schematics (see below)
</ul>
<br>
<h3>Final schematic (ESP32-S2 based):</h3>
<img width="1088" alt="Screenshot 2022-07-07 at 21 45 12" src="https://user-images.githubusercontent.com/46562447/177868190-8f3dac36-4d2f-49e9-b07d-a71ff26dd909.png">

<br>
<br>
<br>
<h3>Final PCB (ESP32-S2 based) - design:</h3>
<img width="1299" alt="Screenshot 2022-07-07 at 21 27 09" src="https://user-images.githubusercontent.com/46562447/177865693-10016c07-eb08-447c-9519-59ccc6162da4.png">
<br>
<img width="1037" alt="Screenshot 2022-07-07 at 21 36 29" src="https://user-images.githubusercontent.com/46562447/177866919-afb88319-9ee9-4c2e-9bfc-9122b748b7b3.png">

<br>
<h3>Preparing the boards to solder:</h3>
<img width="1221" alt="preparation" src="https://user-images.githubusercontent.com/46562447/180612901-c6468ebf-43ef-4aff-9153-c8fa3c386690.png">



<br>
<h3>Soldered on hot plate:</h3>
<img width="1004" alt="soldered" src="https://user-images.githubusercontent.com/46562447/180612912-b02b499b-4335-4df5-aa38-8d840106d9a7.png">

<br>
<h3>SHT31 - temperature and humidity sensor:</h3>
<img width="721" alt="sht" src="https://user-images.githubusercontent.com/46562447/180615851-c810309d-3509-4d79-b8c0-36f1eb9ef930.png">

<br>
<h3>TSL2561 - light sensor:</h3>
<img width="731" alt="tsl" src="https://user-images.githubusercontent.com/46562447/180615863-af1f1b5c-3b35-44e4-bff1-814092fb4a18.png">

<br>
<h3>Testing all before putting into the box:</h3>
<img width="1158" alt="testing" src="https://user-images.githubusercontent.com/46562447/180615874-d182682f-36ac-4bc9-83c6-fdc08481ce72.png">

<br>
<br>
<h2>Power consumption and management</h2>
<h3> Sensor device is powered with LiPo battery and equipped with TP4056 USB-C charger.</h3>
It can be connected to solar panel or - from time to time if possible/needed - to USB-C charger.
<br>
To minimise the sleep current, the power for all sensors is drawn from one of the ESP32 GPIO, so during the sleep time there is no current leakage - sensors are not powered up.
With the above configuration the sleep current is as following (measured with PPK2):
<ul>
  <li>ESP32-S  WROOM - 4.5uA
  <li>ESP32-S2 WROOM - 22.5uA
</ul>
You would say: "ok, ESP32-S is the winner!" but wait, working time and current are as following:
<ul>
  <li>ESP32-S  WROOM - 600ms, 50mA average
  <li>ESP32-S2 WROOM - 200ms, 38mA average
</ul>

ESP32-S WROOM working time and current:
<br>
<img width="1635" alt="s working time 1-2022-06-17 at 20 33 04" src="https://user-images.githubusercontent.com/46562447/174502504-b3937797-6bf0-40bb-af8e-29bc7e98cdff.png">

ESP32-S2 WROOM working time and current:
<br>
<img width="1667" alt="s2 working time 1-2022-06-18 at 12 51 43" src="https://user-images.githubusercontent.com/46562447/174502510-bb3200b7-647a-4269-9ca5-2978d3c97793.png">


And that is what really matters with the battery life time calculation, because 3 times shorter working time makes the difference.<br>
<h4>Rough and rounded up calculation (1000mAh battery, reserve capacity=20%, working time as above, sleep time=180s) shows as following:</h4>
<ul>
  <li>ESP32-S  WROOM: 4 700 hours with average 170uA = 195 days, 6.5 months
  <li>ESP32-S2 WROOM: 12 370 hours with average 65uA = 515 days, 17 months
</ul>

<br>
<b>ESP32-S WROOM battery life time calculation:</b>
<br>

<img width="454" alt="Screenshot 2022-06-19 at 22 20 17" src="https://user-images.githubusercontent.com/46562447/174500944-ad46fd8c-fa2e-4983-8f97-e45409a844da.png">


<br>
<b>ESP32-S2 WROOM battery life time calculation:</b>
<br>

<img width="435" alt="Screenshot 2022-06-19 at 22 20 59" src="https://user-images.githubusercontent.com/46562447/174500948-ce6a89dc-d225-4094-a5b6-923444074d7f.png">
<br>
So apparently the winner is <b>ESP32-S2 WROOM</b> with almost triple battery life.
<p>
<b>EDIT:</b> Final product as per design above consumes while sleeping 33uA - I have included LDO to make sure I have stable 3.3V - unfortunately it takes 8uA - still, as described above, the clue is in the very short working time rather than few uA here or there during the sleep - we are still talking above 9 200 hours on 1 battery charge - more than a year (383 days).
<p>
<br>
  <img width="470" alt="Screenshot 2022-07-23 at 18 24 08" src="https://user-images.githubusercontent.com/46562447/180616082-d279e82b-7feb-4438-9cb7-816fcb115e12.png">

<br>
<br>
<h3>Measuring the working time</h3>
To measure working time you shall NOT rely only on millis or micros. I.e. ESP32S reports millis as first line in setup() as xx while ESP32-S2 shows 280ms.
And actually both are wrong as you can see below on the screens:
<br>
<br>
<b>ESP32S times measured:</b>
<br>
<img width="1728" alt="esp32s" src="https://user-images.githubusercontent.com/46562447/175294471-01d2a13f-5269-493d-ad02-d072cb563ad3.png">

<br>
<br>
<br>
<b>ESP32S-2 times measured. And on the left side the correct ontime reported by the sketch with error correction applied:</b>
<br>
<br>
<img width="1707" alt="s2" src="https://user-images.githubusercontent.com/46562447/175366621-a6fbe7bb-2338-48c7-8939-bfce6faa7541.png">

<br>
<br>
I used PPK2 and estimated the time the ESP32 works measuring the power consumption.
<br>
<br>
<b>Conclusions:</b>
<ul>
  <li>don't rely on millis() only
  <li>ESP32-S2 starts much faster (35ms) than ESP32S (310ms)
  <li>ESP32-S2 total working time is much shorter so the total power consumption with S2 is much lower accordingly - battery life is extended
</ul>
<br>
<br>
<b>ESP32S and ESP32-S2 ontime after 1.5 days with 180s sleep - almost 2.5-3x more "ontime" for ESP32S:</b>
<br>
<img width="1556" alt="Screenshot 2022-07-01 at 09 51 50" src="https://user-images.githubusercontent.com/46562447/176860918-338ccdcc-a744-4832-8502-c4c54a152008.png">
<br>
<br>
<img width="809" alt="Screenshot 2022-07-01 at 09 54 44" src="https://user-images.githubusercontent.com/46562447/176861422-8171adab-98db-4501-8c49-6f7d3a2e9f59.png">

<br>
<br>


<h3>Charging details</h3>
Sensor device also provides information about charging status:
<ul>
  <li>NC - not connected
  <li>ON - charging
  <li>FULL - charged
</ul>  
To achieve this, you need to connect the pins from TP4056 that control charging/power LEDs - see below - to GPIO of ESP32 - see the schematics above. <br>
<br>
<img width="643" alt="Screenshot 2022-06-19 at 21 39 22" src="https://user-images.githubusercontent.com/46562447/174499779-ea0c5474-8ac7-4ac6-acd1-df2f67e84a80.png">

<img width="626" alt="Screenshot 2022-06-19 at 21 40 27" src="https://user-images.githubusercontent.com/46562447/174499788-fd8f5c83-6684-4a11-8a63-4529930a9508.png">

<h2>Firmware update - OTA</h2>
<h3>Sensor device</h3>
To perform firmware update there are 3 possibilities - 2 are with web server in use, where you store the binary file (sender.ino.esp32.bin)
<ul>
  <li>double reset click - built in functions recognise double reset and if so done, performs firmware update - of course you need to visit the sensor to double click it ;-)
  <li>routine check for new firmware availability on the server: every 24h (configurable) sensor device connects to server and if new file found, performs update
  <li>3rd: wire TX/RX to the FTDI programmer
</ul>
Sensor device during firmware update:<br>
<br>
<img width="540" alt="Screenshot 2022-06-22 at 18 55 39" src="https://user-images.githubusercontent.com/46562447/175105073-ad607c41-860e-4edd-8235-77bd8709eb10.png">
<br>
<h3>Gateway device</h3>
To perform firmware update you simply click the button "Update" on Home Assistant (in the device section of gateway) - gateway will connect to the server where the binary is stored and if file is found, it will perform firmware update and restart gateway.
I am using Apache minimal add on Home Assistant - since all sensors are in the same network where Home Assistant is, <b>there is no need for internet access for sensors (and gateway).</b>
<br>

<h2>Configuration</h2>
All sensors used in the sensor device (SHT31, TSL2561, MAX17048, checking charging status) are optional and can be disabled in configuration file.
On top of that, light sensor (TSL2561) can be replaced with phototransistor (i.e. TEPT4400) - also configurable.
In the simplest (and useless) configuration, sensor sends only... its name to Home Assistant.

Sleep time (configurable) is initially set to 180s (3 minutes).

Other important configurable settings (some mandatory, some optional):
<ul>
  <li>WiFi channel - it must be the same for ESPnow communication and gateway to AP (router) communication - gateway and sensor devices
  <li>SSID - gateway and sensor devices
  <li>password - gateway and sensor devices
  <li>MQTT server IP - gateway device
  <li>MQTT username - gateway device
  <li>MQTT password - gateway device
</ul>
<h2>Software</h2>
<h3>Sensor device - sender</h3>
Tasks are as described above so the code is in 1 file only: sender.ino <br>
<br>
Next is the file with credentials: passwords.h (ssid, password, webserver where your firmware is stored) - this file is only used for OTA.<br>
<br>
Finally there is a configuration file where you must specify details for each sensor device (such as sensors used, GPIO etc.): devices_config.h<br>
<br>
The sequence is:
<ul>
  <li>configure the device in devices_config.h
  <li>uncomment the DEVICE_ID you want to compile the sketch for in main sketch: sender.ino
  <li>compile/upload to the device
  <li>repeat the above 3 points for the next sensor device
</ul>

<br>
Sender in action (test device):<br>
<br>
<img width="484" alt="Screenshot 2022-06-22 at 18 56 08" src="https://user-images.githubusercontent.com/46562447/175104814-a21c53e7-a631-441d-b5cc-1bffd557b9a9.png">
<br>

<h3>Gateway device - receiver</h3>
Gateway tasks are more complex (as described above) so the code is split into multiple files - per function<br>
Entire configuration is in config.h file<br>
<br>
Next is the file with credentials: passwords.h (ssid, password, mqtt ip and credentials, webserver where your firmware is stored)
<br>

<br>
Receiver in action (real device):<br>
<br>
<img width="529" alt="Screenshot 2022-06-22 at 18 41 34" src="https://user-images.githubusercontent.com/46562447/175102381-c1ade15e-66b6-44f4-95fa-aba3ba5a88b8.png">

<br>

<h3>Libraries needed (non standard):</h3>
<ul>
  <li>ESP_DoubleResetDetector
  <li>HTTPClient
  <li>Update
  <li>Adafruit_SHT31
  <li>SparkFunTSL2561
  <li>SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library
  <li>PubSubClient
  <li>ArduinoJson
</ul>

<h2>Home Assistant</h2>
<h3>Information from sensor device on Home Assistant:</h3>
<ul>
  <li>device name
  <li>temperature [C]
  <li>humidity [%]
  <li>light [lux]
  <li>battery in [%] and [Volt]
</ul>

Additionally few diagnostic information:
<ul>
  <li>wifi signal/rssi [dBm]
  <li>firmware version
  <li>boot number
  <li>[cumulative] ontime (in seconds) for further postprocessing i.e. in Grafana - it resests to 0 when charging starts
  <li>[cumulative] pretty ontime, i.e.: "1d 17h 23m 12s" (human readable) - it resests to 0 when charging starts
</ul>
<br>
<b>Ontime is calculated</b> based on millis (time just before hibernation - start time) including empirically established difference between real startup time and measured with PPK2 (apparently both: ESP32 and ESP32-S2 are "cheating" with millis, however S2 shows hundreds of millis just after start, while ESP32 shows very low value after start - both are innaccurate and mainly depend on the size of the binary file, that has to be loaded into ESP32 memory during startup by bootloader).
Ontime is also reduced by turning off the bootloader logo during wake up from sleep.
<h4>Gateway device (with its entities) as well as all sensor devices (with their entities) are automatically configured in Home Assistant using MQTT discovery</h4>
<br>
<br>
<b>Gateway device as discovered by Home Assistant:<br>
<br>

<img width="532" alt="Screenshot 2022-07-04 at 09 11 38" src="https://user-images.githubusercontent.com/46562447/177111787-2e294347-cb40-4041-80e7-a02f0d6d7af2.png">

<br><br>
<b>Gateway device on lovelace dashboard:<br>
<br>

<img width="390" alt="Screenshot 2022-07-04 at 09 10 54" src="https://user-images.githubusercontent.com/46562447/177111873-b1d236dd-abfc-4cbf-91e7-c3a316369cee.png">

<br><br>
<b>Sensor device as discovered by Home Assistant:<br>
<br>

<img width="531" alt="Screenshot 2022-07-04 at 09 11 52" src="https://user-images.githubusercontent.com/46562447/177111944-b11afa4f-93ac-4319-966b-8fcfd8772ad1.png">

<br><br><br>
<b>Sensor Device on lovelace dashboard:<br>
<br>

<img width="390" alt="Screenshot 2022-07-04 at 09 11 21" src="https://user-images.githubusercontent.com/46562447/177112009-923aa439-4b00-4d8e-9aac-3a878f2b3087.png">

<br>
<h3>MQTT structure</h3>
All information from sensor device is sent to Home Assistant MQTT broker in one topic: sensor_hostname/sensor/state<br>
<br>
<img width="593" alt="Screenshot 2022-06-20 at 00 17 05" src="https://user-images.githubusercontent.com/46562447/174503988-5944ceb1-49b5-40a9-a96f-f9aeae532dee.png">

<br>And message arrives in JSON format:<br>
<br>
<img width="628" alt="Screenshot 2022-06-22 at 19 26 51" src="https://user-images.githubusercontent.com/46562447/175109966-148e09bf-1cd4-48b1-bd95-0f66ca9b1206.png">


<br>
<h2>TODO list</h2>
<ul>
  <li>Sensor device: OTA - how to do it ON DEMAND? No idea yet. If device is to listen to gateway, it will consume battery... Anybody knows?
</ul>
