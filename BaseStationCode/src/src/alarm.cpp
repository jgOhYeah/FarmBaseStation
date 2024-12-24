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
#ifdef PIN_SPEAKER
extern QueueHandle_t audioQueue;
#endif
extern SemaphoreHandle_t stateUpdateMutex;
extern AlarmState alarmState;
extern TaskHandle_t ledTaskHandle;

void alarmTask(void *pvParameters)
{
    while (true)
    {
        // TODO: Get rid of this task.
        AlarmState state;
        xQueueReceive(alarmQueue, (void *)&state, portMAX_DELAY);
        
        // We actually got something:
        LOGD("ALARM", "State set to %d", state);
        xSemaphoreTake(stateUpdateMutex, portMAX_DELAY);
        alarmState = state;
        xSemaphoreGive(stateUpdateMutex);
#ifdef PIN_SPEAKER
        xQueueSend(audioQueue, (void *)&state, portMAX_DELAY);
#endif
    }
}