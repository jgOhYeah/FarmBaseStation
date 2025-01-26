/**
 * @file hvacir.h
 * @brief Functions for sending IR commands to an air conditioner.
 * @author Vincent Cruvellier
 * @date 2016, modified Jan 2025
 * Based on https://github.com/r45635/HVAC-IR-Control/blob/master/HVAC_ESP8266/HVAC_ESP8266T.ino
 */
#include "hvacir.h"

void HVAC::begin()
{
    pinMode(m_pinIR, OUTPUT);
}

void HVAC::sendHvacToshiba(HvacMode mode, int temperature, HvacFanMode fanMode, int turnOff)
{

#define HVAC_TOSHIBA_DATALEN 9
    // #define HVAC_TOSHIBA_DEBUG ; // Un comment to access DEBUG information through Serial Interface

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
    switch (mode)
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
    if (turnOff)
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
    if (temperature > 30)
    {
        Temp = 30;
    }
    else if (temperature < 17)
    {
        Temp = 17;
    }
    else
    {
        Temp = temperature;
    };
    data[5] = (uint8_t)Temp - 17 << 4;

    // Byte 10 - FAN / VANNE
    switch (fanMode)
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

    m_space(0);
    for (int j = 0; j < 2; j++)
    { // For Mitsubishi IR protocol we have to send two time the packet data
        // Header for the Packet
        m_mark(HVAC_TOSHIBA_HDR_MARK);
        m_space(HVAC_TOSHIBA_HDR_SPACE);
        for (i = 0; i < HVAC_TOSHIBA_DATALEN; i++)
        {
            // Send all Bits from Byte Data in Reverse Order
            for (mask = 0b10000000; mask > 0; mask >>= 1)
            { // iterate through bit mask
                if (data[i] & mask)
                { // Bit ONE
                    m_mark(HVAC_TOSHIBA_BIT_MARK);
                    m_space(HVAC_TOSHIBA_ONE_SPACE);
                }
                else
                { // Bit ZERO
                    m_mark(HVAC_TOSHIBA_BIT_MARK);
                    m_space(HVAC_MISTUBISHI_ZERO_SPACE);
                }
                // Next bits
            }
        }
        // End of Packet and retransmission of the Packet
        m_mark(HVAC_TOSHIBA_RPT_MARK);
        m_space(HVAC_TOSHIBA_RPT_SPACE);
        m_space(0); // Just to be sure
    }
}

void HVAC::m_mark(int time)
{
    long beginning = micros();
    while (micros() - beginning < time)
    {
        digitalWrite(m_pinIR, HIGH);
        delayMicroseconds(HVAC_TOSHIBA_HALF_PERIODIC_TIME);
        digitalWrite(m_pinIR, LOW);
        delayMicroseconds(HVAC_TOSHIBA_HALF_PERIODIC_TIME); // 38 kHz -> T = 26.31 microsec (periodic time), half of it is 13
    }
}

void HVAC::m_space(int time)
{
    digitalWrite(m_pinIR, LOW);
    if (time > 0)
        delayMicroseconds(time);
}

void HVAC::m_sendRaw(unsigned int buf[], int len)
{
    for (int i = 0; i < len; i++)
    {
        if (i & 1)
        {
            m_space(buf[i]);
        }
        else
        {
            m_mark(buf[i]);
        }
    }
    m_space(0); // Just to be sure
}
