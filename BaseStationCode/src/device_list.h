/**
 * @file device_list.h
 * @brief List of all devices and fields known to the system.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2024-12-19
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
#define FIELD_FENCE SettableByteField("FenceEnabled", 'F') // Remote control
#define FIELD_FENCE_VOLTAGE TenthsByteField("Fence Voltage", 'k') // Monitor

// Water baby
#define FIELD_WATER_CAPACITIVE UIntField("Water capacitive reading", 'w')

// Gate monitor
#define FIELD_GATE_STATE ByteField("Gate state", 'g')
#define FIELD_LIGHT_LEVEL ByteField("Light level", 'l')
#define FIELD_MOVEMENT_DETECTED ByteField("Movement detected", 'M')

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

Field *fenceMonitorFieldsList[] = {
    new FIELD_BATTERY_VOLTAGE,
    new FIELD_FENCE_VOLTAGE,
    new FIELD_TEMPERATURE
};

Field *gateMonitorFieldsList[] = {
    new FIELD_GATE_STATE,
    new FIELD_BATTERY_VOLTAGE,
    new FIELD_TEMPERATURE,
    new FIELD_HUMIDITY,
    new FIELD_LIGHT_LEVEL,
    new FIELD_MOVEMENT_DETECTED,
    new FIELD_TX_INTERVAL
};

LookupManager<Field> pumpFieldsManager(pumpFieldsList, sizeof(pumpFieldsList) / sizeof(Field*));
LookupManager<Field> fenceFieldsManager(fenceFieldsList, sizeof(fenceFieldsList) / sizeof(Field*));
LookupManager<Field> waterBabyFieldsManager(waterBabyFieldsList, sizeof(waterBabyFieldsList) / sizeof(Field*));
LookupManager<Field> fenceMonitorFieldsManager(fenceMonitorFieldsList, sizeof(fenceMonitorFieldsList) / sizeof(Field*));
LookupManager<Field> gateMonitorFieldsManager(gateMonitorFieldsList, sizeof(gateMonitorFieldsList) / sizeof(Field*));

Device *deviceList[] = {
    new Device("Main Pressure Pump", 0x5A, pumpFieldsManager),
    new Device("Solar Electric Fence", 0x4A, fenceFieldsManager),
    new Device("Irrigation Water Detector", 167, waterBabyFieldsManager), // 0xA7
    new Device("Electric fence monitor", 168, fenceMonitorFieldsManager), // 0xA8
    new Device("Front gate monitor", 169, gateMonitorFieldsManager) // 0xA9
};

DeviceManager deviceManager(deviceList, sizeof(deviceList) / sizeof(Device*));