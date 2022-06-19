# ESPnow MQTT HomeAssistant Gateway

Idea:
<ul>
  <li>build universal sensor device equipped with temperature, humidity and light sensor that can live <b>LOOOONG</b> on 1 battery charge</li>
  <li>add battery monitoring capability
  <li>Sensor's tasks: wake up, measure the environment and battery, send to gateway over ESPnow, goe to sleep, wake up after specified period and repeat
  <li>Gateway's tasks: (always ON) receive message over ESPnow from sensors, convert to Home Assistant auto discovery code and send to Home Assistant over MQTT
</ul>

# To satisfy the requirements I've chosen:
<ul>
  <li>SHT31 temperature and humidity sensor, SDA
  <li>TSL2561 light sensor, SDA
  <li>MAX17048 LiPo battery sensor, SDA
</ul>

To minimize the sleep current, the power for all sensors is drawn from one of the sensor's GPIO, so during the sleep time there is no current leakage.

Sensor is powered with LiPo battery and equipped with TP4056 usb-c charger.
Due to the low overall power consumption, the battery is capable of delivering the power for long time, however sensor can be connected to solar panel or charged from time to time using usb-c charger.

Sensor also provides the information about charging status:
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

To perform firmware upgrade there are 2 possibilities - both are with web server in use, where you store the binary file (sender.ino.esp32.bin)
<ul>
  <li>double reset click - built in library recognises double reset and if so, performs firmware upgrade - of course you need to visit the sensor to do it
  <li>routine check new firmware availability on the server: every 24h (configurable) sensors connects to server and if new file found, performs upgrade
</ul>

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

Sensor Device on lovelace dashboard:

<img width="400" alt="Screenshot 2022-06-19 at 12 43 51" src="https://user-images.githubusercontent.com/46562447/174488029-645ff458-5a33-4814-8637-d4f40de59a2d.png">

Sensor device as discovered by MQTT:

<img width="809" alt="Screenshot 2022-06-19 at 12 43 32" src="https://user-images.githubusercontent.com/46562447/174488031-cf575458-4a8f-4193-bfaf-33d7e14fd2a3.png">
