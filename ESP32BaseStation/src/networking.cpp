/**
 * @file networking.cpp
 * @brief File that handles connecting to WiFi and MQTT and staying connected.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */
#include "networking.h"

extern WiFiClient wifi;
extern PubSubClient mqtt;
extern QueueHandle_t mqttPublishQueue;
extern SemaphoreHandle_t mqttMutex;
extern SemaphoreHandle_t serialMutex;
extern DeviceManager deviceManager;
extern NetworkState networkState;
extern SemaphoreHandle_t stateUpdateMutex;
extern TaskHandle_t ledTaskHandle;
extern void mqttReceived(char *topic, byte *message, unsigned int length);

#define SET_NETWORK_STATE(STATE)                     \
    xSemaphoreTake(stateUpdateMutex, portMAX_DELAY); \
    networkState = STATE;                            \
    xSemaphoreGive(stateUpdateMutex)

/**
 * @brief Connects to WiFi.
 *
 */
void wifiConnect()
{
    do
    {
        WiFi.disconnect();
        LOGI("Networking", "Connecting to '" WIFI_SSID "'.");
        SET_NETWORK_STATE(NETWORK_WIFI_CONNECTING);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        for (uint32_t iterCount = 0; WiFi.status() != WL_CONNECTED && iterCount < WIFI_RECONNECT_ATTEMPT_TIME; iterCount++)
        {
            vTaskDelay(1);
        }
    } while (WiFi.status() != WL_CONNECTED);
    LOGI("Networking", "Connected with IP address '%s'.", WiFi.localIP().toString().c_str());
}

/**
 * @brief Connects to the MQTT broker.
 */
void mqttConnect()
{
    xSemaphoreTake(mqttMutex, portMAX_DELAY);
    LOGI("Networking", "Connecting to MQTT broker '" MQTT_BROKER "' on port " xstringify(MQTT_PORT) ".");
    SET_NETWORK_STATE(NETWORK_MQTT_CONNECTING);
    mqtt.setServer(MQTT_BROKER, MQTT_PORT);
    mqtt.setCallback(mqttReceived);
    mqttSetup();
    int iterations = 0;
    while (!mqtt.connected())
    {
        vTaskDelay(1000 / portTICK_RATE_MS);

        // Check if WiFi is connected and reconnect if needed.
        if (WiFi.status() != WL_CONNECTED)
        {
            wifiConnect();
            SET_NETWORK_STATE(NETWORK_MQTT_CONNECTING);
        }
        // Attempt to start the connection every so often.
        iterations++;
        if (iterations == MQTT_RETRY_ITERATIONS)
        {
            LOGD("Networking", "Having another go at connecting MQTT.");
            mqttSetup();
            iterations = 0;
        }
    }
    LOGI("Networking", "Connected to broker.");
    xSemaphoreGive(mqttMutex);
    deviceManager.connectDevices(); // Publish the connected devices
}

/**
 * @brief Subscribes to the required topics.
 *
 */
void mqttSetup()
{
    mqtt.connect(THINGSBOARD_NAME, THINGSBOARD_TOKEN, NULL);
    mqtt.subscribe(Topic::RPC_GATEWAY);
    mqtt.subscribe(Topic::RPC_ME_SUBSCRIBE);
}

/**
 * Task that manages connecting to WiFi and MQTT and remaining connected.
 */
void networkingTask(void *pvParameters)
{
    SET_NETWORK_STATE(NETWORK_NONE);
    while (true)
    {
        // Connect to WiFi
        if (WiFi.status() != WL_CONNECTED)
        {
            SET_NETWORK_STATE(NETWORK_NONE);
            LOGW("Networking", "LOST WIFI CONNECTION!!!");
            vTaskDelay(RECONNECT_DELAY / portTICK_PERIOD_MS);
            wifiConnect();
            vTaskDelay(RECONNECT_DELAY / portTICK_PERIOD_MS);
            mqttConnect(); // Force reconnection to mqtt after messing with WiFi.
        }

        // Connect to MQTT
        if (!mqtt.connected())
        {
            LOGW("Networking", "LOST MQTT CONNECTION!!!");
            vTaskDelay(RECONNECT_DELAY / portTICK_PERIOD_MS);
            mqttConnect();
        }

        // Should be connected if we reached this point.
        SET_NETWORK_STATE(NETWORK_CONNECTED);

        // Thread safe mqtt operations.
        xSemaphoreTake(mqttMutex, portMAX_DELAY);
        mqtt.loop();

        // Check if there is anything to publish
        MqttMsg msg;
        int result = xQueueReceive(mqttPublishQueue, (void *)&msg, 0);
        if (result)
        {
            // Something needs to be published.
            LOGI("Networking", "Publishing on topic '%s' message '%s'", msg.topic, msg.payload);
            mqtt.publish(msg.topic, msg.payload);
        }
        xSemaphoreGive(mqttMutex);
        taskYIELD();
    }
}