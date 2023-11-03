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
extern QueueHandle_t audioQueue;

void alarmTask(void *pvParameters)
{
    AlarmState curState = ALARM_OFF;
    AlarmState prevState = ALARM_HIGH;
    while (true)
    {
        AlarmState state;
        int result = xQueueReceive(alarmQueue, (void *)&state, 1000 / portTICK_RATE_MS);
        if (result)
        {
            // We actually got something:
            LOGD("ALARM", "State set to %d", state);
            curState = state;
            xQueueSend(audioQueue, (void *)&state, portMAX_DELAY);
        }

        // Flash the LEDs as needed (only on state change.)
        switch (curState)
        {
        case ALARM_OFF:
            // All LEDs off.
            if (curState != prevState)
            {
                digitalWrite(PIN_LED_TOP, LOW);
                digitalWrite(PIN_LED_INSIDE, LOW);
            }
            break;

        case ALARM_MEDIUM:
            // LED on top on, inside off.
            if (curState != prevState)
            {
                digitalWrite(PIN_LED_TOP, HIGH);
                digitalWrite(PIN_LED_INSIDE, LOW);
            }
            break;

        case ALARM_HIGH:
            // LED on top flashing, inside on.
            digitalWrite(PIN_LED_TOP, !digitalRead(PIN_LED_TOP));
            if (curState != prevState)
            {
                digitalWrite(PIN_LED_INSIDE, HIGH);
            }
            break;
        }
        prevState = curState;
    }
}