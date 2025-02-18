/**
 * @file fields.cpp
 * @brief Contains fields classes.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2024-12-19
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
    if (checkDecodeable(length))
    {
        return actuallyDecode(bytes, length, json);
    }
    return FIELD_NO_MEMORY;
}

int8_t Field::encode(uint8_t *bytes, uint8_t length)
{
    return 0;
}

void Field::handleRpc(JsonObject &data, JsonObject &replyData)
{
}

template <typename T>
void SettableField<T>::setTxRequired()
{
    if (setValue == curValue)
    {
        // Mission accomplished
        txRequired = false;
    }
}

template <typename T>
void SettableField<T>::handleRpc(JsonObject &data, JsonObject &replyData)
{
    T candidate = data["params"];
    if (isValidNewValue(candidate))
    {
        setValue = candidate;
        txRequired = true;
        replyData["success"] = true;
        LOGD("RPC", "Successfully setting rpc call");
    }
    else
    {
        replyData["success"] = false;
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

int8_t Field::actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    return 0;
}

int8_t TenthsByteField::actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json)
{

    // Convert and save to json
    // Obtain the value
    uint8_t value = (uint8_t)bytes[0];
    // Use sprintf to manually format to avoid floating point rounding issues.
    char charBuff[8];
    sprintf(charBuff, "%d.%d", value / 10, value % 10);
    json[name] = serialized(charBuff);
    return 1;
}

int8_t TenthsField::actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Convert and save to json
    // Obtain the value
    int16_t value = (int16_t)byteArrayToUInt(bytes);

    // Separate the sign and magnitude into a string and variable
    char signStr[2] = "-";
    if (value >= 0)
    {
        // No sign. Replace with a null char to make the string empty.
        signStr[0] = '\0';
    }
    value = abs(value);

    // Use sprintf to manually format to avoid floating point rounding issues.
    char charBuff[8];
    sprintf(charBuff, "%s%d.%d", signStr, value / 10, value % 10);
    json[name] = serialized(charBuff);
    return 2;
}

int8_t LongUIntField::actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Convert and save to json
    json[name] = byteArrayToULong(bytes);
    return 4;
}

int8_t ByteField::actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Convert and save to json
    json[name] = (uint8_t)bytes[0];
    return 1;
}

int8_t SettableByteField::actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Convert and save to json
    json[name] = (int8_t)bytes[0];
    curValue = bytes[0];
    setTxRequired();
    return 1;
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

int8_t FlagField::actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Convert and save to json
    json[name] = 1; // Set to a constant
    return 0;
}

int8_t SettableFlagField::actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Convert and save to json
    json[name] = 1; // Set to a constant
    // TODO: Some way to know when this has been successfully sent and received.
    setTxRequired();
    return 0;
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

int8_t PumpOnTimeField::actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Convert and save to json
    uint16_t value = byteArrayToUInt(bytes);
    // Use sprintf to manually format to avoid floating point rounding issues.
    char charBuff[8];
    sprintf(charBuff, "%d.%d", value >> 1, (value & 0x1) ? 5 : 0);
    json[name] = serialized(charBuff);
    return 2;
}

int8_t UIntField::actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Convert and save to json
    json[name] = byteArrayToUInt(bytes);
    return 2;
}