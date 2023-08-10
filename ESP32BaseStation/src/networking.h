/**
 * @file networking.h
 * @brief Header file for networking.cpp
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-10
 */
#pragma once
#include "../defines.h"
#include "devices.h"

void wifiConnect();
void mqttReceived(char *topic, byte *message, unsigned int length);
void mqttConnect();
void mqttSetup();
void networkingTask(void *pvParameters);