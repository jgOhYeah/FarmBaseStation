/**
 * @file conversions.h
 * @brief General functions for converting to and from char arrays.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */
#pragma once
#include "../defines.h"
/**
 * Converts a long (4 bytes) to a char array in little endian format.
 * @param integer The integer to convert to a char array.
 * @param charBuffer The char array to put the number in. Must be at least 4 chars + startPos long.
 * @param startPos [Optional, default 0] The position in the char array to put the given long in.
 */
void uLongToByteArray(uint32_t integer, uint8_t *charBuffer);

/**
 * Converts a char array to a long (4 bytes) in little endian format.
 * @param charBuffer The char array to use. Must be at least 4 chars + startPos long.
 * @param startPos [Optional, default 0] The position in the char array to read the given long from.
 */
uint32_t byteArrayToULong(uint8_t *charBuffer);

/**
 * Converts an unsigned int (2 bytes) to a char array in little endian format.
 * @param integer The integer to convert to a char array.
 * @param charBuffer The char array to put the number in. Must be at least 4 chars + startPos long.
 * @param startPos [Optional, default 0] The position in the char array to put the given long in.
 */
void uIntToByteArray(uint16_t integer, uint8_t *charBuffer);

/**
 * Converts a char array to an unsigned int (2 bytes) in little endian format.
 * @param charBuffer The char array to use. Must be at least 4 chars + startPos long.
 * @param startPos [Optional, default 0] The position in the char array to read the given long from.
 */
uint16_t byteArrayToUInt(uint8_t *charBuffer);