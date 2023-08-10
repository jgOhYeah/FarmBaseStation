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

Field fieldsList[] = {
    TenthsField("Battery Voltage", 'V'),
    LongUIntField("Uptime", 't'),
    TenthsField("Temperature", 'T'),
    ByteField("Transmit Enabled", 'r'),
    ByteField("Fence Enabled", 'F'),
    FlagField("Request status", 's'),
    ByteField("Transmit Interval", 'I'),
    PumpOnTimeField("Pump on time", 'P'),
    PumpOnTimeField("Average pump on time", 'a'),
    UIntField("Water capacitive reading", 'w'),
    PumpOnTimeField("Maximum pump on time in block", 'm'),
    PumpOnTimeField("Minimum pump on time in block", 'n'),
    UIntField("Count of pump starts in block", 'c'),
    ByteField("Reset", 'X') // Needs value to be set to 101 to reset.
};

Device deviceList[] = {
    Device("Main Pressure Pump", 0x5A),
    Device("Solar Electric Fence", 0x4A),
    Device("Irrigation Water Detector", 0x47)
};

DeviceManager deviceManager(deviceList, sizeof(deviceList));