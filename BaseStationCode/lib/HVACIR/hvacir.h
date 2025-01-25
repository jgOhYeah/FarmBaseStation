/**
 * @file hvacir.h
 * @brief Functions for sending IR commands to an air conditioner.
 * @author Vincent Cruvellier
 * @date 2016
 * Based on https://github.com/r45635/HVAC-IR-Control/blob/master/HVAC_ESP8266/HVAC_ESP8266T.ino
 */
#pragma once
#include <Arduino.h>

int halfPeriodicTime;
int IRpin;
int khz;

typedef enum HvacMode
{
    HVAC_HOT,
    HVAC_COLD,
    HVAC_DRY,
    HVAC_FAN, // used for Panasonic only
    HVAC_AUTO
} HvacMode_t; // HVAC  MODE

typedef enum HvacFanMode
{
    FAN_SPEED_1,
    FAN_SPEED_2,
    FAN_SPEED_3,
    FAN_SPEED_4,
    FAN_SPEED_5,
    FAN_SPEED_AUTO,
    FAN_SPEED_SILENT
} HvacFanMode_; // HVAC  FAN MODE

typedef enum HvacVanneMode
{
    VANNE_AUTO,
    VANNE_H1,
    VANNE_H2,
    VANNE_H3,
    VANNE_H4,
    VANNE_H5,
    VANNE_AUTO_MOVE
} HvacVanneMode_; // HVAC  VANNE MODE

typedef enum HvacWideVanneMode
{
    WIDE_LEFT_END,
    WIDE_LEFT,
    WIDE_MIDDLE,
    WIDE_RIGHT,
    WIDE_RIGHT_END
} HvacWideVanneMode_t; // HVAC  WIDE VANNE MODE

typedef enum HvacAreaMode
{
    AREA_SWING,
    AREA_LEFT,
    AREA_AUTO,
    AREA_RIGHT
} HvacAreaMode_t; // HVAC  WIDE VANNE MODE

typedef enum HvacProfileMode
{
    NORMAL,
    QUIET,
    BOOST
} HvacProfileMode_t; // HVAC PANASONIC OPTION MODE

// HVAC TOSHIBA_
#define HVAC_TOSHIBA_HDR_MARK 4400
#define HVAC_TOSHIBA_HDR_SPACE 4300
#define HVAC_TOSHIBA_BIT_MARK 543
#define HVAC_TOSHIBA_ONE_SPACE 1623
#define HVAC_MISTUBISHI_ZERO_SPACE 472
#define HVAC_TOSHIBA_RPT_MARK 440
#define HVAC_TOSHIBA_RPT_SPACE 7048 // Above original iremote limit

