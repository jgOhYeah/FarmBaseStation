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

DecodeResult Device::decodePacketFields(uint8_t *payload, uint8_t length, JsonDocument &json)
{
    LOGD("DEVICES", "Decoding packet of length %d.", length);

    // Make the object that will be used.
    // This doesn't quite follow the documentation - leaving out ts at least
    // means that the nested values object in an array isn't needed.
    JsonArray deviceArray = json[name].to<JsonArray>();
    JsonObject valuesObject = deviceArray.add<JsonObject>();

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
            uint8_t valueStart = i + 1;
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

DecodeResult Device::decodePacketFields(uint8_t *payload, uint8_t length, JsonDocument &json, int rssi, float snr)
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

bool Device::rpcWaiting()
{
    // For each field, check if it needs to be transmitted.
    for (uint8_t i = 0; i < fields.count; i++)
    {
        if (fields.items[i]->txRequired)
        {
            return true;
        }
    }

    // Nothing needs to be transmitted.
    return false;
}

int8_t Device::generatePacket(uint8_t *payload, uint8_t maxLength)
{
    // For each field, check if it needs to be transmitted.
    int8_t length = 0;
    for (uint8_t i = 0; i < fields.count; i++)
    {
        if (fields.items[i]->txRequired)
        {
            // Need to encode this field.
            LOGD("LORA_TX", "Encoding field '%s'.", fields.items[i]->name);
            int8_t result = fields.items[i]->encode(payload, maxLength - length);
            if (result != FIELD_NO_MEMORY)
            {
                LOGD("LORA_TX", "This field was %d bytes long including header", result);
                length += result;
            }
            else
            {
                return FIELD_NO_MEMORY;
            }
        }
    }

    // Return the size of the buffer that was actually used.
    return length;
}

void DeviceManager::connectDevices()
{
    // For each device, connect it.
    for (uint8_t i = 0; i < count; i++)
    {
        // Generate a json object with everything required.
        JsonDocument json;
        json["device"] = items[i]->name;
        char charBuff[MAX_JSON_TEXT_LENGTH];
        serializeJson(json, charBuff, sizeof(charBuff));

        // Do the sending.
        xSemaphoreTake(mqttMutex, portMAX_DELAY);
        LOGI("DEVICES", "Registering %s", charBuff);
        mqtt.publish(Topic::DEVICE_CONNECT, charBuff);
        xSemaphoreGive(mqttMutex);
    }
}

uint8_t DeviceManager::txRequired()
{
    uint8_t tx_count = 0;
    // For each device, check if it needs to send a packet.
    for (uint8_t i = 0; i < count; i++)
    {
        if (items[i]->rpcWaiting())
        {
            // Transmission waiting
            tx_count++;
        }
    }

    return tx_count;
}