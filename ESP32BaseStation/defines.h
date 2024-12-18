/**
 * @file defines.h
 * @brief Definitions and other preprocessor and global stuff.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2024-12-19
 */
#pragma once
#include "credentials.h"

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define PJON_INCLUDE_TL
#include <PJONThroughLora.h>

// OTA (enable or disable in credentials.h)
#ifdef OTA_ENABLE
#include <ArduinoOTA.h>
#endif

// For alarm tunes
#include <TunePlayer.h>

#define VERSION "0.2.2"

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
    ESP_LOGV(tag, format, ##__VA_ARGS__); \
    SERIAL_GIVE()
#define LOGD(tag, format, ...)            \
    SERIAL_TAKE();                        \
    ESP_LOGD(tag, format, ##__VA_ARGS__); \
    SERIAL_GIVE()
#define LOGI(tag, format, ...)            \
    SERIAL_TAKE();                        \
    ESP_LOGI(tag, format, ##__VA_ARGS__); \
    SERIAL_GIVE()
#define LOGW(tag, format, ...)            \
    SERIAL_TAKE();                        \
    ESP_LOGW(tag, format, ##__VA_ARGS__); \
    SERIAL_GIVE()
#define LOGE(tag, format, ...)            \
    SERIAL_TAKE();                        \
    ESP_LOGE(tag, format, ##__VA_ARGS__); \
    SERIAL_GIVE()

// https://gcc.gnu.org/onlinedocs/gcc-4.8.5/cpp/Stringification.html
#define xstringify(s) stringify(s)
#define stringify(s) #s

#define MAX_ID_TEXT_LENGTH 11
#define MAX_JSON_TEXT_LENGTH 200
#define MAX_TOPIC_LENGTH 50
#define STRINGS_MATCH(A, B) (strcmp(A, B) == 0)

#define PJON_DEVICE_ID 255
#define LORA_LED_FLASH_TIME 100

// Pins
// LoRa
// VSPI is default without specifying
#define PIN_LORA_CS 5
#define PIN_LORA_MOSI 23
#define PIN_LORA_MISO 19
#define PIN_LORA_SCLK 18
#define PIN_LORA_DIO 17
#define PIN_LORA_RESET 16

// LEDs
#define PIN_LED_TOP 32
#define PIN_LED_INSIDE 33
// #define PIN_WIFI_LED LED_BUILTIN
#define PIN_WIFI_LED PIN_LED_INSIDE

// Speaker
#define PIN_SPEAKER 25

#define LORA_CHECK_INTERVAL 30000
#define LORA_TX_INTERVAL 10000
#define LORA_MAX_PACKET_SIZE 50

#include "src/topics.h"