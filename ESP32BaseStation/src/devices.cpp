/**
 * @file devices.cpp
 * @brief Devices and manager for all known devices the base station connects
 * to.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */
#include "devices.h"

extern SemaphoreHandle_t serialMutex;
extern SemaphoreHandle_t mqttMutex;
extern PubSubClient mqtt;

void Device::decodePacketFields(uint8_t *payload, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json)
{
    LOGD("DEVICES", "Decoding packet of length %d.", length);
    // Check we have at least 1 character to decode.
    if (length == 0)
    {
        LOGI("DEVICES", "Was given a packet to decode with 0 length.");
        return;
    }

    // Make the object that will be used
    JsonArray deviceArray = json.createNestedArray(name);
    JsonObject pointInTime = deviceArray.createNestedObject();
    JsonObject valuesObject = pointInTime.createNestedObject("values");

    // For each field, add it to the document.
    for (uint8_t i = 0; i < length; i++)
    {
        Field *field = fields.getWithSymbol((char)payload[i]);
        if (field)
        {
            uint8_t valueStart = i+1;
            int8_t result = field->decode(&payload[valueStart], length - valueStart, valuesObject);
            if (result != FIELD_NO_MEMORY)
            {
                LOGI("DEVICES", "Ran out of spots in the packet to properly decode.");
                i += result;
            }
        }
        else
        {
            LOGI("DEVICES", "Unrecognised field. Discarding from here on.");
            return;
        }
    }
}

void DeviceManager::connectDevices()
{
    // For each device, connect it.
    for (uint8_t i = 0; i < m_count; i++)
    {
        // Generate a json object with everything required.
        StaticJsonDocument<MAX_JSON_TEXT_LENGTH> json;
        json["device"] = m_items[i]->name;
        char charBuff[MAX_JSON_TEXT_LENGTH];
        serializeJson(json, charBuff, sizeof(charBuff));

        // Do the sending.
        xSemaphoreTake(mqttMutex, portMAX_DELAY);
        LOGI("DEVICES", "Registering %s", charBuff);
        mqtt.publish(Topic::DEVICE_CONNECT, charBuff);
        xSemaphoreGive(mqttMutex);
    }
}