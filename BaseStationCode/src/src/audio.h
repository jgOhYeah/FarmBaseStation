/**
 * @file audio.h
 * @brief Plays audio
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-18
 */

#pragma once
#include "../defines.h"

#ifdef PIN_SPEAKER
#include "alarm.h"

/**
 * @brief Task for playing audio
 * 
 * @param pvParameters 
 */
void audioTask(void *pvParameters);
#endif