/**
 * @file devices.h
 * @brief Devices and manager for all known devices the base station connects
 * to.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-09
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
    Device(const char *name, const char symbol) : Lookupable(name, symbol) {}
    // Device(const char *name, const char symbol, LookupManager<Field> fields) : Lookupable(name, symbol), fields(fields) {}
    // LookupManager<Field<char>> fields;
};

/**
 * @brief Class for managing all devices
 *
 */
class DeviceManager : public LookupManager<Device>
{
public:
    DeviceManager(Device *items, const uint8_t count) : LookupManager(items, count) {}

    /**
     * @brief Registers each device to Thingsboard over MQTT.
     *
     */
    void connectDevices();
};