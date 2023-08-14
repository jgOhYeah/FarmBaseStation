/**
 * @file fields.cpp
 * @brief Contains fields classes.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */
#include "fields.h"

extern SemaphoreHandle_t serialMutex;
extern uint16_t byteArrayToUInt(uint8_t *charBuffer);
extern uint32_t byteArrayToULong(uint8_t *charBuffer);

char Field::writeSymbol()
{
    return symbol | 0x80;
}

int8_t Field::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    return 0;
}

int8_t Field::encode(uint8_t *bytes, uint8_t length)
{
    return 0;
}

void Field::handleRpc(JsonObject &params, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &reply)
{
}

bool Field::txRequired()
{
    // Never update by default
    return false;
}

template <typename T>
bool SettableField<T>::txRequired()
{
    return setValue != curValue;
}

template <typename T>
void SettableField<T>::handleRpc(JsonObject &data, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &reply)
{
    T candidate = data["params"];
    if (isValidNewValue(candidate))
    {
        setValue = candidate;
        reply["success"] = true;
        LOGD("RPC", "Successfully setting rpc call");
    }
    else
    {
        reply["success"] = false;
        LOGD("RPC", "Invalid rpc call value to method '%s'", name);
    }
}

template <typename T>
bool SettableField<T>::isValidNewValue(T value)
{
    return true; // Always accepted by default
}

bool Field::checkDecodeable(uint8_t length)
{
    LOGD("FIELDS", "Decoding '%s' using %d bytes, %d bytes provided", name, encodedLength, length);
    if (length < encodedLength)
    {
        LOGI("FIELDS", "%d bytes provided, but need %d to decode.", length, encodedLength);
        return false;
    }
    return true;
}

int8_t TenthsField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (checkDecodeable(length))
    {
        // Convert and save to json
        int16_t value = (int16_t)byteArrayToUInt(bytes);
        // Use sprintf to manually format to avoid floating point rounding issues.
        char charBuff[8];
        sprintf(charBuff, "%d.%d", value / 10, abs(value) % 10);
        json[name] = serialized(charBuff);
        return 2;
    }
    return FIELD_NO_MEMORY;
}

int8_t LongUIntField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (checkDecodeable(length))
    {
        // Convert and save to json
        json[name] = byteArrayToULong(bytes);
        return 4;
    }
    return FIELD_NO_MEMORY;
}

int8_t ByteField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (checkDecodeable(length))
    {

        // Convert and save to json
        json[name] = (uint8_t)bytes[0];
        return 1;
    }
    return FIELD_NO_MEMORY;
}

int8_t SettableByteField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (checkDecodeable(length))
    {

        // Convert and save to json
        json[name] = (uint8_t)bytes[0];
        return 1;
    }
    return FIELD_NO_MEMORY;
}

int8_t SettableByteField::encode(uint8_t *bytes, uint8_t length)
{
    uint8_t totalBytes = encodedLength + 1;
    if (length >= totalBytes)
    {
        // Have enough memory to properly encode.
        bytes[0] = writeSymbol();
        bytes[1] = setValue;
        return totalBytes;
    }
    else
    {
        LOGE("FIELD", "Not enough memory to encode field.");
        return FIELD_NO_MEMORY;
    }
}

int8_t FlagField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (checkDecodeable(length))
    {
        // Convert and save to json
        json[name] = 1; // Set to a constant
        return 0;
    }
    return FIELD_NO_MEMORY;
}

int8_t SettableFlagField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (checkDecodeable(length))
    {
        // Convert and save to json
        json[name] = 1; // Set to a constant
        return 0;
    }
    return FIELD_NO_MEMORY;
}

int8_t SettableFlagField::encode(uint8_t *bytes, uint8_t length)
{
    if (length >= (encodedLength + 1))
    {
        // Have enough memory to properly encode.
        bytes[0] = writeSymbol();
        return 1;
    }
    else
    {
        LOGE("FIELD", "Not enough memory to encode field.");
        return FIELD_NO_MEMORY;
    }
}

int8_t PumpOnTimeField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (checkDecodeable(length))
    {

        // Convert and save to json
        uint16_t value = byteArrayToUInt(bytes);
        // Use sprintf to manually format to avoid floating point rounding issues.
        char charBuff[8];
        sprintf(charBuff, "%d.%d", value >> 1, (value & 0x1) ? 5 : 0);
        json[name] = serialized(charBuff);
        return 2;
    }
    return FIELD_NO_MEMORY;
}

int8_t UIntField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (checkDecodeable(length))
    {
        // Convert and save to json
        json[name] = byteArrayToUInt(bytes);
        return 2;
    }
    return FIELD_NO_MEMORY;
}