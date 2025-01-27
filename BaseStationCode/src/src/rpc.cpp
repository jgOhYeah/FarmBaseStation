/**
 * @file rpc.cpp
 * @brief Handles RPC calls.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */

#include "rpc.h"

extern PubSubClient mqtt;
extern QueueHandle_t mqttPublishQueue;
extern SemaphoreHandle_t mqttMutex;
extern SemaphoreHandle_t serialMutex;
extern QueueHandle_t alarmQueue;
extern QueueHandle_t audioQueue;
extern DeviceManager deviceManager;

#ifdef PIN_IR
#include "hvacir.h"
extern HVAC airConditioner;
HvacMode airConditionerMode = HVAC_AUTO;
HvacFanMode airConditionerFanMode = FAN_SPEED_AUTO;
int airConditionerTemp = 22;
bool airConditionerOn = true;
#endif

void mqttReceived(char *topic, byte *message, unsigned int length)
{
    // Check that the message isn't too long.
    if (length >= MAX_JSON_TEXT_LENGTH)
    {
        LOGW("MQTT", "Got a %d character message that was longer than the %d supported. Discarding", length, MAX_JSON_TEXT_LENGTH);
        return;
    }

    // Convert to a char array.
    char messageString[MAX_JSON_TEXT_LENGTH];
    memcpy(messageString, message, length);
    messageString[length] = 0;
    LOGD("MQTT", "MQTT received on topic '%s', '%s'.", topic, messageString);

    // Send to the appropriate handler.
    if (startsWith(topic, Topic::RPC_GATEWAY))
    {
        LOGD("MQTT", "MQTT message is RPC for a connected device.");
        rpcGateway(message, length);
    }
    else if (startsWith(topic, Topic::RPC_ME))
    {
        LOGD("MQTT", "MQTT message is RPC me.");
        char id[MAX_ID_TEXT_LENGTH];
        strncpy(id, strrchr(topic, '/') + 1, MAX_ID_TEXT_LENGTH);
        rpcMe(id, message, length);
    }
}

void rpcMe(char *id, uint8_t *message, uint16_t length)
{
    JsonDocument json;
    DeserializationError error = deserializeJson(json, message, length);
    if (error)
    {
        LOGW("MQTT", "Could not deserialise the json document. Discarding");
        return;
    }

    // Handle each known method
    const char *method = json["method"];
    if (method)
    {
        if (STRINGS_MATCH(method, "alarm"))
        {
            // Alarms
            LOGI("MQTT", "Alarm method");

            // Extract the alarm level
            // TODO: Track individual alarms.
            const char *params = json["params"];
            AlarmState state = ALARM_OFF;
            if (params)
            {
                // Might not be any params provided.
                if (STRINGS_MATCH(params, "Critical"))
                {
                    state = ALARM_HIGH;
                }
                else if (STRINGS_MATCH(params, "Major"))
                {
                    state = ALARM_HIGH;
                }
                else if (STRINGS_MATCH(params, "Minor"))
                {
                    state = ALARM_MEDIUM;
                }
            }

            // Add to the queue
            const unsigned int ALARM_TIMEOUT = 3000;
            xQueueSend(alarmQueue, (void *)&state, ALARM_TIMEOUT / portTICK_PERIOD_MS);
            replyMeRpc(id, (char *)"{}");
        }
        else if (STRINGS_MATCH(method, "reset"))
        {
            // Reset
            LOGI("MQTT", "Reset method. Restarting in a few seconds");
            replyMeRpc(id, (char *)"{}");
            delay(10000);
            ESP.restart();
        }
#ifdef PIN_SPEAKER
        else if (STRINGS_MATCH(method, "doorbell"))
        {
            // Doorbell noises
            LOGI("MQTT", "Doorbell");
            AlarmState state = ALARM_DOORBELL;
            xQueueSend(audioQueue, (void *)&state, portMAX_DELAY);
            replyMeRpc(id, (char *)"{}");
        }
#endif
#ifdef PIN_IR
        else if (STRINGS_MATCH(method, "aircond"))
        {
            // Send a signal over IR to the air conditioner.
            LOGI("MQTT", "Air conditioner");
            JsonDocument reply;
            sendAirConditioner(json["params"], reply);
            airConditionerReplySettings(reply);
            char buf[100];
            serializeJson(reply, buf, sizeof(buf));
            replyMeRpc(id, buf);
            setAirConditionerAttribute(buf);
        }
        else if (STRINGS_MATCH(method, "aircondGet"))
        {
            // Return the previously used settings.
            LOGI("MQTT", "Air conditioner get");
            JsonDocument reply;
            airConditionerReplySettings(reply);
            char buf[100];
            serializeJson(reply, buf, sizeof(buf));
            replyMeRpc(id, buf);
        }
#endif
        else
        {
            LOGI("MQTT", "Unrecognised MQTT method '%s' for me.", method);
        }
    }
    else
    {
        LOGI("MQTT", "Method doesn't exist.");
    }
}

