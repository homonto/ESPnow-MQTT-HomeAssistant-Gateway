# ESPnow-MQTT-HomeAssistant-Gateway
ESPnow MQTT Home Assistant Gateway

Idea:
<ul>
  <li>build universal sensor device equipped with temperature, humidity and light sensor</li>
  <li>add battery monitoring capability
  <li>sensor wakes up, measures the environment and battery, sends to gateway over ESPnow, goes to sleep
  <li>gateway (always ON) receives message over ESPnow, converts to Home Assistant auto discovery code and sends to Home Assistant over MQTT
</ul>

This project consists of 2 parts:
<ul>
  <li>sensor device - sender</li>
  <li>gateway device - receiver</li>
</ul>


Sensor Device on lovelace dashboard:

<img width="400" alt="Screenshot 2022-06-19 at 12 43 51" src="https://user-images.githubusercontent.com/46562447/174488029-645ff458-5a33-4814-8637-d4f40de59a2d.png">

Sensor device as discovered by MQTT:

<img width="809" alt="Screenshot 2022-06-19 at 12 43 32" src="https://user-images.githubusercontent.com/46562447/174488031-cf575458-4a8f-4193-bfaf-33d7e14fd2a3.png">
