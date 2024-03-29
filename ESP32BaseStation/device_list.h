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

// Generic
#define FIELD_TEMPERATURE TenthsField("Temperature", 'T')
#define FIELD_HUMIDITY ByteField("Humidity", 'H')
#define FIELD_BATTERY_VOLTAGE TenthsField("Battery Voltage", 'V')
#define FIELD_TRANSMIT SettableByteField("TransmitEnabled", 'r')
#define FIELD_TX_INTERVAL SettableByteField("TransmitInterval", 'I')

// Controls
#define FIELD_RESET ByteField("Reset", 'X') // Needs value to be set to 101 to reset. // TODO: Make settable
#define FIELD_STATUS FlagField("RequestStatus", 's')

// Pump specific
#define FIELD_PUMP_ON_TIME PumpOnTimeField("Pump on time", 'P')
#define FIELD_PUMP_AVE_ON_TIME PumpOnTimeField("Average pump on time", 'a')
#define FIELD_PUMP_MAX_ON_TIME PumpOnTimeField("Maximum pump on time in block", 'm')
#define FIELD_PUMP_MIN_ON_TIME PumpOnTimeField("Minimum pump on time in block", 'n')
#define FIELD_PUMP_START_COUNT UIntField("Count of pump starts in block", 'c')

// Fence specific
#define FIELD_FENCE SettableByteField("FenceEnabled", 'F')

// Water baby
#define FIELD_WATER_CAPACITIVE UIntField("Water capacitive reading", 'w')

Field *pumpFieldsList[] = {
    new FIELD_TEMPERATURE,
    new FIELD_HUMIDITY,
    new FIELD_PUMP_ON_TIME,
    new FIELD_PUMP_AVE_ON_TIME,
    new FIELD_PUMP_MAX_ON_TIME,
    new FIELD_PUMP_MIN_ON_TIME,
    new FIELD_PUMP_START_COUNT,
    new FIELD_RESET
};

Field *fenceFieldsList[] = {
    new FIELD_BATTERY_VOLTAGE,
    new FIELD_TEMPERATURE,
    new FIELD_TRANSMIT,
    new FIELD_FENCE,
    new FIELD_STATUS,
    new FIELD_TX_INTERVAL,
    new FIELD_RESET
};

Field *waterBabyFieldsList[] = {
    new FIELD_BATTERY_VOLTAGE,
    new FIELD_STATUS,
    new FIELD_WATER_CAPACITIVE,
    new FIELD_RESET
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