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

#define NOTIFY_DELAY(TIME) ulTaskNotifyTake(pdTRUE, TIME / portTICK_PERIOD_MS)

#ifdef PIN_LED_TOP
#define SET_LED_TOP(STATE) ledTop.set(STATE)
#else
#define SET_LED_TOP(STATE)
#endif
#ifdef PIN_LED_INSIDE
#define SET_LED_INSIDE(STATE) ledInside.set(STATE)
#else
#define SET_LED_INSIDE(STATE)
#endif
void ledTask(void *pvParameters)
{
    LOGD("LED", "LED task started.");
#ifdef PIN_LED_TOP
    Led ledTop(PIN_LED_TOP);
    ledTop.begin();
#endif
#ifdef PIN_LED_INSIDE
    Led ledInside(PIN_LED_INSIDE);
    ledInside.begin();
#endif
    Led ledBuiltin(LED_BUILTIN);
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
            ledBuiltin.set(true);
            SET_LED_TOP(true);
            SET_LED_INSIDE(true);
            if (!NOTIFY_DELAY(500))
            {
                // No event, keep going
                SET_LED_TOP(false);
                SET_LED_INSIDE(false);
                ledBuiltin.set(false);
                NOTIFY_DELAY(500);
            }
            break;

        case ALARM_OFF:
            // No alarm, so use the LEDs for other status indication.
            switch (networkStateCopy)
            {
            case NETWORK_NONE:
                ledBuiltin.set(true); // Show that we are displaying the network state.
                SET_LED_TOP(true);
                SET_LED_INSIDE(true);
                break;

            case NETWORK_WIFI_CONNECTING:
                // On for a short time, off for long time.
                SET_LED_TOP(false);
                SET_LED_INSIDE(true);
                ledBuiltin.set(true); // Show that we are displaying the network state.
                if (!NOTIFY_DELAY(100))
                {
                    // No event, keep going
                    SET_LED_INSIDE(false);
                    ledBuiltin.set(false);
                    NOTIFY_DELAY(900);
                }
                break;

            case NETWORK_MQTT_CONNECTING:
                // On for a long time, off for a short time.
                SET_LED_TOP(false);
                SET_LED_INSIDE(true);
                ledBuiltin.set(true); // Show that we are displaying the network state.
                if (!NOTIFY_DELAY(900))
                {
                    // No event, keep going
                    SET_LED_INSIDE(false);
                    ledBuiltin.set(false);
                    NOTIFY_DELAY(100);
                }
                break;

            case NETWORK_CONNECTED:
            default:
                // Network is happy, so show the radio status.
                // Show that we are not displaying the network state.
                SET_LED_INSIDE(true);
                if (millis() - lastLoRaTimeCopy < LORA_LED_FLASH_TIME)
                {
                    // TX or RX happened recently.
                    SET_LED_TOP(!txRequired);
                    ledBuiltin.set(!txRequired);
                }
                else
                {
                    // No TX or RX recently.
                    SET_LED_TOP(txRequired);
                    ledBuiltin.set(txRequired);
                }
            }
            break;

        case ALARM_MEDIUM:
        default:
            // OTher alarms.
            SET_LED_TOP(true);
            SET_LED_INSIDE(true);
            ledBuiltin.set(true);
            if (!NOTIFY_DELAY(1000))
            {
                // No event, keep going
                SET_LED_TOP(false);
                SET_LED_INSIDE(false);
                ledBuiltin.set(true);
                NOTIFY_DELAY(4000);
            }
        }
        taskYIELD();
    }
}