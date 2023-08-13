/**
 * @file topics.h
 * @brief MQTT topics for a thingsboard gateway.
 * 
 * Can't at this stage use the Thingsboard Arduino library as it doesn't
 * support gateway mode.
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-09
 */
#pragma once
namespace Topic
{
    const char* const DEVICE_CONNECT = "v1/gateway/connect";
    const char* const DEVICE_DISCONNECT = "v1/gateway/disconnect";   
    const char* const TELEMETRY_UPLOAD = "v1/gateway/telemetry";
    const char* const RPC_GATEWAY = "v1/gateway/rpc";
    const char* const RPC_ME = "v1/devices/me/rpc/request/";
    const char* const RPC_ME_SUBSCRIBE = "v1/devices/me/rpc/request/+";
    const char* const RPC_ME_RESPOND = "v1/devices/me/rpc/response/";
}