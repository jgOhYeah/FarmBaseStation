/**
 * @file networking.h
 * @brief Header file for networking.cpp
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-08
 */
#pragma once
#include "../defines.h"

void wifiConnect();
void mqttReceived(char* topic, byte* message, unsigned int length);
void mqttConnect();
void networkingTask(void *pvParameters);