/**
 * @file fields.cpp
 * @brief Contains fields classes.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-09
 */
#include "fields.h"

extern SemaphoreHandle_t serialMutex;
extern uint16_t charArrayToUInt(char *charBuffer);
extern uint32_t charArrayToULong(char *charBuffer);

char Field::writeSymbol()
{
    return symbol | 0x80;
}

void Field::decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json) {}

void TenthsField::decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json)
{
    // Check we have enough bytes to safely complete this.
    if (length < 2)
    {
        LOGI("FIELDS", "Got a field without enough bytes");
        return;
    }

    // Convert and save to json
    float value = (int16_t)charArrayToUInt(bytes) / 10.;
    json[name] = value;
}

void LongUIntField::decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json)
{
    // Check we have enough bytes to safely complete this.
    if (length < 4)
    {
        LOGI("FIELDS", "Got a field without enough bytes");
        return;
    }

    // Convert and save to json
    json[name] = charArrayToULong(bytes);
}

void ByteField::decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json)
{
    // Check we have enough bytes to safely complete this.
    if (length < 1)
    {
        LOGI("FIELDS", "Got a field without enough bytes");
        return;
    }

    // Convert and save to json
    json[name] = (uint8_t)bytes[0];
}

void FlagField::decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json)
{
    // Convert and save to json
    json[name] = 1; // Set to a constant
}

void PumpOnTimeField::decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json)
{
    // Check we have enough bytes to safely complete this.
    if (length < 2)
    {
        LOGI("FIELDS", "Got a field without enough bytes");
        return;
    }

    // Convert and save to json
    float value = charArrayToUInt(bytes) / 2.;
    json[name] = value;
}

void UIntField::decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json)
{
    // Check we have enough bytes to safely complete this.
    if (length < 4)
    {
        LOGI("FIELDS", "Got a field without enough bytes");
        return;
    }

    // Convert and save to json
    json[name] = charArrayToULong(bytes);
}