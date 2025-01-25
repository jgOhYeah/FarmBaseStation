/**
 * @file defines.h
 * @brief Definitions and other preprocessor and global stuff.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2024-12-19
 */
#pragma once

#include <Arduino.h>

// Networking physical layer
#ifdef USE_ETHERNET
#include <ETH.h>
#include <Network.h>
#define CONNECTION_METHOD "Ethernet"
#else
#include <WiFi.h>
#define CONNECTION_METHOD "WiFi"
#endif

// Networking application layers
#include <PubSubClient.h>
#include <ArduinoJson.h>

// LoRa
#define PJON_INCLUDE_TL
#define TL_RESPONSE_DELAY 30 // Started out at 50ms, 20ms is too short, >=30ms works. // The PICAXE microcontrollers on the other end are pretty slow.
#include <PJONThroughLora.h>

// OTA (enable or disable in credentials.h)
#ifdef OTA_ENABLE
#include <ArduinoOTA.h>
#endif

// For alarm tunes
#ifdef PIN_SPEAKER
#include <TunePlayer.h>
#endif

#if defined(GENERATE_TIMESERIES) && defined(USE_BMP180)
#include <Adafruit_BMP085.h>
#endif

#define VERSION "0.2.6"

#define SERIAL_BAUD 115200 // Same as the bootloader.

#define MQTT_TOPIC_SEPARATOR '/'

#define RECONNECT_DELAY 1000
#define MQTT_RETRY_ITERATIONS 20
#define WIFI_RECONNECT_ATTEMPT_TIME 60000 // If not connected in 1 minute, disconnect and attempt again.

// Logging (with mutexes)
#define SERIAL_TAKE() xSemaphoreTake(serialMutex, portMAX_DELAY)
#define SERIAL_GIVE() xSemaphoreGive(serialMutex)
#define LOGV(tag, format, ...)            \
    SERIAL_TAKE();                        \
    log_v("[" tag "] " format, ##__VA_ARGS__); \
    SERIAL_GIVE()
#define LOGD(tag, format, ...)            \
    SERIAL_TAKE();                        \
    log_d("[" tag "] " format, ##__VA_ARGS__); \
    SERIAL_GIVE()
#define LOGI(tag, format, ...)            \
    SERIAL_TAKE();                        \
    log_i("[" tag "] " format, ##__VA_ARGS__); \
    SERIAL_GIVE()
#define LOGW(tag, format, ...)            \
    SERIAL_TAKE();                        \
    log_w("[" tag "] " format, ##__VA_ARGS__); \
    SERIAL_GIVE()
#define LOGE(tag, format, ...)            \
    SERIAL_TAKE();                        \
    log_e("[" tag "] " format, ##__VA_ARGS__); \
    SERIAL_GIVE()

// https://gcc.gnu.org/onlinedocs/gcc-4.8.5/cpp/Stringification.html
#define xstringify(s) stringify(s)
#define stringify(s) #s

#define MAX_ID_TEXT_LENGTH 11
#define MAX_JSON_TEXT_LENGTH 200
#define MAX_TOPIC_LENGTH 50
#define STRINGS_MATCH(A, B) (strcmp(A, B) == 0)

#ifdef DISABLE_PJON
#define PJON_DEVICE_ID 254
#else
#define PJON_DEVICE_ID 255
#endif

#define LORA_LED_FLASH_TIME 100

// Pins
// LORA pins are specified in platformio.ini
// LEDs (Defined in platformio.ini. If not defined, these LEDs will not be used (LED_BUILTIN will always be used).
// Speaker (Defined in platformio.ini. If not defined, no audio capabilities will be included).

#define LORA_CHECK_INTERVAL 30000
#define LORA_TX_INTERVAL 10000
#define LORA_MAX_PACKET_SIZE 50

#include "src/topics.h"