/***************************************************************************/
/* Send IR command to Toshiba HVAC - sendHvacToshiba                       */
/***************************************************************************/
void sendHvacToshiba(
    HvacMode HVAC_Mode,       // Example HVAC_HOT
    int HVAC_Temp,            // Example 21  (°c)
    HvacFanMode HVAC_FanMode, // Example FAN_SPEED_AUTO
    int OFF                   // Example false
)
{

#define HVAC_TOSHIBA_DATALEN 9
#define HVAC_TOSHIBA_DEBUG ; // Un comment to access DEBUG information through Serial Interface

    uint8_t mask = 1; // our bitmask
    // ﻿F20D03FC0150000051
    uint8_t data[HVAC_TOSHIBA_DATALEN] = {0xF2, 0x0D, 0x03, 0xFC, 0x01, 0x00, 0x00, 0x00, 0x00};
    // data array is a valid trame, only byte to be chnaged will be updated.

    uint8_t i;

#ifdef HVAC_TOSHIBA_DEBUG
    Serial.println("Packet to send: ");
    for (i = 0; i < HVAC_TOSHIBA_DATALEN; i++)
    {
        Serial.print("_");
        Serial.print(data[i], HEX);
    }
    Serial.println(".");
#endif

    data[6] = 0x00;
    // Byte 7 - Mode
    switch (HVAC_Mode)
    {
    case HVAC_HOT:
        data[6] = (uint8_t)0b00000011;
        break;
    case HVAC_COLD:
        data[6] = (uint8_t)0b00000001;
        break;
    case HVAC_DRY:
        data[6] = (uint8_t)0b00000010;
        break;
    case HVAC_AUTO:
        data[6] = (uint8_t)0b00000000;
        break;
    default:
        break;
    }

    // Byte 7 - On / Off
    if (OFF)
    {
        data[6] = (uint8_t)0x07; // Turn OFF HVAC
    }
    else
    {
        // Turn ON HVAC (default)
    }

    // Byte 6 - Temperature
    // Check Min Max For Hot Mode
    uint8_t Temp;
    if (HVAC_Temp > 30)
    {
        Temp = 30;
    }
    else if (HVAC_Temp < 17)
    {
        Temp = 17;
    }
    else
    {
        Temp = HVAC_Temp;
    };
    data[5] = (uint8_t)Temp - 17 << 4;

    // Byte 10 - FAN / VANNE
    switch (HVAC_FanMode)
    {
    case FAN_SPEED_1:
        data[6] = data[6] | (uint8_t)0b01000000;
        break;
    case FAN_SPEED_2:
        data[6] = data[6] | (uint8_t)0b01100000;
        break;
    case FAN_SPEED_3:
        data[6] = data[6] | (uint8_t)0b10000000;
        break;
    case FAN_SPEED_4:
        data[6] = data[6] | (uint8_t)0b10100000;
        break;
    case FAN_SPEED_5:
        data[6] = data[6] | (uint8_t)0b11000000;
        break;
    case FAN_SPEED_AUTO:
        data[6] = data[6] | (uint8_t)0b00000000;
        break;
    case FAN_SPEED_SILENT:
        data[6] = data[6] | (uint8_t)0b00000000;
        break; // No FAN speed SILENT for TOSHIBA so it is consider as Speed AUTO
    default:
        break;
    }

    // Byte 9 - CRC
    data[8] = 0;
    for (i = 0; i < HVAC_TOSHIBA_DATALEN - 1; i++)
    {
        data[HVAC_TOSHIBA_DATALEN - 1] = (uint8_t)data[i] ^ data[HVAC_TOSHIBA_DATALEN - 1]; // CRC is a simple bits addition
    }

#ifdef HVAC_TOSHIBA_DEBUG
    Serial.println("Packet to send: ");
    for (i = 0; i < HVAC_TOSHIBA_DATALEN; i++)
    {
        Serial.print("_");
        Serial.print(data[i], HEX);
    }
    Serial.println(".");
    for (i = 0; i < HVAC_TOSHIBA_DATALEN; i++)
    {
        Serial.print(data[i], BIN);
        Serial.print(" ");
    }
    Serial.println(".");
#endif

    enableIROut(38); // 38khz
    space(0);
    for (int j = 0; j < 2; j++)
    { // For Mitsubishi IR protocol we have to send two time the packet data
        // Header for the Packet
        mark(HVAC_TOSHIBA_HDR_MARK);
        space(HVAC_TOSHIBA_HDR_SPACE);
        for (i = 0; i < HVAC_TOSHIBA_DATALEN; i++)
        {
            // Send all Bits from Byte Data in Reverse Order
            for (mask = 10000000; mask > 0; mask >>= 1)
            { // iterate through bit mask
                if (data[i] & mask)
                { // Bit ONE
                    mark(HVAC_TOSHIBA_BIT_MARK);
                    space(HVAC_TOSHIBA_ONE_SPACE);
                }
                else
                { // Bit ZERO
                    mark(HVAC_TOSHIBA_BIT_MARK);
                    space(HVAC_MISTUBISHI_ZERO_SPACE);
                }
                // Next bits
            }
        }
        // End of Packet and retransmission of the Packet
        mark(HVAC_TOSHIBA_RPT_MARK);
        space(HVAC_TOSHIBA_RPT_SPACE);
        space(0); // Just to be sure
    }
}

