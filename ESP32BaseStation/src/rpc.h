/**
 * @file rpc.h
 * @brief Handles RPC calls.
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */

#pragma once
#include "../defines.h"
#include "topics.h"
#include "alarm.h"


/**
 * @brief Function that is called when an mqtt message is received.
 *
 */
void mqttReceived(char *topic, byte *message, unsigned int length);

/**
 * @brief Handles an RPC message addressed to this device (not one of the remote devices).
 * 
 * @param id id number as a char array to reply with.
 * @param message the incoming message.
 * @param length the length of the incoming message.
 */
void rpcMe(char *id, uint8_t *message, uint16_t length);

/**
 * @brief Replies to an RPC message for this device.
 * 
 * @param id char array of the request ID.
 * @param reply the payload to return.
 */
void replyMeRpc(char *id, char *reply);

/**
 * @brief Checks if one string starts with another (quicker than strstr?)
 *
 * @param input the string to be checked.
 * @param compare what to look for.
 * @return Whether input starts with compare.
 */
bool startsWith(char *input, const char *compare);