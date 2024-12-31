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
#include "networking.h"
#include "rpc.h"

/**
 * @brief Handles an incoming packet received from the radio. Uses the latest
 * received snr and rssi.
 *
 * @param payload the data in the packet.
 * @param length the length of the payload.
 * @param packetInfo information about the packet.
 */
void pjonReceive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packetInfo);

/**
 * @brief Handles an incoming packet received from the radio and adds the given
 * snr and rssi to the packet.
 *
 * @param payload the data in the packet.
 * @param length the length of the payload.
 * @param packetInfo information about the packet.
 * @param rssi the rssi of the received packet.
 * @param snr the received snr.
 */
void pjonReceive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packetInfo, int rssi, float snr);

/**
 * @brief Logs PJON errors.
 *
 * @param code the error code.
 * @param data data relating to the error.
 * @param customPointer
 */
void pjonError(uint8_t code, uint16_t data, void *customPointer);

/**
 * @brief "Receives" some fake data for testing purposes.
 *
 */
// void fakeReceiveTask(void *pvParameters);

/**
 * @brief Task that manages the pjon protocol and radio.
 *
 */
void pjonTask(void *pvParameters);

/**
 * @brief Task that checks if the radio is connected.
 *
 * @param pvParameters
 */
void loraWatchdogTask(void *pvParameters);

/**
 * @brief Task for sending packets when needed.
 *
 * @param pvParameters
 */
void loraTxTask(void *pvParameters);

/**
 * @brief Task for printing out a byte array.
 *
 * @param payload
 * @param length
 */
void debugLoRaPacket(uint8_t *payload, uint8_t length);

/**
 * @brief Sends an attribute over mqtt depending on the state of the radio
 * connection.
 *
 */
void sendRadioConnectedMsg(bool state);

/**
 * @brief Sets the state for if there are packets left to send.
 *
 */
void sendTxWaitingMsg(bool state);

/**
 * @brief Updates the attribute waiting to false if required.
 *
 * The update occurs if no more messages need to be sent and a message was sent in the previous iteration (previous is true). If so, false is returned, otherwise previous.
 *
 * @return the new value for whether there was a message to send in the previous
 * loop.
 */
bool sendOnTxNotRequired(bool previous);