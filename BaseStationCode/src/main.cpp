/**
 * @file ESP32BaseStation.ino
 * @brief Main file for a base station to connect to remote sensors on a farm.
 *
 * This is a base station to convert between PJON through LoRa to Thingsboard
 * MQTT.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2024-12-19
 */
#include "defines.h"

#ifdef USE_ETHERNET
NetworkClient client;
PubSubClient mqtt(client);
bool ethernetConnected = false;
#else
WiFiClient wifi;
PubSubClient mqtt(wifi);
#endif
PJONThroughLora bus(PJON_DEVICE_ID);
QueueHandle_t alarmQueue;
#ifdef PIN_SPEAKER
QueueHandle_t audioQueue;
#endif
QueueHandle_t mqttPublishQueue;
SemaphoreHandle_t loraMutex;
SemaphoreHandle_t mqttMutex;
SemaphoreHandle_t serialMutex;

#include "device_list.h"
#include "src/networking.h"
#include "src/lora.h"
#include "src/rpc.h"
#include "src/alarm.h"
#include "src/audio.h"
#include "src/leds.h"
#include "src/ota.h"

// States used for LED control.
SemaphoreHandle_t stateUpdateMutex;
AlarmState alarmState;
NetworkState networkState;
uint32_t lastLoRaTime;
TaskHandle_t ledTaskHandle;
bool otaUpdating = false;

#define PIO_VERSION_STR "Env=" PIO_ENV ", Platform=" PIO_PLATFORM " (" PIO_PLATFORM_VERSION "), FRAMEWORK=" PIO_FRAMEWORK "."

void setup()
{
#ifdef PIN_SPEAKER
    // In case a reset pccured at the wrong time.
    pinMode(PIN_SPEAKER, OUTPUT);
    digitalWrite(PIN_SPEAKER, LOW);
    audioQueue = xQueueCreate(3, sizeof(AlarmState));
#endif

    // Setup queues and mutexes
    // TODO: Swap to notifications
    alarmQueue = xQueueCreate(3, sizeof(AlarmState));
    mqttPublishQueue = xQueueCreate(15, sizeof(MqttMsg));
    mqttMutex = xSemaphoreCreateMutex();
    serialMutex = xSemaphoreCreateMutex(); // Needs to be created before logging anything.
    loraMutex = xSemaphoreCreateMutex();
    stateUpdateMutex = xSemaphoreCreateMutex();

    Serial.begin(SERIAL_BAUD); // Already running from the bootloader.
    // Serial.setDebugOutput(true);
    LOGI("Setup", "Farm PJON LoRa base station v" VERSION ". Compiled " __DATE__ ", " __TIME__ ". Connecting using " CONNECTION_METHOD ".");
    LOGI("Setup", PIO_VERSION_STR);
    // Serial.println(PIO_VERSION_STR);

    if (!alarmQueue ||
#ifdef PIN_SPEAKER
        !audioQueue ||
#endif
        !mqttPublishQueue || !mqttMutex || !serialMutex || !loraMutex || !stateUpdateMutex)
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

#ifdef PIN_SPEAKER
    xTaskCreatePinnedToCore(
        audioTask,
        "Audio",
        2048,
        NULL,
        1,
        NULL,
        1);
#endif

    xTaskCreatePinnedToCore(
        loraTxTask,
        "LoRa TX",
        4096,
        NULL,
        1,
        NULL,
        1);

    xTaskCreatePinnedToCore(
        ledTask,
        "LEDs",
        2048,
        NULL,
        1,
        &ledTaskHandle,
        1);
    // TODO: Actually measure ram and high water marks rather than guessing.

#ifdef OTA_ENABLE
    // Setup OTA
    OTAManager::setupOTA();
#else
    // Don't need the loop, so can remove the main Arduino task.
    vTaskDelete(NULL);
#endif
}

void loop()
{
#ifdef OTA_ENABLE
    OTAManager::handleOTA();
    vTaskDelay(1000/portTICK_PERIOD_MS);
#endif
}