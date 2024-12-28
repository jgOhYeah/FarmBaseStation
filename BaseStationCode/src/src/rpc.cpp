/**
 * @file rpc.cpp
 * @brief Handles RPC calls.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */

#include "rpc.h"

extern PubSubClient mqtt;
extern QueueHandle_t mqttPublishQueue;
extern SemaphoreHandle_t mqttMutex;
extern SemaphoreHandle_t serialMutex;
extern QueueHandle_t alarmQueue;
extern DeviceManager deviceManager;

void mqttReceived(char *topic, byte *message, unsigned int length)
{
    // Check that the message isn't too long.
    if (length >= MAX_JSON_TEXT_LENGTH)
    {
        LOGW("MQTT", "Got a %d character message that was longer than the %d supported. Discarding", length, MAX_JSON_TEXT_LENGTH);
        return;
    }

    // Convert to a char array.
    char messageString[MAX_JSON_TEXT_LENGTH];
    memcpy(messageString, message, length);
    messageString[length] = 0;
    LOGD("MQTT", "MQTT received on topic '%s', '%s'.", topic, messageString);

    // Send to the appropriate handler.
    if (startsWith(topic, Topic::RPC_GATEWAY))
    {
        LOGD("MQTT", "MQTT message is RPC for a connected device.");
        rpcGateway(message, length);
    }
    else if (startsWith(topic, Topic::RPC_ME))
    {
        LOGD("MQTT", "MQTT message is RPC me.");
        char id[MAX_ID_TEXT_LENGTH];
        strncpy(id, strrchr(topic, '/')+1, MAX_ID_TEXT_LENGTH);
        rpcMe(id, message, length);
    }
}

void rpcMe(char *id, uint8_t *message, uint16_t length)
{
    JsonDocument json;
    DeserializationError error = deserializeJson(json, message, length);
    if (error)
    {
        LOGW("MQTT", "Could not deserialise the json document. Discarding");
        return;
    }

    // Handle each known method
    const char* method = json["method"];
    if (method && STRINGS_MATCH(method, "alarm"))
    {
        // Alarms
        LOGI("MQTT", "Alarm method");

        // Extract the alarm level
        // TODO: Track individual alarms.
        const char* params = json["params"];
        AlarmState state = ALARM_OFF;
        if (params)
        {
            // Might not be any params provided.
            if (STRINGS_MATCH(params, "Critical"))
            {
                state = ALARM_HIGH;
            }
            else if (STRINGS_MATCH(params, "Major"))
            {
                state = ALARM_HIGH;
            }
            else if (STRINGS_MATCH(params, "Minor"))
            {
                state = ALARM_MEDIUM;
            }
        }

        // Add to the queue
        const unsigned int ALARM_TIMEOUT = 3000;
        xQueueSend(alarmQueue, (void*)&state, ALARM_TIMEOUT / portTICK_PERIOD_MS);
        replyMeRpc(id, (char*)"{}");
    } else if (method && STRINGS_MATCH(method, "reset"))
    {
        // Reset
        LOGI("MQTT", "Reset method. Restarting in a few seconds");
        replyMeRpc(id, (char*)"{}");
        delay(10000);
        ESP.restart();
    } else {
        LOGI("MQTT", "Unrecognised MQTT method '%s' for me.", method);
    }

}

void replyMeRpc(char *id, char *reply)
{
    // Build the topic
    size_t rpcBaseLength = strlen(Topic::RPC_ME_RESPOND);
    char topic[rpcBaseLength + MAX_ID_TEXT_LENGTH];
    strcpy(topic, Topic::RPC_ME_RESPOND);
    memcpy(topic + rpcBaseLength, id, MAX_ID_TEXT_LENGTH);

    // Publish
    LOGD("MQTT", "Replying to topic %s", topic);
    // xSemaphoreTake(mqttMutex, portMAX_DELAY);
    mqtt.publish(topic, reply);
    // xSemaphoreGive(mqttMutex);
}

void rpcGateway(uint8_t *message, uint16_t length)
{
    // Deserialise
    JsonDocument json;
    DeserializationError error = deserializeJson(json, message, length);
    if (error)
    {
        LOGW("MQTT", "Could not deserialise the json document. Discarding.");
        return;
    }

    // Find the device this command is for.
    const char *deviceName = json["device"];
    if (!deviceName)
    {
        LOGW("MQTT", "Device not provided. Discarding.");
        return;
    }
    Device *device = deviceManager.getWithName(deviceName);
    if (!device)
    {
        LOGW("MQTT", "Device '%s' not recognised. Discarding.", deviceName);
        return;
    }

    // Find the field.
    JsonObject data = json["data"];
    if (!data)
    {
        LOGW("MQTT", "No data provided.");
        return;
    }
    const char *method = data["method"];
    LOGD("MQTT", "Method is %s", method);
    if (!method)
    {
        LOGW("MQTT", "Method or data not provided.");
        return;
    }
    Field *field = device->fields.getWithName(method);
    if (!field)
    {
        LOGW("MQTT", "Field not recognised.");
        return;
    }
    else
    {
        LOGD("MQTT", "Field is '%s'", field->name);
    }

    // Handle the RPC call
    JsonDocument reply;
    JsonObject replyData = reply["data"].to<JsonObject>();
    field->handleRpc(data, replyData);

    // Add the other metadata and send the reply
    reply["id"] = data["id"];
    reply["device"] = deviceName;

    char replyText[MAX_JSON_TEXT_LENGTH];
    serializeJson(reply, replyText, MAX_JSON_TEXT_LENGTH);

    LOGD("MQTT", "Replying with payload '%s'", replyText);
    // xSemaphoreTake(mqttMutex, portMAX_DELAY);
    mqtt.publish(Topic::RPC_GATEWAY, replyText);
    // xSemaphoreGive(mqttMutex);
}

bool startsWith(char *input, const char *compare)
{
    for (int i = 0; compare[i] != '\00'; i++)
    {
        if (input[i] != compare[i])
        {
            return false;
        }
    }
    return true;
}

void setAttributeState(const char* const attribute, bool state)
{
    MqttMsg msg{Topic::ATTRIBUTE_ME_UPLOAD, ""};
    JsonDocument json;
    json[attribute] = state;
    serializeJson(json, msg.payload, MAX_JSON_TEXT_LENGTH);
    xQueueSend(mqttPublishQueue, (void *)&msg, portMAX_DELAY);
}

void setVersionAttribute()
{
    MqttMsg msg{Topic::ATTRIBUTE_ME_UPLOAD, ""};
    JsonDocument json;
    JsonObject version = json["version"].to<JsonObject>();
    version["date"] = __DATE__;
    version["time"] = __TIME__;
    version["version"] = VERSION;
    version["method"] = CONNECTION_METHOD;
    version["pio-env"] = PIO_ENV;
    version["pio-plat"] = PIO_PLATFORM;
    version["pio-plat-vers"] = PIO_PLATFORM_VERSION;
    version["pio-framework"] = PIO_FRAMEWORK;
    serializeJson(json, msg.payload, MAX_JSON_TEXT_LENGTH);
    xQueueSend(mqttPublishQueue, (void*)&msg, portMAX_DELAY);
}
