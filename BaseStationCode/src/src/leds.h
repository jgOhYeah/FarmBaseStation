/**
 * @file leds.h
 * @brief Controls the LEDs on the unit.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-11-05
 */
#pragma once
#include "../defines.h"
#include "alarm.h"
#include "networking.h"
#include "devices.h"

/**
 * @brief Class for controlling an LED.
 *
 */
class Led
{
public:
    /**
     * @brief Construct a new Led object.
     * 
     * @param pin the GPIO pin that the LED is attached to.
     */
    Led(const uint8_t pin) : pin(pin) {}

    /**
     * @brief Initialises the pins
     * 
     */
    void begin();

    /**
     * @brief Called regularly to update the LED during fades and animations.
     *
     */
    void tick();

    /**
     * @brief Turns the LED on or off.
     * 
     * @param state true if on, false is off.
     */
    void set(bool state);

private:
    const uint8_t pin;
};

/**
 * @brief Task that manages the LEDs on the unit.
 *
 */
void ledTask(void *pvParameters);