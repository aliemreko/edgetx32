/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#pragma once

#include "edgetx_types.h"

extern "C" volatile tmr10ms_t g_tmr10ms;
static inline tmr10ms_t get_tmr10ms() { return g_tmr10ms; }

void watchdogSuspend(uint32_t timeout);

void timersInit();

uint32_t timersGetMsTick();
uint32_t timersGetUsTick();

void per5ms();
void per10ms();
