# Base station for connecting remote sensors to Thingsboard.
Remote sensors use [PJON over LoRa](https://github.com/gioblu/PJON) radio to communicate.
The [Thingsboard Gateway MQTT API](https://thingsboard.io/docs/reference/gateway-mqtt-api/) is used to connect on the Thingsboard side.

See [here](https://github.com/jgOhYeah/Farm-PJON-LoRa-network) for other sensors and more information on the system.

Written by Jotham Gates
August 2023

## Photos
<!-- <img src="Photos/Inside.jpg" alt="Inside the enclosure." width="50%">
<img src="Photos/Completed.jpg" alt="The completed unit." width="49%"> -->
![Inside the enclosure.](Photos/Inside.jpg)
![The completed unit.](Photos/Completed.jpg)

## Getting started
The Github repository containing all design files and code is [here](https://github.com/jgOhYeah/FarmBaseStation).

1. Install the ESP32 Arduino boards manager.
2. Install the following libraries using the Arduino library manager:
   - [`PubSubClient1`](https://github.com/knolleary/pubsubclient)
   - [`ArduinoJson`](https://arduinojson.org/)
   - [`PJON`](https://github.com/gioblu/PJON)
   - [`LoRa`](https://github.com/sandeepmistry/arduino-LoRa)
   - [`TunePlayer`](https://github.com/jgOhYeah/TunePlayer) (A library I have written).
3. Create a copy of [`credentials.example.h`](./ESP32BaseStation/credentials.example.h) and name this `credentials.h`. Fill out the settings in this file for the WiFi network to connect to,
MQTT broker and Thingsboard credentials.
4. Upload the code to the ESP32.
5. Assemble the electronics as shown in the schematics and optionally place inside a case.
6. Test with other sensors and the Thingsboard server.

## Schematics
The base station uses an ESP32 development board as its base. An SX1278 LoRa radio module breakout board is connected via jumper leads. LEDs are soldered along with their respective current limiting resistors to wires that can be plugged into the ESP32 board. An audio amplifier and speaker can also be connected for alarm functionality. I salvaged this from the sound effects module of a broken children's ride on toy car.

![The schematic for the base station](BaseStationSchematics/Exports/BaseStationSchematics.svg)

## Fun part / experiments
I originally planned to use the build in DAC of the ESP32 to play sounds on alarm conditions. At this point I haven't succeded in this (currently using the [TunePlayer](https://github.com/jgOhYeah/TunePlayer) library to play simple monotonic songs instead). I did come across the [ESP32-A2DP](https://github.com/pschatzmann/ESP32-A2DP) library that allows the use of this unit as a terrible sounding bluetooth speaker.

[This sketch](Fun/BluetoothSpeaker/BluetoothSpeaker.ino) is the `bt_music_receiver_to_internal_dac` example from the ESP32-A2DP library with the addition of flashing lights.