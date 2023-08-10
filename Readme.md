# Base station for connecting remote sensors to Thingsboard.
Remote sensors use [PJON over LoRa](https://github.com/gioblu/PJON) radio to communicate.
The [Thingsboard Gateway MQTT API](https://thingsboard.io/docs/reference/gateway-mqtt-api/) is used to connect on the Thingsboard side.

See [here](https://github.com/jgOhYeah/Farm-PJON-LoRa-network) for other sensors and more information on the system.

Written by Jotham Gates
August 2023

## Getting started
Create a copy of [`credentials.example.h`](./ESP32BaseStation/credentials.example.h) and
name this `credentials.h`. Fill out the settings in this file for the WiFi network to connect to,
MQTT broker and Thingsboard credentials.