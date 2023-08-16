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

// For alarm tunes
#define DEFAULT_BUFFER_SIZE 100
#include "AudioTools.h"
#include "AudioCodecs/CodecMP3Helix.h"

WiFiClient wifi;
PubSubClient mqtt(wifi);
PJONThroughLora bus(PJON_DEVICE_ID);
QueueHandle_t alarmQueue;
QueueHandle_t audioQueue;
SemaphoreHandle_t loraMutex;
SemaphoreHandle_t mqttMutex;
SemaphoreHandle_t serialMutex;

const char *urls[] = {
    "DESKTOP-RVE60DS.local:8000/TardisTakeoff.mp3",
    "DESKTOP-RVE60DS.local:8000/TardisCloister.mp3"};
URLStream urlStream;
AudioSourceURL source(urlStream, urls, "audio/mp3");
AnalogAudioStream analog;
MP3DecoderHelix decoder;
AudioPlayer player(source, analog, decoder);

#include "device_list.h"
#include "src/networking.h"
#include "src/lora.h"
#include "src/rpc.h"
#include "src/alarm.h"
// #include "src/audio.h"
void audioCTask(void *pvParameters);
void setup()
{
    pinMode(PIN_LED_TOP, OUTPUT);
    pinMode(PIN_LED_INSIDE, OUTPUT);

    // Setup queues and mutexes
    alarmQueue = xQueueCreate(3, sizeof(AlarmState));
    audioQueue = xQueueCreate(3, sizeof(AlarmState));
    mqttMutex = xSemaphoreCreateMutex();
    serialMutex = xSemaphoreCreateMutex(); // Needs to be created before logging anything.
    loraMutex = xSemaphoreCreateMutex();

    // Serial.begin(SERIAL_BAUD); // Already running from the bootloader.
    Serial.setDebugOutput(true);
    LOGI("Setup", "Farm PJON LoRa base station v" VERSION ".");

    if (!alarmQueue || !audioQueue || !mqttMutex || !serialMutex || !loraMutex)
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

    xTaskCreatePinnedToCore(
        audioTask,
        "Audio",
        2048,
        NULL,
        1,
        NULL,
        1);

    xTaskCreatePinnedToCore(
        loraTxTask,
        "LoRa TX",
        2048,
        NULL,
        1,
        NULL,
        1);
    // TODO: Actually measure ram and high water marks rather than guessing.

    // Don't need the loop, so can remove the main Arduino task.
    // vTaskDelete(NULL);
}

void loop()
{
    // Not used.
    // TODO: OTA updates
    delay(10000);
    LOGD("M", "Hi");
}

// Located here for now as the library doesn't like being included from other files.
void audioTask(void *pvParameters)
{
    // begin processing
    LOGD("AUDIO", "Beginning");
    // auto cfg = analog.defaultConfig();
    // analog.begin(cfg);
    // player.begin();
    // player.setAutoNext(false);
    LOGD("AUDIO", "Entering loop");
    while (true)
    {
        // // Wait for something to play.
        LOGD("AUDIO", "Waiting for a play instruction.");
        int fileIndex = 0;
        AlarmState state;
        xQueueReceive(audioQueue, (void *)&state, portMAX_DELAY);
        switch (state)
        {
        case ALARM_HIGH:
            fileIndex = 0;
            break;
        case ALARM_MEDIUM:
            fileIndex = 1;
            break;
        }

        // Start playing
        LOGI("AUDIO", "Playing %d.", fileIndex);
        // player.setIndex(index);
        // player.play();

        // // Play the file
        // do
        // {
        //     player.copy();
        //     yield();
        // } while (player.isActive());
    }
}