/**
 * @file ESP32BaseStation.ino
 * @brief Main file for a base station to connect to remote sensors on a farm.
 * 
 * This is a base station to convert between PJON through LoRa to Thingsboard
 * MQTT.
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-08
 */
#include "defines.h"

WiFiClient wifi;
PubSubClient mqtt(wifi);

// QueueHandle_t rpcQueue;
// QueueHandle_t ioQueue;
// SemaphoreHandle_t hwsMutex;
SemaphoreHandle_t mqttMutex;
SemaphoreHandle_t serialMutex;

#include "device_list.h"
#include "src/networking.h"

void setup() {
    // pinMode(PIN_HWS, OUTPUT);
    // digitalWrite(PIN_HWS, LOW);

    // Setup queues and mutexes
    // rpcQueue = xQueueCreate(CONCURRENT_RPC_CALLS, sizeof(RpcMessage));
    // ioQueue = xQueueCreate(CONCURRENT_RPC_CALLS, sizeof(IoInstruction));
    // hwsMutex = xSemaphoreCreateMutex();
    mqttMutex = xSemaphoreCreateMutex();
    serialMutex = xSemaphoreCreateMutex(); // Needs to be created before logging anything.

    // Serial.begin(SERIAL_BAUD); // Already running from the bootloader.
    Serial.setDebugOutput(true);
    LOGI("Setup", "Farm PJON LoRa base station v" VERSION ".");

    if (!mqttMutex || !serialMutex) {
        LOGE("Setup", "Could not create something!!!");
    }

    xTaskCreatePinnedToCore (
        networkingTask,
        "Networking",
        4096,
        NULL,
        1,
        NULL,
        1
    );
    
    vTaskDelete(NULL); // Don't need the loop, so can remove the main Arduino task.
}

void loop() {
    // Not used
}