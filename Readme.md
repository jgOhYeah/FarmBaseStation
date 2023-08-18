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

## Fun part
I originally planned to use the build in DAC of the ESP32 to play sounds on alarm conditions. At this point I haven't succeded in this (currently using a slightly modified version of the [TunePlayer](https://github.com/jgOhYeah/TunePlayer) library to play simple monotonic songs instead). I did come across the [ESP32-A2DP](https://github.com/pschatzmann/ESP32-A2DP) library that allows the use of this unit as a terrible sounding bluetooth speaker.

[This sketch](Fun/BluetoothSpeaker/BluetoothSpeaker.ino) is the `bt_music_receiver_to_internal_dac` example from the ESP32-A2DP library with the addition of flashing lights.