void replyMeRpc(char *id, char *reply)
{
    // Build the topic
    size_t rpcBaseLength = strlen(Topic::RPC_ME_RESPOND);
    char topic[rpcBaseLength + MAX_ID_TEXT_LENGTH];
    strcpy(topic, Topic::RPC_ME_RESPOND);
    memcpy(topic + rpcBaseLength, id, MAX_ID_TEXT_LENGTH);

    // Publish
    LOGD("MQTT", "Replying to topic %s", topic);
    // xSemaphoreTake(mqttMutex, portMAX_DELAY);
    mqtt.publish(topic, reply);
    // xSemaphoreGive(mqttMutex);
}

void rpcGateway(uint8_t *message, uint16_t length)
{
    // Deserialise
    JsonDocument json;
    DeserializationError error = deserializeJson(json, message, length);
    if (error)
    {
        LOGW("MQTT", "Could not deserialise the json document. Discarding.");
        return;
    }

    // Find the device this command is for.
    const char *deviceName = json["device"];
    if (!deviceName)
    {
        LOGW("MQTT", "Device not provided. Discarding.");
        return;
    }
    Device *device = deviceManager.getWithName(deviceName);
    if (!device)
    {
        LOGW("MQTT", "Device '%s' not recognised. Discarding.", deviceName);
        return;
    }

    // Find the field.
    JsonObject data = json["data"];
    if (!data)
    {
        LOGW("MQTT", "No data provided.");
        return;
    }
    const char *method = data["method"];
    LOGD("MQTT", "Method is %s", method);
    if (!method)
    {
        LOGW("MQTT", "Method or data not provided.");
        return;
    }
    Field *field = device->fields.getWithName(method);
    if (!field)
    {
        LOGW("MQTT", "Field not recognised.");
        return;
    }
    else
    {
        LOGD("MQTT", "Field is '%s'", field->name);
    }

    // Handle the RPC call
    JsonDocument reply;
    JsonObject replyData = reply["data"].to<JsonObject>();
    field->handleRpc(data, replyData);

    // Add the other metadata and send the reply
    reply["id"] = data["id"];
    reply["device"] = deviceName;

    char replyText[MAX_JSON_TEXT_LENGTH];
    serializeJson(reply, replyText, MAX_JSON_TEXT_LENGTH);

    LOGD("MQTT", "Replying with payload '%s'", replyText);
    // xSemaphoreTake(mqttMutex, portMAX_DELAY);
    mqtt.publish(Topic::RPC_GATEWAY, replyText);
    // xSemaphoreGive(mqttMutex);
}

bool startsWith(char *input, const char *compare)
{
    for (int i = 0; compare[i] != '\00'; i++)
    {
        if (input[i] != compare[i])
        {
            return false;
        }
    }
    return true;
}

void setAttributeState(const char *const attribute, bool state)
{
    MqttMsg msg{Topic::ATTRIBUTE_ME_UPLOAD, ""};
    JsonDocument json;
    json[attribute] = state;
    serializeJson(json, msg.payload, MAX_JSON_TEXT_LENGTH);
    xQueueSend(mqttPublishQueue, (void *)&msg, portMAX_DELAY);
}

void setVersionAttribute()
{
    char buf[400]; // Longer than a standard MqttMsg object as lots of details.
    JsonDocument json;
    JsonObject version = json["version"].to<JsonObject>();
    version["date"] = __DATE__;
    version["time"] = __TIME__;
    version["version"] = VERSION;
    version["method"] = CONNECTION_METHOD;
    version["pio-env"] = PIO_ENV;
    version["pio-plat"] = PIO_PLATFORM;
    version["pio-plat-vers"] = PIO_PLATFORM_VERSION;
    version["pio-framework"] = PIO_FRAMEWORK;
    version["connect-time"] = millis(); // So we can see if this reset itself recently.
    version["reset-reas"] = resetReasonName(esp_reset_reason());
    serializeJson(json, buf, sizeof(buf));
    // Don't use a queue as that may be full if reconnecting after a long time being disconnected.
    xSemaphoreTake(mqttMutex, portMAX_DELAY);
    LOGD("RPC", "Sending version attribute.");
    mqtt.publish(Topic::ATTRIBUTE_ME_UPLOAD, buf);
    xSemaphoreGive(mqttMutex);
}

const char *resetReasonName(esp_reset_reason_t reason)
{
    switch (reason)
    {
    case ESP_RST_UNKNOWN:
        return "Unknown";
    case ESP_RST_POWERON:
        return "PowerOn"; // Power on or RST pin toggled
    case ESP_RST_EXT:
        return "ExtPin"; // External pin - not applicable for ESP32
    case ESP_RST_SW:
        return "Reboot"; // esp_restart()
    case ESP_RST_PANIC:
        return "Crash"; // Exception/panic
    case ESP_RST_INT_WDT:
        return "WDT_Int"; // Interrupt watchdog (software or hardware)
    case ESP_RST_TASK_WDT:
        return "WDT_Task"; // Task watchdog
    case ESP_RST_WDT:
        return "WDT_Other"; // Other watchdog
    case ESP_RST_DEEPSLEEP:
        return "Sleep"; // Reset after exiting deep sleep mode
    case ESP_RST_BROWNOUT:
        return "BrownOut"; // Brownout reset (software or hardware)
    case ESP_RST_SDIO:
        return "SDIO"; // Reset over SDIO
    default:
        return "";
    }
}

