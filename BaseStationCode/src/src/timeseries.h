/**
 * @file timeseries.h
 * @brief Measures the temperature and pressure and sends them every so often as timeseries.
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2025-01-4
 */

#pragma once
#include "../defines.h"

#ifdef GENERATE_TIMESERIES
#include "topics.h"

/**
 * @brief Task for taking measurements and sending timeseries data from this device.
 *
 * @param pvParameters
 */
void timeseriesTask(void *pvParameters);
#endif