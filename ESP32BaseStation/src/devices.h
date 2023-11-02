/**
 * @file devices.h
 * @brief Devices and manager for all known devices the base station connects
 * to.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */
#pragma once
#include "../defines.h"
#include "lookups.h"
#include "fields.h"

/**
 * @brief List of statuses to return when decoding packets.
 * 
 */
enum DecodeResult {DECODE_SUCCESS, DECODE_PARTIAL, DECODE_FAIL};

/**
 * @brief Each sensor / device on the PJON network.
 *
 */
class Device : public Lookupable
{
public:
    Device(const char *name, const char symbol, LookupManager<Field> &fields) : Lookupable(name, symbol), fields(fields) {}

    /**
     * @brief Decodes the payload from a PJON packet and converts it to thingsboard MQTT JSON.
     *
     * @param payload the packet payload from PJON.
     * @param length the length of the payload.
     * @param json the JSON document to place the output in.
     */
    DecodeResult decodePacketFields(uint8_t *payload, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json);

    /**
     * @brief Decodes the payload from a PJON packet and converts it to thingsboard MQTT JSON. Also adds on packet metadata.
     *
     * @param payload the packet payload from PJON.
     * @param length the length of the payload.
     * @param json the JSON document to place the output in.
     * @param rssi the RSSI of the received packet.
     * @param snr the SNR of the received packet.
     */
    DecodeResult decodePacketFields(uint8_t *payload, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json, int rssi, float snr);

    /**
     * @brief Checks if a transmission is required.
     * 
     * @returns true if a transmission is required (latest received value != value to set to).
     */
    bool rpcWaiting();

    /**
     * @brief Generates a packet containing all the fields and instructions to set.
     * 
     * @param payload is the PJON payload to put this in.
     * @param maxLength the maximum length of the payload available.
     * @return uint8_t the length of the payload used.
     */
    int8_t generatePacket(uint8_t *payload, uint8_t maxLength);

    LookupManager<Field> &fields;
};

/**
 * @brief Class for managing all devices
 *
 */
class DeviceManager : public LookupManager<Device>
{
public:
    DeviceManager(Device **items, uint8_t count) : LookupManager(items, count) {}

    /**
     * @brief Registers each device to Thingsboard over MQTT.
     *
     */
    void connectDevices();

    /**
     * @brief Returns the number of packets that need to be sent across all
     * devices.
     * 
     * @return uint8_t 
     */
    uint8_t txRequired();
};