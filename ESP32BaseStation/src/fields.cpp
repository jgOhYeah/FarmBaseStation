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

int8_t TenthsField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (length < 2)
    {
        LOGI("FIELDS", "Got a field without enough bytes");
        return FIELD_NO_MEMORY;
    }

    // Convert and save to json
    float value = (int16_t)byteArrayToUInt(bytes) / 10.;
    json[name] = value;
    return 2;
}

int8_t LongUIntField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (length < 4)
    {
        LOGI("FIELDS", "Got a field without enough bytes");
        return FIELD_NO_MEMORY;
    }

    // Convert and save to json
    json[name] = byteArrayToULong(bytes);
    return 4;
}

int8_t ByteField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (length < 1)
    {
        LOGI("FIELDS", "Got a field without enough bytes");
        return FIELD_NO_MEMORY;
    }

    // Convert and save to json
    json[name] = (uint8_t)bytes[0];
    return 1;
}

int8_t FlagField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Convert and save to json
    json[name] = 1; // Set to a constant
    return 0;
}

int8_t PumpOnTimeField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (length < 2)
    {
        LOGI("FIELDS", "Got a field without enough bytes");
        return FIELD_NO_MEMORY;
    }

    // Convert and save to json
    float value = byteArrayToUInt(bytes) / 2.;
    json[name] = value;
    return 2;
}

int8_t UIntField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (length < 4)
    {
        LOGI("FIELDS", "Got a field without enough bytes");
        return FIELD_NO_MEMORY;
    }

    // Convert and save to json
    json[name] = byteArrayToULong(bytes);
    return 4;
}