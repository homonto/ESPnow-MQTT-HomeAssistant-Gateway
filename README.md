<h1> ESPnow MQTT HomeAssistant Gateway </h1>

<h2> Idea:</h2>
<ul>
  <li>Build universal sensor device equipped with temperature, humidity and light sensor that can live <b>LOOOONG</b> on 1 battery charge</li>
  <li>Deploy as many sensors as needed (indoor, outdoor etc.) - all battery powered
  <li>Build gateway device (that is always powered ON) that transfers data from sensor devices to Home Assistant
</ul>

<h3>Tasks for the devices:</h3>
<ul>
  <li>Sensor devices tasks: wake up, measure the environment and battery, send to gateway over ESPnow, go to sleep, wake up after specified period and repeat
  <li>Gateway device tasks: (always ON) receive message over ESPnow from sensors, convert to Home Assistant auto discovery code and send to Home Assistant over MQTT
</ul>

<h3> To satisfy the requirements I've chosen: </h3>
<ul>
  <li>SHT31 temperature and humidity sensor, SDA, around 1.5$ on Aliexpress (breakout)
  <li>TSL2561 light sensor, SDA, around 1.5$ on Aliexpress (breakout)
  <li>MAX17048 LiPo battery sensor, SDA, 1$-5$ (depending if chip only or breakout)
  <li>ESP32S-WROOM or ESP32-S2 WROOM, around 2$ on Aliexpress (bare metal - <b>not the development board</b> with additional components!)
  <li>few additional components as per schematics (see below)
</ul>

<img width="1056" alt="s" src="https://user-images.githubusercontent.com/46562447/174493436-59580695-339b-47a6-a3c7-aca9be8a14dc.png">
<img width="1030" alt="s2" src="https://user-images.githubusercontent.com/46562447/174493450-0c83bded-bc4d-4e2b-81a3-2a97067c947f.png">


<h2>Power consumption and management</h2>
To minimise the sleep current, the power for all sensors is drawn from one of the sensor's GPIO, so during the sleep time there is no current leakage.
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

<b>ESP32-S WROOM working time:</b>
<img width="1635" alt="s working time 1-2022-06-17 at 20 33 04" src="https://user-images.githubusercontent.com/46562447/174492638-ced6a7ae-1e08-4c57-b3ef-5d4f652dbff0.png">

<b>ESP32-S2 WROOM working time:</b>
<img width="1667" alt="s2 working time 1-2022-06-18 at 12 51 43" src="https://user-images.githubusercontent.com/46562447/174492694-d02a87da-8b9a-471e-8958-c4b7a4ad815b.png">



And that is what really matters with the battery life time calculation, because 3 times shorter working time makes the difference.<br>
<h4>Rough calculation (1000mAh battery, reserve capacity=20%, working time as above, sleep time=180s) shows as following:</h4>
<ul>
  <li>ESP32-S  WROOM: 4 000 hours with average 80uA = 166 days, 5.5 months
  <li>ESP32-S2 WROOM: 10 000 hours with average 80uA = 417 days, 13.7 months
</ul>
So apparently the winner is <b>ESP32-S2 WROOM</b>
<br>
<h3> Sensor device is powered with LiPo battery and equipped with TP4056 USB-C charger.</h3>
It can be connected to solar panel or - from time to time if possible/needed - to USB-C charger.
<br><br>
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
<h4>Sensor device - sender</h4>
Tasks are as described above so the code is in 1 file only: sender.ino <br>
Additionally there is a configuration file where you must specify details for each sensor device (such as sensors used, GPIO etc.): devices_config.h<br>
Finally the file with your credentials: passwords.h (ssid, password, webserver where your firmware is stored) - this file is only used for OTA

<h4>Gateway device - receiver</h4>
Gateway tasks are more complex (as described above) so the code is split into multiple files - per function<br>
Entire configuration is in config.h file<br>
Finally the file with your credentials: passwords.h (ssid, password, mqtt ip and credentials, webserver where your firmware is stored)
<br>

<h4>Libraries needed:</h4>
ESP_DoubleResetDetector

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
