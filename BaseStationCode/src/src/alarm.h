/**
 * @file alarm.h
 * @brief Handles alarms
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-13
 */

#pragma once
#include "../defines.h"

enum AlarmState {ALARM_OFF, ALARM_HIGH, ALARM_MEDIUM, ALARM_DOORBELL};

/**
 * @brief Task for managing the alarm mode.
 * 
 * @param pvParameters 
 */
void alarmTask(void *pvParameters);