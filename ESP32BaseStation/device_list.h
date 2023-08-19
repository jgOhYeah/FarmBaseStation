/**
 * @file device_list.h
 * @brief List of all devices and fields known to the system.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */

#pragma once
#include "src/fields.h"
#include "src/devices.h"

// TODO: Something less horrible for instantiating fields than this (fields can't be shared between devices as they store the current and next values).
Field *pumpFieldsList[] = {
    // new LongUIntField("Uptime", 't'),
    // new TenthsField("Temperature", 'T'),
    // new SettableByteField("Transmit Enabled", 'r'),
    // new FlagField("Request status", 's'),
    // new ByteField("Transmit Interval", 'I'),
    new PumpOnTimeField("Pump on time", 'P'),
    new PumpOnTimeField("Average pump on time", 'a'),
    // new UIntField("Water capacitive reading", 'w'),
    new PumpOnTimeField("Maximum pump on time in block", 'm'),
    new PumpOnTimeField("Minimum pump on time in block", 'n'),
    new UIntField("Count of pump starts in block", 'c'),
    new ByteField("Reset", 'X') // Needs value to be set to 101 to reset. // TODO: Make settable
};

Field *fenceFieldsList[] = {
    new TenthsField("Battery Voltage", 'V'),
    // new LongUIntField("Uptime", 't'),
    new TenthsField("Temperature", 'T'),
    new SettableByteField("TransmitEnabled", 'r'),
    new SettableByteField("FenceEnabled", 'F'),
    new FlagField("RequestStatus", 's'),
    new ByteField("TransmitInterval", 'I'),
    new ByteField("Reset", 'X') // Needs value to be set to 101 to reset.
};

Field *waterBabyFieldsList[] = {
    new TenthsField("Battery Voltage", 'V'),
    // new LongUIntField("Uptime", 't'),
    // new TenthsField("Temperature", 'T'),
    // new SettableByteField("Transmit Enabled", 'r'),
    new FlagField("RequestStatus", 's'),
    // new ByteField("TransmitInterval", 'I'),
    new UIntField("Water capacitive reading", 'w'),
    new ByteField("Reset", 'X') // Needs value to be set to 101 to reset.
};

LookupManager<Field> pumpFieldsManager(pumpFieldsList, sizeof(pumpFieldsList) / sizeof(Field*));
LookupManager<Field> fenceFieldsManager(fenceFieldsList, sizeof(fenceFieldsList) / sizeof(Field*));
LookupManager<Field> waterBabyFieldsManager(waterBabyFieldsList, sizeof(waterBabyFieldsList) / sizeof(Field*));

Device *deviceList[] = {
    new Device("Main Pressure Pump", 0x5A, pumpFieldsManager),
    new Device("Solar Electric Fence", 0x4A, fenceFieldsManager),
    new Device("Irrigation Water Detector", 167, waterBabyFieldsManager)
};

DeviceManager deviceManager(deviceList, sizeof(deviceList) / sizeof(Device*));