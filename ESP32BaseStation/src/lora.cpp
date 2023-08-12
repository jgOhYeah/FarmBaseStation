/**
 * @file lora.cpp
 * @brief File that handles the LoRa radio and PJON.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */

#include "lora.h"

extern DeviceManager deviceManager;
extern SemaphoreHandle_t serialMutex;
extern SemaphoreHandle_t mqttMutex;
extern PubSubClient mqtt;

void pjonReceive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info)
{
    // Get the device, decode the payload and add it to the json object.
    Device *device = deviceManager.getWithSymbol((char)(packet_info.tx.id));
    if (device)
    {
        StaticJsonDocument<MAX_JSON_TEXT_LENGTH> json;
        device->decodePacketFields(payload, length, json);

        // Convert to a string
        char jsonText[MAX_JSON_TEXT_LENGTH];
        serializeJson(json, jsonText, MAX_JSON_TEXT_LENGTH);

        // Log and send to console
        LOGI("LORA", "Sending '%s'", jsonText);
        xSemaphoreTake(mqttMutex, portMAX_DELAY);
        mqtt.publish(Topic::DEVICE_CONNECT, jsonText);
        xSemaphoreGive(mqttMutex);
    }
    else
    {
        LOGI("LORA", "Received packet for unkown device '%d'. Discarding.", packet_info.tx.id);
    }
}

void fakeReceive()
{
    LOGI("LORA", "Receiving fake data for testing purposes");
    PJON_Packet_Info info;
    info.rx.id = 255;
    info.tx.id = 0x5A; // Pressure pump
    uint8_t pressurePump[] = {80, 26, 0, 97, 51, 0, 99, 1, 0};
    pjonReceive(pressurePump, sizeof(pressurePump) / sizeof(uint8_t), info);

    info.tx.id = 0x4A; // Fence
    uint8_t electricFence[] = {86, 122, 0, 84, 21, 0, 70, 1, 114, 1, 73, 10};
    pjonReceive(electricFence, sizeof(electricFence) / sizeof(uint8_t), info);
}