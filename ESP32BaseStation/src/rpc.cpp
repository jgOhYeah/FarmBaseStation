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
extern SemaphoreHandle_t mqttMutex;
extern SemaphoreHandle_t serialMutex;

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
    StaticJsonDocument<MAX_JSON_TEXT_LENGTH> json;
    DeserializationError error = deserializeJson(json, message, length);
    if (error)
    {
        LOGW("MQTT", "Could not deserialise the json document. Discarding");
        return;
    }

    // Handle each known method
    const char* method = json["method"];
    if (STRINGS_MATCH(method, "alarm"))
    {
        LOGI("MQTT", "Alarm method");
        const char* params = json["params"];
        // TODO: Alarm part. Parameters contain the level / sound effect.
        replyMeRpc(id, "{}");
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
    LOGD("MQTT", "Replying to topic %s", topic); // TODO: Seems to crash.
    // xSemaphoreTake(mqttMutex, portMAX_DELAY);
    mqtt.publish(topic, reply);
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