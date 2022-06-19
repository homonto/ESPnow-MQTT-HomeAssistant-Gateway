<h1> ESPnow MQTT HomeAssistant Gateway </h1>

<h2> Idea:</h2>
<ul>
  <li>build universal sensor device equipped with temperature, humidity and light sensor that can live <b>LOOOONG</b> on 1 battery charge</li>
  <li>add battery monitoring capability
  <li>Sensor's tasks: wake up, measure the environment and battery, send to gateway over ESPnow, go to sleep, wake up after specified period and repeat
  <li>Gateway's tasks: (always ON) receive message over ESPnow from sensors, convert to Home Assistant auto discovery code and send to Home Assistant over MQTT
</ul>

<h3> To satisfy the requirements I've chosen: </h3>
<ul>
  <li>SHT31 temperature and humidity sensor, SDA, around 1.5$ on Aliexpress
  <li>TSL2561 light sensor, SDA, around 1.5$ on Aliexpress
  <li>MAX17048 LiPo battery sensor, SDA, 2-3$ on ebay or 5$ SparkFun
  <li>ESP32S-WROOM or ESP32-S2 WROOM, around 2$ on Aliexpress
  <li>few additional components as per schematics (see below)
</ul>
<h2>Power consumption and management</h2>
To minimise the sleep current, the power for all sensors is drawn from one of the sensor's GPIO, so during the sleep time there is no current leakage.
With the above configuration the sleep current is as following (measures with PPK2):
<ul>
  <li>ESP32-S  WROOM - 4.5uA
  <li>ESP32-S2 WROOM - 22.5uA
</ul>
You would say: "ok, ESP32-S is the winner!" but wait, working time and current are as following:
<ul>
  <li>ESP32-S  WROOM - 600ms, 50mA average
  <li>ESP32-S2 WROOM - 200ms, 38mA average
</ul>
And that is what really matters with the battery life time calculation, because 3 times shorter working time makes the difference.<br>
<h4>Rough calculation (1000mAh battery, reserve capacity=20%, working time as above, sleep time=180s) shows as following:</h4>
<ul>
  <li>ESP32-S  WROOM: 4 000 hours with average 80uA = 166 days, 5.5 months
  <li>ESP32-S2 WROOM: 10 000 hours with average 80uA = 417 days, 13.7 months
</ul>
So apparently the winner is <b>ESP32-S2 WROOM</b>
<br>
<h3> Sensor is powered with LiPo battery and equipped with TP4056 USB-C charger.</h3>
It can be connected to solar panel or - from time to time if possible - to USB-C charger.

Sensor also provides information about charging status:
<ul>
  <li>NC - not connected
  <li>ON - charging
  <li>FULL - charged
</ul>  

Information from sensor on Home Assistant:
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
  <li>double reset click - built in functions recognise double reset and if so done, performs firmware upgrade - of course you need to visit the sensor to double click it
  <li>routine check for new firmware availability on the server: every 24h (configurable) sensors connects to server and if new file found, performs upgrade
</ul>
I am using Apache minimal add on to Home Assistant - since all sensors are in the same network where Home Assistant is, <b>there is no need for internet access for sensors (and gateway).</b><br>

<h2>Configuration</h2>
All devices used in the sensor (SHT31, TSL2561, MAX17048, checking charging status) are optional and can be disabled in configuration file.
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

<h2>Home Assistant</h2>
<h4>Both device and all entities are automatically configured in Home Assistant using MQTT discovery</h4>
<br>
Sensor Device on lovelace dashboard:

<img width="400" alt="Screenshot 2022-06-19 at 12 43 51" src="https://user-images.githubusercontent.com/46562447/174488029-645ff458-5a33-4814-8637-d4f40de59a2d.png">

Sensor device as discovered by MQTT:

<img width="809" alt="Screenshot 2022-06-19 at 12 43 32" src="https://user-images.githubusercontent.com/46562447/174488031-cf575458-4a8f-4193-bfaf-33d7e14fd2a3.png">
