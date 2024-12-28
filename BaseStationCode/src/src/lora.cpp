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
extern QueueHandle_t mqttPublishQueue;
extern SemaphoreHandle_t serialMutex;
extern SemaphoreHandle_t mqttMutex;
extern PubSubClient mqtt;
extern SemaphoreHandle_t loraMutex;
extern TaskHandle_t ledTaskHandle;
extern SemaphoreHandle_t stateUpdateMutex;
extern uint32_t lastLoRaTime;
extern void setAttributeState(const char *const attribute, bool state);

void pjonReceive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packetInfo, int rssi, float snr)
{
    // Get the device, decode the payload and add it to the json object.
    LOGD("PJON", "Received a packet.");
    Device *device = deviceManager.getWithSymbol((char)(packetInfo.tx.id));
    if (device)
    {
        // Decode
        JsonDocument json;
        device->decodePacketFields(payload, length, json, rssi, snr);

        // Convert to a string
        MqttMsg msg{Topic::TELEMETRY_UPLOAD, ""};
        serializeJson(json, msg.payload, MAX_JSON_TEXT_LENGTH);

        // Log and send to console
        xQueueSend(mqttPublishQueue, (void *)&msg, portMAX_DELAY);
    }
    else
    {
        LOGI("LORA", "Received packet for unkown device '%d'. Discarding.", packetInfo.tx.id);
    }
    xSemaphoreTake(stateUpdateMutex, portMAX_DELAY);
    lastLoRaTime = millis();
    xSemaphoreGive(stateUpdateMutex);
    xTaskNotifyGive(ledTaskHandle); // Tell the led task something changed.
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

    default:
        LOGE("PJON", "There was an unkown error (code %d, data %d).", code, data);
    }
}

void fakeReceiveTask(void *pvParameters)
{
    const uint32_t TIME_BETWEEN = 20000;
    while (true)
    {
        LOGI("LORA", "Receiving fake pump data");
        PJON_Packet_Info info;
        info.rx.id = 255;
        info.tx.id = 0x5A; // Pressure pump
        uint8_t pressurePump[] = {80, 26, 0, 97, 51, 0, 99, 1, 0};
        pjonReceive(pressurePump, sizeof(pressurePump) / sizeof(uint8_t), info, -90, 10);

        delay(TIME_BETWEEN);

        LOGI("LORA", "Receiving fake pump data 2");
        uint8_t pressurePump2[] = {80, 58, 0, 97, 57, 0, 99, 2, 0};
        pjonReceive(pressurePump2, sizeof(pressurePump2) / sizeof(uint8_t), info, -87, 5);

        delay(TIME_BETWEEN);

        LOGI("LORA", "Receiving fake fence data");
        info.tx.id = 0x4A; // Fence
        uint8_t electricFence[] = {86, 122, 0, 84, 21, 0, 70, 1, 114, 1, 73, 10};
        pjonReceive(electricFence, sizeof(electricFence) / sizeof(uint8_t), info, -20, 25);

        delay(TIME_BETWEEN);

        LOGI("LORA", "Receiving fake fence data 2");
        uint8_t electricFence2[] = {86, 123, 0, 84, 244, 255, 70, 1, 114, 1, 73, 10};
        pjonReceive(electricFence2, sizeof(electricFence2) / sizeof(uint8_t), info, -150, -25);

        delay(TIME_BETWEEN);
    }
}

void pjonTask(void *pvParameters)
{
    // Setup LoRa and PJON
    LOGD("LORA", "Starting bus");
    xSemaphoreTake(loraMutex, portMAX_DELAY);
    bus.set_acknowledge(false);
    bus.set_communication_mode(PJON_SIMPLEX);
    bus.set_receiver(pjonReceive);
    bus.set_error(pjonError);
    // LoRa.setSPIFrequency(4E6);
    SPI.begin(PIN_LORA_SCLK, PIN_LORA_MISO, PIN_LORA_MOSI);
    bus.strategy.setPins(PIN_LORA_CS, PIN_LORA_RESET, PIN_LORA_DIO);
    bus.strategy.setFrequency(433E6); // Calls LoRa.begin()
    bus.strategy.setSpreadingFactor(9);
    bus.begin();
    xSemaphoreGive(loraMutex);

    xTaskCreatePinnedToCore( // TODO: Create this task with all the others.
        loraWatchdogTask,
        "LoRa Watchdog",
        2024,
        NULL,
        1,
        NULL,
        1);

    while (true)
    {
        // Check if we have to send or receive anything
        xSemaphoreTake(loraMutex, portMAX_DELAY);
        bus.update();
        bus.receive();
        xSemaphoreGive(loraMutex);
        vTaskDelay(1);
    }
}

