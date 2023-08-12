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

bool Field::checkDecodeable(uint8_t length, uint8_t required)
{
    LOGD("FIELDS", "Decoding '%s' using %d bytes, %d bytes provided", name, required, length);
    if (length < required)
    {
        LOGI("FIELDS", "%d bytes provided, but need %d to decode.", length, required);
        return false;
    }
    return true;
}

int8_t TenthsField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (checkDecodeable(length, 2))
    {
        // Convert and save to json
        int16_t value = (int16_t)byteArrayToUInt(bytes);
        // Use sprintf to manually format to avoid floating point rounding issues.
        char charBuff[8];
        sprintf(charBuff, "%d.%d", value / 10, value % 10);
        json[name] = serialized(charBuff);
        return 2;
    }
    return FIELD_NO_MEMORY;
}

int8_t LongUIntField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (checkDecodeable(length, 4))
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
    if (checkDecodeable(length, 1))
    {

        // Convert and save to json
        json[name] = (uint8_t)bytes[0];
        return 1;
    }
    return FIELD_NO_MEMORY;
}

int8_t FlagField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (checkDecodeable(length, 0))
    {
        // Convert and save to json
        json[name] = 1; // Set to a constant
        return 0;
    }
    return FIELD_NO_MEMORY;
}

int8_t PumpOnTimeField::decode(uint8_t *bytes, uint8_t length, JsonObject &json)
{
    // Check we have enough bytes to safely complete this.
    if (checkDecodeable(length, 2))
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
    if (checkDecodeable(length, 2))
    {
        // Convert and save to json
        json[name] = byteArrayToUInt(bytes);
        return 2;
    }
    return FIELD_NO_MEMORY;
}