/**
 * @file ota.cpp
 * @brief Handles over the air (OTA) updates.
 *
 * @author Jotham Gates (koyugaDev@gmail.com)
 * @version 0.0.1
 * @date 2024-01-12
 */
#include "ota.h"
extern SemaphoreHandle_t serialMutex;
extern SemaphoreHandle_t stateUpdateMutex;
extern NetworkState networkState;
extern bool otaUpdating;
extern TaskHandle_t ledTaskHandle;

#ifdef OTA_ENABLE

#define SET_OTA_STATE(STATE)                         \
    xSemaphoreTake(stateUpdateMutex, portMAX_DELAY); \
    otaUpdating = STATE;                             \
    xSemaphoreGive(stateUpdateMutex);                \
    xTaskNotifyGive(ledTaskHandle) // Tell the led task something changed.

void OTAManager::setupOTA()
{
    LOGD("OTA", "Waiting for network");

    // Wait until the network is connected before initialising.
    NetworkState networkStateCopy;
    do
    {
        xSemaphoreTake(stateUpdateMutex, portMAX_DELAY);
        networkStateCopy = networkState;
        xSemaphoreGive(stateUpdateMutex);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while (networkStateCopy != NETWORK_CONNECTED && networkStateCopy != NETWORK_MQTT_CONNECTING);

    LOGD("OTA", "OTA initialising");

    // Set config
    ArduinoOTA.setPort(OTA_PORT);
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    // Set callbacks
    ArduinoOTA
        .onStart(otaStartCallback)
        .onEnd(otaEndCallback)
        .onProgress(otaProgressCallback)
        .onError(otaErrorCallback);

    // Start
    ArduinoOTA.begin();
}

void OTAManager::otaStartCallback()
{
    if (ArduinoOTA.getCommand() == U_FLASH)
    {
        LOGI("OTA", "Sketch updating");
    }
    else
    {
        // U_SPIFFS
        LOGI("OTA", "Filesystem updating");
    }
    SET_OTA_STATE(true);
}

void OTAManager::otaEndCallback()
{
    LOGI("OTA", "Update finished");
    SET_OTA_STATE(false);
}

void OTAManager::otaProgressCallback(unsigned int progress, unsigned int total)
{
    LOGD("OTA", "Progress: (%u/%u) %u%%", progress, total, (progress / (total / 100)));
}

void OTAManager::otaErrorCallback(ota_error_t error)
{
    SERIAL_TAKE();
    ESP_LOGE("OTA", "Error[%u]: ", error);
    switch (error)
    {
    case OTA_AUTH_ERROR:
        ESP_LOGE("OTA", "Auth failed");
        break;
    case OTA_BEGIN_ERROR:
        ESP_LOGE("OTA", "Begin failed");
        break;
    case OTA_CONNECT_ERROR:
        ESP_LOGE("OTA", "Connect failed");
        break;
    case OTA_RECEIVE_ERROR:
        ESP_LOGE("OTA", "Receive failed");
        break;
    case OTA_END_ERROR:
        ESP_LOGE("OTA", "End failed");
        break;
    default:
        ESP_LOGE("OTA", "Other error");
    }
    SERIAL_GIVE();
}

#endif