/**
 * @file alarm.cpp
 * @brief Handles alarms
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-13
 */

#include "alarm.h"

extern SemaphoreHandle_t serialMutex;
extern QueueHandle_t alarmQueue;

void alarmTask(void *pvParameters)
{
    while (true)
    {
        AlarmState state;
        xQueueReceive(alarmQueue, (void*)&state, portMAX_DELAY);
        LOGD("ALARM", "State set to %d", state);
    }
}