/**
 * @file fields.h
 * @brief Contains fields classes.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2024-12-19
 */

#pragma once
#include "../defines.h"
#include "lookups.h"
#include "conversions.h"

#define FIELD_NO_MEMORY -1

/**
 * @brief Class for handling data fields in messages.
 *
 */
class Field : public Lookupable
{
public:
    Field(const char *name, char symbol, const uint8_t encodedLength) : Lookupable(name, symbol), encodedLength(encodedLength) {}

    /**
     * @brief Decodes the value from bytes into an existing json document.
     *
     * @param bytes the data to decode.
     * @param length the amount of data remaining from the start of bytes.
     * @param json the document to place the results into.
     */
    int8_t decode(uint8_t *bytes, uint8_t length, JsonObject &json);

    /**
     * @brief Encodes the value to set into a packet ready to send if needed.
     *
     * @param bytes the payload to place in.
     * @param length the available length.
     * @returns the number of bytes used, including the symbol for the field.
     */
    virtual int8_t encode(uint8_t *bytes, uint8_t length);

    /**
     * @brief Returns the symbol in write mode (MSB set to 1).
     */
    char writeSymbol();

    /**
     * @brief handles an RPC call for a field.
     *
     * @param reply
     */
    virtual void handleRpc(JsonObject &data, JsonObject &replyData);

    const uint8_t encodedLength;
    bool txRequired = false;

protected:
    /**
     * @brief Checks if the value can be decoded from the packet.
     *
     * @param length the number of bytes provided.
     * @return true if the packet can be successfully decoded.
     * @return false otherwise.
     */
    bool checkDecodeable(uint8_t length);

    /**
     * @brief Decodes the value from bytes into an existing json document. decode does the checking for length, so this doesn't need to.
     *
     * @param bytes the data to decode.
     * @param length the amount of data remaining from the start of bytes.
     * @param json the document to place the results into.
     */
    virtual int8_t actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json);
};

/**
 * @brief Class for fields that can be set.
 *
 * @tparam T
 */
template <typename T>
class SettableField : public Field
{
public:
    using Field::Field;

    T setValue = 255;
    T curValue = 255;

    /**
     * @brief Updates txRequired (if setValue == current, then not required).
     */
    virtual void setTxRequired();

    /**
     * @brief handles an RPC call for a field.
     *
     * @param reply
     */
    virtual void handleRpc(JsonObject &data, JsonObject &replyData);

protected:
    /**
     * @brief Checks if the given value is acceptable to transmit.
     *
     * @param value
     * @return true
     * @return false
     */
    virtual bool isValidNewValue(T value);
};

/**
 * @brief Field for decoding 1 byte signed data in tenths.
 *
 */
class TenthsByteField : public Field
{
public:
    TenthsByteField(const char *name, char symbol) : Field(name, symbol, 1) {}

protected:
    virtual int8_t actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json);
};

/**
 * @brief Field for decoding 2 byte signed data in tenths.
 *
 */
class TenthsField : public Field
{
public:
    TenthsField(const char *name, char symbol) : Field(name, symbol, 2) {}

protected:
    virtual int8_t actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json);
};

/**
 * @brief Fields that contain an unsigned, 4 byte integer.
 *
 */
class LongUIntField : public Field
{
public:
    LongUIntField(const char *name, char symbol) : Field(name, symbol, 4) {}

protected:
    virtual int8_t actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json);
};

/**
 * @brief Fields that contain an unsigned, single byte integer.
 *
 */
class ByteField : public Field
{
public:
    ByteField(const char *name, char symbol) : Field(name, symbol, 1) {}

protected:
    virtual int8_t actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json);
};

/**
 * @brief Fields that contain an unsigned, single byte integer that can be set
 * using an RPC call.
 *
 */
class SettableByteField : public SettableField<int8_t>
{
public:
    SettableByteField(const char *name, char symbol) : SettableField(name, symbol, 1) {}

    /**
     * @brief Encodes the value to set into a packet ready to send.
     *
     * @param bytes the payload to place in.
     * @param length the available length.
     * @returns the number of bytes used, including the symbol for the field.
     */
    virtual int8_t encode(uint8_t *bytes, uint8_t length);

protected:
    virtual int8_t actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json);
};

/**
 * @brief Fields that don't have any value or payload.
 *
 */
class FlagField : public Field
{
public:
    FlagField(const char *name, char symbol) : Field(name, symbol, 0) {}

protected:
    virtual int8_t actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json);
};

/**
 * @brief Class for flags that can be set via an RPC call.
 *
 */
class SettableFlagField : public SettableField<int8_t>
{
public:
    SettableFlagField(const char *name, char symbol) : SettableField(name, symbol, 0) {}

    /**
     * @brief Encodes the value to set into a packet ready to send.
     *
     * @param bytes the payload to place in.
     * @param length the available length.
     * @returns the number of bytes used, including the symbol for the field.
     */
    virtual int8_t encode(uint8_t *bytes, uint8_t length);

protected:
    virtual int8_t actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json);
};

/**
 * @brief Fields for pump on times (2 bytes in 0.5s increments)
 *
 */
class PumpOnTimeField : public Field
{
public:
    PumpOnTimeField(const char *name, char symbol) : Field(name, symbol, 2) {}

protected:
    virtual int8_t actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json);
};

/**
 * @brief Fields that contain an unsigned, 2 byte integer.
 *
 */
class UIntField : public Field
{
public:
    UIntField(const char *name, char symbol) : Field(name, symbol, 2) {}

protected:
    virtual int8_t actuallyDecode(uint8_t *bytes, uint8_t length, JsonObject &json);
};