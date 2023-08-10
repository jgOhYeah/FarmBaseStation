/**
 * @file fields.h
 * @brief Contains fields classes.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-09
 */

#pragma once
#include "../defines.h"
#include "lookups.h"
#include "conversions.h"

/**
 * @brief Class for handling data fields in messages.
 *
 */
class Field : public Lookupable
{
public:
    Field(const char *name, char symbol) : Lookupable(name, symbol) {}

    // /** // TODO
    //  * @brief Encodes the value as bytes ready to send.
    //  *
    //  * @param value the value to encode.
    //  * @param bytes pointer to the memory to write the encoded value to.
    //  */
    // virtual void encode(DataFormat value, uint8_t *bytes) {}

    /**
     * @brief Decodes the value from bytes into an existing json document.
     * 
     * @param bytes the data to decode.
     * @param length the amount of data remaining from the start of bytes.
     * @param json the document to place the results into.
     */
    virtual void decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json);

    /**
     * @brief Returns the symbol in write mode (MSB set to 1).
     */
    char writeSymbol();
};

/**
 * @brief Field for decoding 2 byte signed data in tenths.
 * 
 */
class TenthsField : public Field
{
    using Field::Field; // Inherit field constructors.

    virtual void decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json);
};

/**
 * @brief Fields that contain an unsigned, 4 byte integer.
 * 
 */
class LongUIntField : public Field
{
    using Field::Field; // Inherit field constructors.

    virtual void decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json);
};

/**
 * @brief Fields that contain an unsigned, single byte integer.
 * 
 */
class ByteField : public Field
{
    using Field::Field; // Inherit field constructors.

    virtual void decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json);
};

/**
 * @brief Fields that don't have any value or payload.
 * 
 */
class FlagField : public Field
{
    using Field::Field;

    virtual void decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json);
};

/**
 * @brief Fields for pump on times (2 bytes in 0.5s increments)
 * 
 */
class PumpOnTimeField : public Field
{
    using Field::Field;

    virtual void decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json);
};

// TODO: Make decode return the number of bytes read.

/**
 * @brief Fields that contain an unsigned, 2 byte integer.
 * 
 */
class UIntField : public Field
{
    using Field::Field; // Inherit field constructors.

    virtual void decode(char *bytes, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json);
};