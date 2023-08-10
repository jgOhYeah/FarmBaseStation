/**
 * @file device_list.h
 * @brief List of all devices and fields known to the system.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-09
 */

#pragma once
#include "src/fields.h"
#include "src/devices.h"

Field *fieldsList[] = {
    new TenthsField("Battery Voltage", 'V'),
    new LongUIntField("Uptime", 't'),
    new TenthsField("Temperature", 'T'),
    new ByteField("Transmit Enabled", 'r'),
    new ByteField("Fence Enabled", 'F'),
    new FlagField("Request status", 's'),
    new ByteField("Transmit Interval", 'I'),
    new PumpOnTimeField("Pump on time", 'P'),
    new PumpOnTimeField("Average pump on time", 'a'),
    new UIntField("Water capacitive reading", 'w'),
    new PumpOnTimeField("Maximum pump on time in block", 'm'),
    new PumpOnTimeField("Minimum pump on time in block", 'n'),
    new UIntField("Count of pump starts in block", 'c'),
    new ByteField("Reset", 'X') // Needs value to be set to 101 to reset.
};

Device *deviceList[] = {
    new Device("Main Pressure Pump", 0x5A),
    new Device("Solar Electric Fence", 0x4A),
    new Device("Irrigation Water Detector", 0x47)
};

DeviceManager deviceManager(deviceList, sizeof(deviceList));