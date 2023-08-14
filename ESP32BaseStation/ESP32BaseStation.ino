/**
 * @file ESP32BaseStation.ino
 * @brief Main file for a base station to connect to remote sensors on a farm.
 *
 * This is a base station to convert between PJON through LoRa to Thingsboard
 * MQTT.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */
#include "defines.h"

WiFiClient wifi;
PubSubClient mqtt(wifi);
PJONThroughLora bus(PJON_DEVICE_ID);
QueueHandle_t alarmQueue;
SemaphoreHandle_t loraMutex;
SemaphoreHandle_t mqttMutex;
SemaphoreHandle_t serialMutex;

#include "device_list.h"
#include "src/networking.h"
#include "src/lora.h"
#include "src/rpc.h"
#include "src/alarm.h"

void setup()
{
    // Setup queues and mutexes
    alarmQueue = xQueueCreate(3, sizeof(AlarmState));
    mqttMutex = xSemaphoreCreateMutex();
    serialMutex = xSemaphoreCreateMutex(); // Needs to be created before logging anything.
    loraMutex = xSemaphoreCreateMutex();

    // Serial.begin(SERIAL_BAUD); // Already running from the bootloader.
    Serial.setDebugOutput(true);
    LOGI("Setup", "Farm PJON LoRa base station v" VERSION ".");

    if (!alarmQueue || !mqttMutex || !serialMutex || !loraMutex)
    {
        LOGE("SETUP", "Could not create something!!!");
    }

    // Create tasks
    xTaskCreatePinnedToCore(
        networkingTask,
        "Networking",
        4096,
        NULL,
        1,
        NULL,
        1);

    // xTaskCreatePinnedToCore(
    //     fakeReceiveTask,
    //     "FakeData",
    //     4096,
    //     NULL,
    //     1,
    //     NULL,
    //     1);

    xTaskCreatePinnedToCore(
        pjonTask,
        "PJON",
        4096,
        NULL,
        1,
        NULL,
        1);

    xTaskCreatePinnedToCore(
        alarmTask,
        "Alarm",
        4096,
        NULL,
        1,
        NULL,
        1);

    // Don't need the loop, so can remove the main Arduino task.
    vTaskDelete(NULL);
}

void loop()
{
    // Not used.
    // TODO: OTA updates
}