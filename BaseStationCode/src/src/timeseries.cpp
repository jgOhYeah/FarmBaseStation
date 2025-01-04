/**
 * @file timeseries.h
 * @brief Measures the temperature and pressure and sends them every so often as timeseries.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2025-01-4
 */
#include "timeseries.h"
#include "networking.h"
extern SemaphoreHandle_t serialMutex;
extern QueueHandle_t mqttPublishQueue;

#ifdef GENERATE_TIMESERIES
#ifdef USE_BMP180
Adafruit_BMP085 bmp;
#endif

void timeseriesTask(void *pvParameters)
{
    LOGI("TS", "Starting");
#ifdef USE_BMP180
    if (!bmp.begin())
    {
        LOGE("TS", "Could not connect to BMP180");
    }
#endif
    // Measure every so often.
    while (true)
    {
        // Take readings
        JsonDocument json;
#ifdef USE_BMP180
        json["temperature"] = bmp.readTemperature();
        json["pressure"] = bmp.readPressure();
        LOGD("TS", "%dC, %fPa");
#endif
        // Send the message.
        MqttMsg msg{Topic::TELEMETRY_ME_UPLOAD, ""};
        serializeJson(json, msg.payload, MAX_JSON_TEXT_LENGTH);
        xQueueSend(mqttPublishQueue, (void *)&msg, portMAX_DELAY);

        // Wait for a while.
        vTaskDelay(300000 / portTICK_PERIOD_MS); // Wait 5 min between uploads.
    }
}
#endif