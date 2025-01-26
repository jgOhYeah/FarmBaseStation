/**
 * @file hvacir.h
 * @brief Functions for sending IR commands to an air conditioner.
 * @author Vincent Cruvellier
 * @date 2016, modified Jan 2025.
 * Based on https://github.com/r45635/HVAC-IR-Control/blob/master/HVAC_ESP8266/HVAC_ESP8266T.ino
 */
#pragma once
#include <Arduino.h>

enum HvacMode
{
    HVAC_HOT,
    HVAC_COLD,
    HVAC_DRY,
    HVAC_FAN, // used for Panasonic only
    HVAC_AUTO
}; // HVAC  MODE

enum HvacFanMode
{
    FAN_SPEED_1,
    FAN_SPEED_2,
    FAN_SPEED_3,
    FAN_SPEED_4,
    FAN_SPEED_5,
    FAN_SPEED_AUTO,
    FAN_SPEED_SILENT
}; // HVAC  FAN MODE

enum HvacVanneMode
{
    VANNE_AUTO,
    VANNE_H1,
    VANNE_H2,
    VANNE_H3,
    VANNE_H4,
    VANNE_H5,
    VANNE_AUTO_MOVE
}; // HVAC  VANNE MODE

enum HvacWideVanneMode
{
    WIDE_LEFT_END,
    WIDE_LEFT,
    WIDE_MIDDLE,
    WIDE_RIGHT,
    WIDE_RIGHT_END
}; // HVAC  WIDE VANNE MODE

enum HvacAreaMode
{
    AREA_SWING,
    AREA_LEFT,
    AREA_AUTO,
    AREA_RIGHT
}; // HVAC  WIDE VANNE MODE

enum HvacProfileMode
{
    NORMAL,
    QUIET,
    BOOST
}; // HVAC PANASONIC OPTION MODE

// HVAC TOSHIBA_
#define HVAC_TOSHIBA_KHZ 38
#define HVAC_TOSHIBA_HALF_PERIODIC_TIME (500 / HVAC_TOSHIBA_KHZ)
#define HVAC_TOSHIBA_HDR_MARK 4400
#define HVAC_TOSHIBA_HDR_SPACE 4300
#define HVAC_TOSHIBA_BIT_MARK 543
#define HVAC_TOSHIBA_ONE_SPACE 1623
#define HVAC_MISTUBISHI_ZERO_SPACE 472
#define HVAC_TOSHIBA_RPT_MARK 440
#define HVAC_TOSHIBA_RPT_SPACE 7048 // Above original iremote limit

/**
 * @brief Class for remotely controlling air conditioners over IR.
 * Based on https://github.com/r45635/HVAC-IR-Control/blob/master/HVAC_ESP8266/HVAC_ESP8266T.ino
 *
 */
class HVAC
{
public:
    /**
     * @brief Construct a new HVAC object.
     *
     * @param pinIR pin connected to the IR LED.
     */
    HVAC(const uint8_t pinIR) : m_pinIR(pinIR) {}

    /**
     * @brief Sets the IR pin as an output.
     * 
     */
    void begin();

    /**
     * @brief Sends a command to a Toshiba air conditioner using IR.
     *
     * @param mode air conditioner mode. For example, HVAC_HOT
     * @param temperature set temperature. For example, 21 = 21 degrees C.
     * @param fanMode fan mode. For example, FAN_SPEED_AUTO.
     * @param turnOff whether to turn off the air conditioner. true is turn off, false is turn on.
     */
    void sendHvacToshiba(HvacMode mode, int temperature, HvacFanMode fanMode, int turnOff);

private:
    /**
     * @brief Sends an IR mark for the specified number of microseconds.
     * The mark output is modulated at the PWM frequency.
     *
     * @param time the time in us.
     */
    void m_mark(int time);

    /**
     * @brief Sends an IR space for the specified number of microseconds.
     * A space is no output, so the PWM output is disabled.
     * 
     * @param time the time in us.
     */
    void m_space(int time);
    
    /**
     * @brief Sends raw data over IR.
     * 
     * @param buf 
     * @param len 
     */
    void m_sendRaw(unsigned int buf[], int len);

    const uint8_t m_pinIR;
};