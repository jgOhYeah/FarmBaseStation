/**
 * @file devices.h
 * @brief Devices and manager for all known devices the base station connects
 * to.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-11
 */
#pragma once
#include "../defines.h"
#include "lookups.h"
#include "fields.h"

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
    void decodePacketFields(uint8_t *payload, uint8_t length, StaticJsonDocument<MAX_JSON_TEXT_LENGTH> &json);

    LookupManager<Field> &fields;
};

/**
 * @brief Class for managing all devices
 *
 */
class DeviceManager : public LookupManager<Device>
{
public:
    DeviceManager(const Device *const *items, const uint8_t count) : LookupManager(items, count) {}

    /**
     * @brief Registers each device to Thingsboard over MQTT.
     *
     */
    void connectDevices();
};