void loraWatchdogTask(void *pvParameters)
{
    // Inform the server whether the radio is connected.
    sendRadioConnectedMsg(LoRa.isConnected());

    // Main loop.
    TickType_t lastWakeTime;
    while (true)
    {
        // Check if the radio is connected
        xSemaphoreTake(loraMutex, portMAX_DELAY);
        bool connected = LoRa.isConnected();
        xSemaphoreGive(loraMutex);
        if (!connected)
        {
            // Not connected. We have issues.
            LOGE("LORA_WATCHDOG", "LoRa radio is not connected. Resetting");
            sendRadioConnectedMsg(false);
            delay(10000);
            ESP.restart();
        }

        xTaskDelayUntil(&lastWakeTime, LORA_CHECK_INTERVAL / portTICK_PERIOD_MS);
    }
}

void loraTxTask(void *pvParameters)
{
    bool previousTxState = false;
    sendTxWaitingMsg(false);
    while (true)
    {
        for (uint8_t i = 0; i < deviceManager.count; i++)
        {
            // For each device, check if we need to send a packet.
            Device *device = deviceManager.items[i];
            if (device->rpcWaiting())
            {
                // Need to send something.
                LOGD("LORA_TX", "Sending packet to '%s'.", deviceManager.items[i]->name);
                uint8_t payload[LORA_MAX_PACKET_SIZE];
                int8_t length = device->generatePacket(payload, LORA_MAX_PACKET_SIZE);
                if (length != FIELD_NO_MEMORY)
                {
                    LOGD("LORA_TX", "Successfully encoded packet of length %d:", length);
                    debugLoRaPacket(payload, length);
                    // Send
                    xSemaphoreTake(loraMutex, portMAX_DELAY);
                    bus.send(device->symbol, payload, length);
                    xSemaphoreGive(loraMutex);
                    LOGD("LORA_TX", "Packet sent");

                    // Update the send queue info.
                    if (!previousTxState)
                    {
                        // First time we have had to send something in a while.
                        previousTxState = true;
                    }
                    // Always send each TX so we know it happened.
                    sendTxWaitingMsg(true);

                    // Log the time that this was sent.
                    xSemaphoreTake(stateUpdateMutex, portMAX_DELAY);
                    lastLoRaTime = millis();
                    xSemaphoreGive(stateUpdateMutex);
                    xTaskNotifyGive(ledTaskHandle); // Tell the led task something changed.

                    // Wait for a while between transmissions to allow a reply / fairer spectrum usage.
                    for (uint i = 0; i < LORA_TX_INTERVAL / 10; i++)
                    {
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                        // Send a no queue message when needed.
                        previousTxState = sendOnTxNotRequired(previousTxState);
                    }
                }
                else
                {
                    // Couldn't encode
                    LOGE("LORA_TX", "Ran out of memory to encode packet. Will not send.");
                }
            }
        }
        vTaskDelay(10);

        // Send a no queue message when needed.
        previousTxState = sendOnTxNotRequired(previousTxState);
    }
}

void debugLoRaPacket(uint8_t *payload, uint8_t length)
{
    SERIAL_TAKE();
    Serial.print('{');
    if (length > 0)
    {
        Serial.print(payload[0]);
        if (length > 1)
        {
            for (uint8_t i = 1; i < length; i++)
            {
                Serial.print(", ");
                Serial.print(payload[i]);
            }
        }
    }
    Serial.println('}');
    SERIAL_GIVE();
}

void sendRadioConnectedMsg(bool state)
{
    setAttributeState("radioConnected", state);
}

void sendTxWaitingMsg(bool state)
{
    setAttributeState("txWaiting", state);
}

bool sendOnTxNotRequired(bool previous)
{
    if (previous && !deviceManager.txRequired())
    {
        // Was sending something, but now don't have to.
        sendTxWaitingMsg(false);
        return false;
    }

    // No change.
    return previous;
}