#ifdef PIN_IR
#define AIR_CONDITIONER_ERROR(DESC) \
    LOGI("IR", DESC);               \
    reply["desc"] = DESC;           \
    reply["result"] = false;        \
    return false
bool sendAirConditioner(JsonObject obj, JsonDocument &reply)
{
    // Air conditioner mode.
    const char *modeStr = obj["mode"];
    if (modeStr)
    {
        if (STRINGS_MATCH(modeStr, "heat"))
        {
            airConditionerMode = HVAC_HOT;
        }
        else if (STRINGS_MATCH(modeStr, "cool"))
        {
            airConditionerMode = HVAC_COLD;
        }
        else if (STRINGS_MATCH(modeStr, "dry"))
        {
            airConditionerMode = HVAC_DRY;
        }
        else if (STRINGS_MATCH(modeStr, "auto"))
        {
            airConditionerMode = HVAC_AUTO;
        }
        else
        {
            AIR_CONDITIONER_ERROR("Invalid mode");
        }
    }

    // Air conditioner fan mode.
    const char *fanModeStr = obj["fanmode"];
    if (fanModeStr)
    {
        if (STRINGS_MATCH(fanModeStr, "FS1"))
        {
            airConditionerFanMode = FAN_SPEED_1;
        }
        else if (STRINGS_MATCH(fanModeStr, "FS2"))
        {
            airConditionerFanMode = FAN_SPEED_2;
        }
        else if (STRINGS_MATCH(fanModeStr, "FS3"))
        {
            airConditionerFanMode = FAN_SPEED_3;
        }
        else if (STRINGS_MATCH(fanModeStr, "FS4"))
        {
            airConditionerFanMode = FAN_SPEED_4;
        }
        else if (STRINGS_MATCH(fanModeStr, "FS5"))
        {
            airConditionerFanMode = FAN_SPEED_5;
        }
        else if (STRINGS_MATCH(fanModeStr, "auto"))
        {
            airConditionerFanMode = FAN_SPEED_AUTO;
        }
        else
        {
            AIR_CONDITIONER_ERROR("Invalid fan mode");
        }
    }

    // Get the temperature and whether the air conditioner should be on.
    if (obj["temperature"].is<int>())
    {
        airConditionerTemp = obj["temperature"];
    }
    if (obj["on"].is<bool>())
    {
        airConditionerOn = obj["on"];
    }

    airConditioner.sendHvacToshiba(airConditionerMode, airConditionerTemp, airConditionerFanMode, !airConditionerOn);
    reply["result"] = true;
    reply["desc"] = "";
    return true;
}

void airConditionerReplySettings(JsonDocument &obj)
{
    // Mode
    switch (airConditionerMode)
    {
    case HVAC_HOT:
        obj["mode"] = "heat";
        break;
    case HVAC_COLD:
        obj["mode"] = "cool";
        break;
    case HVAC_DRY:
        obj["mode"] = "dry";
        break;
    case HVAC_AUTO:
        obj["mode"] = "auto";
        break;
    default:
        obj["mode"] = "unknown";
    }

    // Fan mode
    switch (airConditionerFanMode)
    {
    case FAN_SPEED_1:
        obj["fanmode"] = "FS1";
        break;
    case FAN_SPEED_2:
        obj["fanmode"] = "FS2";
        break;
    case FAN_SPEED_3:
        obj["fanmode"] = "FS3";
        break;
    case FAN_SPEED_4:
        obj["fanmode"] = "FS4";
        break;
    case FAN_SPEED_5:
        obj["fanmode"] = "FS5";
        break;
    case FAN_SPEED_AUTO:
        obj["fanmode"] = "auto";
        break;
    default:
        obj["fanmode"] = "unknown";
    }

    // Temperature
    obj["temperature"] = airConditionerTemp;

    // On and off
    obj["on"] = airConditionerOn;
}

void setAirConditionerAttribute(const char *payload)
{
    MqttMsg msg = {Topic::ATTRIBUTE_ME_UPLOAD, ""};
    sprintf(msg.payload, "{\"aircond\":%s}", payload); // Add inside a key to make this a bit neater.
    xQueueSend(mqttPublishQueue, (void *)&msg, portMAX_DELAY);
}

void setAirConditionerAttributeInitial()
{
    JsonDocument result;
    airConditionerReplySettings(result);
    char payload[200];
    char buf[200];
    serializeJson(result, payload, sizeof(payload));
    sprintf(buf, "{\"aircond\":%s}", payload); // Add inside a key to make this a bit neater.
    // Don't use a queue as that may be full if reconnecting after a long time being disconnected.
    xSemaphoreTake(mqttMutex, portMAX_DELAY);
    LOGD("RPC", "Sending air conditioner attributes.");
    mqtt.publish(Topic::ATTRIBUTE_ME_UPLOAD, buf);
    xSemaphoreGive(mqttMutex);
}
#endif