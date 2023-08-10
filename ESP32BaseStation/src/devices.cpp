/**
 * @file devices.cpp
 * @brief Devices and manager for all known devices the base station connects
 * to.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-09
 */
#include "devices.h"

extern SemaphoreHandle_t serialMutex;
extern PubSubClient mqtt;

void DeviceManager::connectDevices()
{
    // For each device, connect it.
    for (uint8_t i = 0; i < m_count; i++)
    {
        // Generate a json object with everything required.
        StaticJsonDocument<MAX_JSON_TEXT_LENGTH> json;
        json["device"] = m_items[i].name;
        char charBuff[MAX_JSON_TEXT_LENGTH];
        serializeJson(json, charBuff, sizeof(charBuff));

        // Do the sending.
        // xSemaphoreTake(mqttMutex, portMAX_DELAY);
        LOGI("DEVICES", "Registering %s", charBuff);
        mqtt.publish(Topic::DEVICE_CONNECT, charBuff);
        // xSemaphoreGive(mqttMutex);
    }
}