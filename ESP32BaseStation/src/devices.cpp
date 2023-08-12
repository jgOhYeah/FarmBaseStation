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

DecodeResult Device::decodePacketFields(uint8_t *payload, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json)
{
    LOGD("DEVICES", "Decoding packet of length %d.", length);

    // Make the object that will be used.
    // This doesn't quite follow the documentation - leaving out ts at least
    // means that the nested values object in an array isn't needed.
    JsonArray deviceArray = json.createNestedArray(name);
    JsonObject valuesObject = deviceArray.createNestedObject();

    // Check we have at least 1 character to decode.
    if (length == 0)
    {
        LOGI("DEVICES", "Was given a packet to decode with 0 length.");
        return DECODE_FAIL;
    }


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
                // Successfully got the field. Account for the space taken up by the value.
                i += result;
            }
            else
            {
                // Not enough memory to decode the packet.
                LOGI("DEVICES", "Seeing as we ran out of packet, the rest will be discarded.");
                // If we got at least one term, then not a complete failure.
                return i ? DECODE_PARTIAL : DECODE_FAIL;
            }
        }
        else
        {
            LOGI("DEVICES", "Unrecognised field. Discarding from here on.");
            // If we got at least one term, then not a complete failure.
            return i ? DECODE_PARTIAL : DECODE_FAIL;
        }
    }
    LOGD("DEVICES", "Finished decoding packet.");
    return DECODE_SUCCESS;
}

DecodeResult Device::decodePacketFields(uint8_t *payload, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json, int rssi, float snr)
{
    // Do the decoding as per normal
    DecodeResult result = Device::decodePacketFields(payload, length, json);

    // Add the snr and rssi
    JsonObject valuesObject = json[name][0];
    valuesObject["SNR"] = snr;
    valuesObject["RSSI"] = rssi;

    // Return the result
    return result;
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