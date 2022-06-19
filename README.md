<h1> ESPnow MQTT Home Assistant Gateway </h1>

<h2> Idea:</h2>
<ul>
  <li>Build universal sensor device (ESP32 based) equipped with temperature, humidity and light sensor that can live long on 1 battery charge</li>
  <li>Clone/deploy it to as many sensors as needed (indoor, outdoor etc.) without changing the software or reconfiguration (beyond what is specific to the device i.e. GPIO connection changes)
  <li>Build gateway device (that is always powered ON) that transfers data from sensor devices to Home Assistant
</ul>

<h3>Tasks for the devices:</h3>
<ul>
  <li>Sensor devices: (battery powered) wake up, measure the environment and battery, send to gateway over ESPnow, go to sleep, wake up after specified period and repeat
  <li>Gateway device: (always ON) receive message over ESPnow from sensors, convert to Home Assistant auto discovery code and send to Home Assistant over MQTT/WiFi
</ul>

<h3> To satisfy the requirements I've chosen: </h3>
<ul>
  <li>SHT31 temperature and humidity sensor, SDA, around 1.5$ on Aliexpress (breakout)
  <li>TSL2561 light sensor, SDA, around 1.5$ on Aliexpress (breakout)
  <li>MAX17048 LiPo battery sensor, SDA 1-5$ (depending if chip only or breakout)
  <li>ESP32S-WROOM or ESP32-S2 WROOM, around 2$ on Aliexpress (bare metal - <b>do NOT use the development board with additional components! - they cannot go to uA due to the on board elements, i.e. LDO, LED etc.)</b> 
  <li>few additional components as per schematics (see below)
</ul>

<img width="1056" alt="s" src="https://user-images.githubusercontent.com/46562447/174493436-59580695-339b-47a6-a3c7-aca9be8a14dc.png">
<img width="1030" alt="s2" src="https://user-images.githubusercontent.com/46562447/174493450-0c83bded-bc4d-4e2b-81a3-2a97067c947f.png">


<h2>Power consumption and management</h2>
<h3> Sensor device is powered with LiPo battery and equipped with TP4056 USB-C charger.</h3>
It can be connected to solar panel or - from time to time if possible/needed - to USB-C charger.
<br><br>
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

ESP32-S3 WROOM working time and current:
<br>
<img width="1667" alt="s2 working time 1-2022-06-18 at 12 51 43" src="https://user-images.githubusercontent.com/46562447/174502510-bb3200b7-647a-4269-9ca5-2978d3c97793.png">


And that is what really matters with the battery life time calculation, because 3 times shorter working time makes the difference.<br>
<h4>Rough and rouded up calculation (1000mAh battery, reserve capacity=20%, working time as above, sleep time=180s) shows as following:</h4>
<ul>
  <li>ESP32-S  WROOM: 4 700 hours with average 170uA = 195 days, 6.5 months
  <li>ESP32-S2 WROOM: 12 370 hours with average 65uA = 515 days, 17 months
</ul>

<br>
<b>ESP32-S WROOM battery life time calculation:</b>
<br><br>

<img width="454" alt="Screenshot 2022-06-19 at 22 20 17" src="https://user-images.githubusercontent.com/46562447/174500944-ad46fd8c-fa2e-4983-8f97-e45409a844da.png">


<br>
<b>ESP32-S2 WROOM battery life time calculation:</b>
<br><br>

<img width="435" alt="Screenshot 2022-06-19 at 22 20 59" src="https://user-images.githubusercontent.com/46562447/174500948-ce6a89dc-d225-4094-a5b6-923444074d7f.png">
<br>
So apparently the winner is <b>ESP32-S2 WROOM</b> with almost triple battery life.
<br><br>
<h3>Measuring the working time</h3>
To measure working time you shall NOT use millis or micros - ESP32 gives totally strange readings even if you print millis() just as the second line in void setup() (after Serial.begin(x)) - I used PPK2 and estimated the time the ESP32 works measuring the power consumption. 
<br><br>

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
</ul>

<h2>Firmware upgrade - OTA</h2>
To perform firmware upgrade there are 2 possibilities - both are with web server in use, where you store the binary file (sender.ino.esp32.bin)
<ul>
  <li>double reset click - built in functions recognise double reset and if so done, performs firmware upgrade - of course you need to visit the sensor to double click it ;-)
  <li>routine check for new firmware availability on the server: every 24h (configurable) sensors connects to server and if new file found, performs upgrade
</ul>
I am using Apache minimal add on Home Assistant - since all sensors are in the same network where Home Assistant is, <b>there is no need for internet access for sensors (and gateway) but sensor device needs to be in the accessible range of the AP/router.</b><br>

<h2>Configuration</h2>
All sensors used in the sensor device (SHT31, TSL2561, MAX17048, checking charging status) are optional and can be disabled in configuration file.
On top of that, light sensor (TSL2561) can be replaced with phototransistor (i.e. TEPT4400) - also configurable.
In the simplest (and useless) configuration, sensor sends only... its name to Home Assistant.

Sleep time (configurable) is initially set to 180s (3 minutes).

Other important configurable settings (some mandatory, some optional) (mainly for gateway):
<ul>
  <li>WiFi channel - it must be the same for ESPnow communication and gateway to AP (router) communication
  <li>SSID
  <li>password
  <li>MQTT server IP
  <li>MQTT username
  <li>MQTT password
</ul>
<br>
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
<img width="433" alt="sender" src="https://user-images.githubusercontent.com/46562447/174503034-f410b265-ab90-4d58-b6a0-f1038b03dfcb.png">
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
<img width="547" alt="receiver" src="https://user-images.githubusercontent.com/46562447/174503040-2754766e-4f3d-4fa4-be78-58db44685a28.png">
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
<h4>Gateway device (with its entities) as well as all sensor devices (with their entities) are automatically configured in Home Assistant using MQTT discovery</h4>
<br>

<b>Gateway device as discovered by MQTT:</b>
<br><br>
<img width="540" alt="Screenshot 2022-06-19 at 18 48 53" src="https://user-images.githubusercontent.com/46562447/174493967-357bdf70-0b84-4d87-8edd-88b8b83fb447.png">
<br><br>
<b>Gateway device on lovelace dashboard:</b>
<br><br>
<img width="402" alt="Screenshot 2022-06-19 at 18 48 35" src="https://user-images.githubusercontent.com/46562447/174493970-9901a285-e271-4ceb-a5f3-530999bfc31e.png">
<br><br>
<b>Sensor device as discovered by MQTT:</b>
<br><br>
<img width="809" alt="Screenshot 2022-06-19 at 12 43 32" src="https://user-images.githubusercontent.com/46562447/174488031-cf575458-4a8f-4193-bfaf-33d7e14fd2a3.png">
<br><br>
<b>Sensor Device on lovelace dashboard:</b>
<br><br>
<img width="400" alt="Screenshot 2022-06-19 at 12 43 51" src="https://user-images.githubusercontent.com/46562447/174488029-645ff458-5a33-4814-8637-d4f40de59a2d.png">
<br><br>

<h2>TODO list</h2>
<ul>
  <li>Gateway device: OTA not yet finished
  <li>Sensors device: OTA over ESPnow, in case sensor device cannot reach AP/router
</ul>
