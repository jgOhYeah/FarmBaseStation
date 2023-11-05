/**
 * @file leds.cpp
 * @brief Controls the LEDs on the unit.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-11-05
 */
#include "leds.h"

extern NetworkState networkState;
extern AlarmState alarmState;
extern DeviceManager deviceManager;
extern SemaphoreHandle_t stateUpdateMutex;
extern SemaphoreHandle_t serialMutex;
extern uint32_t lastLoRaTime;

void Led::begin()
{
    pinMode(pin, OUTPUT);
    set(false); // Turn off by default.
}

void Led::tick()
{
    // TODO
}

void Led::set(bool state)
{
    digitalWrite(pin, state);
}

void Led::toggle()
{
    digitalWrite(pin, !digitalRead(pin));
}

#define NOTIFY_DELAY(TIME) ulTaskNotifyTake(pdTRUE, TIME / portTICK_PERIOD_MS)

void ledTask(void *pvParameters)
{
    LOGD("LED", "LED task started.");
    Led ledTop(PIN_LED_TOP);
    Led ledInside(PIN_LED_INSIDE);
    Led ledBuiltin(LED_BUILTIN);
    ledTop.begin();
    ledInside.begin();
    ledBuiltin.begin();

    while (true)
    {
        // Create a copy of the states to avoid race conditions.
        xSemaphoreTake(stateUpdateMutex, portMAX_DELAY);
        AlarmState alarmStateCopy = alarmState;
        NetworkState networkStateCopy = networkState;
        bool txRequired = deviceManager.txRequired(); // Still liable to change.
        uint32_t lastLoRaTimeCopy = lastLoRaTime;
        xSemaphoreGive(stateUpdateMutex);

        // Handle each state in the priority Alarms > Networking > Other stuff.
        switch (alarmStateCopy)
        {
        case ALARM_HIGH:
            // High priority alarms
            ledBuiltin.set(false);
            ledTop.set(true);
            ledInside.set(true);
            if (!NOTIFY_DELAY(500))
            {
                // No event, keep going
                ledTop.set(false);
                ledInside.set(false);
                NOTIFY_DELAY(500);
            }
            break;

        case ALARM_OFF:
            // No alarm, so use the LEDs for other status indication.
            switch (networkStateCopy)
            {
            case NETWORK_NONE:
                ledBuiltin.set(true); // Show that we are displaying the network state.
                ledTop.set(true);
                ledInside.set(true);
                break;

            case NETWORK_WIFI_CONNECTING:
                // On for a short time, off for long time.
                ledTop.set(false);
                ledInside.set(true);
                ledBuiltin.set(true); // Show that we are displaying the network state.
                if (!NOTIFY_DELAY(100))
                {
                    // No event, keep going
                    ledInside.set(false);
                    NOTIFY_DELAY(900);
                }
                break;

            case NETWORK_MQTT_CONNECTING:
                // On for a long time, off for a short time.
                ledTop.set(false);
                ledInside.set(true);
                ledBuiltin.set(true); // Show that we are displaying the network state.
                if (!NOTIFY_DELAY(900))
                {
                    // No event, keep going
                    ledInside.set(false);
                    NOTIFY_DELAY(100);
                }
                break;

            case NETWORK_CONNECTED:
            default:
                // Network is happy, so show the radio status.
                ledBuiltin.set(false); // Show that we are not displaying the network state.
                ledInside.set(true);
                if (millis() - lastLoRaTimeCopy < LORA_LED_FLASH_TIME)
                {
                    // TX or RX happened recently.
                    ledTop.set(!txRequired);
                }
                else
                {
                    // No TX or RX recently.
                    ledTop.set(txRequired);
                }
            }
            break;

        case ALARM_MEDIUM:
        default:
            // OTher alarms.
            ledTop.set(true);
            ledInside.set(true);
            if (!NOTIFY_DELAY(1000))
            {
                // No event, keep going
                ledTop.set(false);
                ledInside.set(false);
                NOTIFY_DELAY(4000);
            }
        }
        taskYIELD();
    }
}