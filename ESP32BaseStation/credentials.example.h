/**
 * @file credentials.example.h
 * @author Jotham Gates
 * @brief Credentials and system specific settings.
 * @version 0.1
 * @date 2024-06-26
 *
 * Create a copy of this file and name it `crendentials.h`. Edit the settings in
 * this new file and leave `credentials.example.h` untouched.
 */
#pragma once

// WiFi settings
#define WIFI_SSID "MyWiFiNetwork"
#define WIFI_PASSWORD "MyWiFiPassword"

// MQTT settings
#define MQTT_BROKER "computername.local"
#define MQTT_PORT 1883

// Thingsboard settings
#define THINGSBOARD_TOKEN "Copy access token in the devices list"
#define THINGSBOARD_NAME "Farm PJON Gateway"

// OTA settings
#define OTA_ENABLE
#ifdef OTA_ENABLE
#define OTA_PORT 3232
#define OTA_HOSTNAME "farmgateway"
#define OTA_PASSWORD "MyOTAPassword"
#endif