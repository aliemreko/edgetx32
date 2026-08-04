/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 *
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#pragma once

#include <inttypes.h>
#include "delays_driver.h"

// ESP32 has no STM32 unique ID peripheral; implemented in cpu_id.cpp
void getCPUUniqueID(char* s);
