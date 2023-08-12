/**
 * @file lora.h
 * @brief File that handles the LoRa radio and PJON.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */

#pragma once
#include "../defines.h"
#include "lookups.h"
#include "devices.h"

/**
 * @brief Handles an incoming packet received from the radio.
 *
 * @param payload the data in the packet.
 * @param length the length of the payload.
 * @param packet_info information about the packet.
 */
void pjonReceive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info);

/**
 * @brief "Receives" some fake data for testing purposes.
 *
 */
void fakeReceive();