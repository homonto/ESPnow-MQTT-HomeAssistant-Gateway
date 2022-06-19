# ESPnow-MQTT-HomeAssistant-Gateway
ESPnow MQTT Home Assistant Gateway

Idea:
<ul>
  <li>build universal sensor device equipped with temperature, humidity and light sensor</li>
  <li>add battery monitoring capability
  <li>sensor wakes up, measures the environment and battery, sends to gateway over ESPnow, goes to sleep
  <li>gateway (always ON) receives message over ESPnow, converts to Home Assistant auto discovery code and sends to Home Assistant over MQTT
</ul>

To satisfy the requirements I've chosen:
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

Sensor Device on lovelace dashboard:

<img width="400" alt="Screenshot 2022-06-19 at 12 43 51" src="https://user-images.githubusercontent.com/46562447/174488029-645ff458-5a33-4814-8637-d4f40de59a2d.png">

Sensor device as discovered by MQTT:

<img width="809" alt="Screenshot 2022-06-19 at 12 43 32" src="https://user-images.githubusercontent.com/46562447/174488031-cf575458-4a8f-4193-bfaf-33d7e14fd2a3.png">
