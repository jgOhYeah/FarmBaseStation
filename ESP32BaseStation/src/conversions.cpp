/**
 * @file conversions.cpp
 * @brief General functions for converting to and from char arrays.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-10
 */
#include "conversions.h"

void uLongToByteArray(uint32_t integer, uint8_t *charBuffer)
{
    // For each byte, generate a char from it.
    for (uint8_t i = 0; i < 4; i++)
    {
        charBuffer[i] = integer & 0xFF;
        integer = integer >> 8;
    }
}

uint32_t byteArrayToULong(uint8_t *charBuffer)
{
    // For each byte, generate a char from it.
    uint32_t integer = 0;
    for (uint8_t i = 3; i >= 0; i--)
    {
        integer = integer << 8;
        integer += (uint8_t)charBuffer[i];
    }
    return integer;
}

void uIntToByteArray(uint16_t integer, uint8_t *charBuffer)
{
    charBuffer[0] = integer & 0xFF;
    charBuffer[1] = (integer >> 8);
}

uint16_t byteArrayToUInt(uint8_t *charBuffer)
{
    uint16_t integer = (uint8_t)charBuffer[0] | (((uint8_t)charBuffer[1]) << 8);
    return integer;
}