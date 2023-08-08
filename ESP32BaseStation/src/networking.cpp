/**
 * @file networking.cpp
 * @brief File that handles connecting to WiFi and MQTT and staying connected.
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-08
 */
#include "networking.h"

extern WiFiClient wifi;
extern PubSubClient mqtt;
extern SemaphoreHandle_t mqttMutex;
extern SemaphoreHandle_t serialMutex;

/**
 * @brief Connects to WiFi.
 * 
 */
void wifiConnect() {
    WiFi.disconnect();
    LOGI("Networking", "Connecting to '" WIFI_SSID "'.");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(PIN_WIFI_LED, HIGH);
        vTaskDelay(900 / portTICK_RATE_MS);
        digitalWrite(PIN_WIFI_LED, LOW);
        vTaskDelay(100 / portTICK_RATE_MS);
    }
    LOGI("Networking", "Connected with IP address '%s'.", WiFi.localIP().toString().c_str());
}

/**
 * @brief Function that is called when an mqtt message is received.
 * 
 */
void mqttReceived(char* topic, byte* message, unsigned int length) {
    LOGD("Networking", "MQTT received on topic '%s'.", topic);
    // if(isRpc(topic)) {
    //     LOGD("Networking", "RPC Message");
    //     addRpc(topic, message, length);
    // }
    // TODO
}

/**
 * @brief Connects to the MQTT broker.
 */
void mqttConnect() {
    xSemaphoreTake(mqttMutex, portMAX_DELAY);
    LOGI("Networking", "Connecting to MQTT broker '" MQTT_BROKER "' on port " xstr(MQTT_PORT) ".");
    mqtt.setServer(MQTT_BROKER, MQTT_PORT);
    mqtt.setCallback(mqttReceived);
    mqtt.connect(THINGSBOARD_NAME, THINGSBOARD_TOKEN, NULL);
    mqtt.subscribe(TOPIC_RPC_REQUEST);
    int iterations = 0;
    while(!mqtt.connected()) {
        digitalWrite(PIN_WIFI_LED, LOW);
        vTaskDelay(900 / portTICK_RATE_MS);
        digitalWrite(PIN_WIFI_LED, HIGH);
        vTaskDelay(100 / portTICK_RATE_MS);
        
        // Check if WiFi is connected and reconnect if needed.
        if(WiFi.status() != WL_CONNECTED) {
            wifiConnect();
        }
        // Attempt to start the connection every so often.
        iterations++;
        if (iterations == MQTT_RETRY_ITERATIONS) {
            LOGD("Networking", "Having another go at connecting MQTT.");
            mqtt.connect(THINGSBOARD_NAME, THINGSBOARD_TOKEN, NULL);
            mqtt.subscribe(TOPIC_RPC_REQUEST);
            iterations = 0;
        }
    }
    LOGI("Networking", "Connected to broker.");
    xSemaphoreGive(mqttMutex);
}

/**
 * Task that manages connecting to WiFi and MQTT and remaining connected.
 */
void networkingTask(void *pvParameters) {
    pinMode(PIN_WIFI_LED, OUTPUT);
    digitalWrite(PIN_WIFI_LED, HIGH);
    
    while(true) {
        // Connect to WiFi
        if (WiFi.status() != WL_CONNECTED) {
            LOGW("Networking", "LOST WIFI CONNECTION!!!");
            vTaskDelay(RECONNECT_DELAY / portTICK_PERIOD_MS);
            wifiConnect();
            vTaskDelay(RECONNECT_DELAY / portTICK_PERIOD_MS);
            mqttConnect();
        }

        // Connect to MQTT
        if (!mqtt.connected()) {
            LOGW("Networking", "LOST MQTT CONNECTION!!!");
            vTaskDelay(RECONNECT_DELAY / portTICK_PERIOD_MS);
            mqttConnect();
        }

        // Depending on whic connect function was called, the LED may be off.
        digitalWrite(PIN_WIFI_LED, HIGH);

        // Thread safe loop.
        xSemaphoreTake(mqttMutex, portMAX_DELAY);
        mqtt.loop();
        xSemaphoreGive(mqttMutex);
        taskYIELD();
    }
}