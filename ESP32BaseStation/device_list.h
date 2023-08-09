/**
 * @file device_list.h
 * @brief List of all devices and fields known to the system.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-09
 */

#pragma once


Device deviceList[] = {
    Device("Main Pressure Pump", 0x5A),
    Device("Solar Electric Fence", 0x4A),
    Device("Irrigation Water Detector", 0x47)
};

DeviceManager deviceManager(deviceList, sizeof(deviceList));