/****************************************************************************
/* enableIROut : Set global Variable for Frequency IR Emission
/***************************************************************************/
void enableIROut(int khz)
{
    // Enables IR output.  The khz value controls the modulation frequency in kilohertz.
    halfPeriodicTime = 500 / khz; // T = 1/f but we need T/2 in microsecond and f is in kHz
}

/****************************************************************************
/* mark ( int time)
/***************************************************************************/
void mark(int time)
{
    // Sends an IR mark for the specified number of microseconds.
    // The mark output is modulated at the PWM frequency.
    long beginning = micros();
    while (micros() - beginning < time)
    {
        digitalWrite(IRpin, HIGH);
        delayMicroseconds(halfPeriodicTime);
        digitalWrite(IRpin, LOW);
        delayMicroseconds(halfPeriodicTime); // 38 kHz -> T = 26.31 microsec (periodic time), half of it is 13
    }
}

/****************************************************************************
/* space ( int time)
/***************************************************************************/
/* Leave pin off for time (given in microseconds) */
void space(int time)
{
    // Sends an IR space for the specified number of microseconds.
    // A space is no output, so the PWM output is disabled.
    digitalWrite(IRpin, LOW);
    if (time > 0)
        delayMicroseconds(time);
}

/****************************************************************************
/* sendRaw (unsigned int buf[], int len, int hz)
/***************************************************************************/
void sendRaw(unsigned int buf[], int len, int hz)
{
    enableIROut(hz);
    for (int i = 0; i < len; i++)
    {
        if (i & 1)
        {
            space(buf[i]);
        }
        else
        {
            mark(buf[i]);
        }
    }
    space(0); // Just to be sure
}

/****************************************************************************
/* setup ()
*  Note: That's also the entry point after DeepSleep TimeOut
*/
/***************************************************************************/
void setup()
{
    // put your setup code here, to run once:
    IRpin = 5;
    khz = 38;
    halfPeriodicTime = 500 / khz;
    pinMode(IRpin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.begin(115200);
    while (!Serial && millis() < 20000)
        ;
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Demo Started.");
}

/****************************************************************************
/* Loop ()
/***************************************************************************/
void loop()
{
    Serial.begin(115200);
    Serial.println("");

    Serial.println("Switch OFF and Wait 5 Seconds to send ON command.");
    sendHvacToshiba(HVAC_AUTO, 20, FAN_SPEED_AUTO, false);
    // sendHvacMitsubishi(HVAC_HOT, 21, FAN_SPEED_AUTO, VANNE_AUTO_MOVE, true);
    delay(5000);
    Serial.println("Switch ON and Wait 30 Seconds to send OFF command.");
    sendHvacToshiba(HVAC_AUTO, 30, FAN_SPEED_AUTO, false);
    delay(5000);

    sendHvacToshiba(HVAC_AUTO, 30, FAN_SPEED_1, false);
    delay(1000);
    sendHvacToshiba(HVAC_AUTO, 30, FAN_SPEED_2, false);
    delay(1000);
    sendHvacToshiba(HVAC_AUTO, 30, FAN_SPEED_3, false);
    delay(1000);
    sendHvacToshiba(HVAC_AUTO, 30, FAN_SPEED_4, false);
    delay(1000);
    sendHvacToshiba(HVAC_AUTO, 30, FAN_SPEED_5, false);
    delay(1000);
    sendHvacToshiba(HVAC_HOT, 22, FAN_SPEED_AUTO, false);
    delay(1000);
    sendHvacToshiba(HVAC_DRY, 22, FAN_SPEED_AUTO, false);
    delay(1000);
    sendHvacToshiba(HVAC_COLD, 22, FAN_SPEED_AUTO, false);
    delay(1000);
    sendHvacToshiba(HVAC_HOT, 30, FAN_SPEED_AUTO, true);
    //  Serial.println("Deep Sleep");
    // ESP.deepSleep(10000000, WAKE_NO_RFCAL); /// WAKE_RF_DISABLED
    // Never go here :=> a reset will be performed !
}
