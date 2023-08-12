/**
 * @file lora.cpp
 * @brief File that handles the LoRa radio and PJON.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */

#include "lora.h"

extern PJONThroughLora bus;
extern DeviceManager deviceManager;
extern SemaphoreHandle_t serialMutex;
extern SemaphoreHandle_t mqttMutex;
extern PubSubClient mqtt;

void pjonReceive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packetInfo, int rssi, float snr)
{
    // TODO: Queue to send
    // Get the device, decode the payload and add it to the json object.
    LOGD("PJON", "Received a packet.");
    Device *device = deviceManager.getWithSymbol((char)(packetInfo.tx.id));
    if (device)
    {
        // Decode
        StaticJsonDocument<MAX_JSON_TEXT_LENGTH> json;
        device->decodePacketFields(payload, length, json, rssi, snr);

        // Convert to a string
        char jsonText[MAX_JSON_TEXT_LENGTH];
        serializeJson(json, jsonText, MAX_JSON_TEXT_LENGTH);

        // Log and send to console
        LOGI("LORA", "Sending '%s'", jsonText);
        xSemaphoreTake(mqttMutex, portMAX_DELAY);
        mqtt.publish(Topic::TELEMETRY_UPLOAD, jsonText);
        xSemaphoreGive(mqttMutex);
    }
    else
    {
        LOGI("LORA", "Received packet for unkown device '%d'. Discarding.", packetInfo.tx.id);
    }
}

void pjonReceive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packetInfo)
{
    pjonReceive(payload, length, packetInfo, bus.strategy.packetRssi(), bus.strategy.packetSnr());
}

void pjonError(uint8_t code, uint16_t data, void *customPointer)
{
    switch (code)
    {
    case PJON_CONNECTION_LOST:
        LOGW("PJON", "Lost connection with device %d.", bus.packets[data].content[0]);
        break;

    case PJON_PACKETS_BUFFER_FULL:
        LOGW("PJON", "Packet bus full with length %d.", data);
        break;

    case PJON_CONTENT_TOO_LONG:
        LOGE("PJON", "PJON content too long (length %d).", data);
        break;
    }
}

void fakeReceiveTask(void *pvParameters)
{
    while (true)
    {
        LOGI("LORA", "Receiving fake pump data");
        PJON_Packet_Info info;
        info.rx.id = 255;
        info.tx.id = 0x5A; // Pressure pump
        uint8_t pressurePump[] = {80, 26, 0, 97, 51, 0, 99, 1, 0};
        pjonReceive(pressurePump, sizeof(pressurePump) / sizeof(uint8_t), info, -90, 10);

        delay(2000);

        LOGI("LORA", "Receiving fake pump data 2");
        uint8_t pressurePump2[] = {80, 58, 0, 97, 57, 0, 99, 2, 0};
        pjonReceive(pressurePump2, sizeof(pressurePump2) / sizeof(uint8_t), info, -87, 5);

        delay(2000);

        LOGI("LORA", "Receiving fake fence data");
        info.tx.id = 0x4A; // Fence
        uint8_t electricFence[] = {86, 122, 0, 84, 21, 0, 70, 1, 114, 1, 73, 10};
        pjonReceive(electricFence, sizeof(electricFence) / sizeof(uint8_t), info, -20, 25);

        delay(2000);

        LOGI("LORA", "Receiving fake fence data 2");
        uint8_t electricFence2[] = {86, 123, 0, 84, 244, 255, 70, 1, 114, 1, 73, 10};
        pjonReceive(electricFence2, sizeof(electricFence2) / sizeof(uint8_t), info, -150, -25);

        delay(2000);
    }
}

void pjonTask(void *pvParameters)
{
    // Setup LoRa and PJON
    bus.set_acknowledge(false);
    bus.set_communication_mode(PJON_SIMPLEX);
    bus.set_receiver(pjonReceive);
    bus.set_error(pjonError);
    bus.strategy.setFrequency(433E6);
    bus.strategy.setSpreadingFactor(9);
    LoRa.setSPIFrequency(4E6);
    bus.begin();

    while (true)
    {
        // Check if we have to send or receive anything
        bus.update();
        bus.receive();
        yield();
    }
}