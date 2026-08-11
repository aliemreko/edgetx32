/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * Symbols normally provided by main.cpp / lua until those modules are linked.
 */

#include <stdint.h>

uint8_t requiredSpeakerVolume = 255;

volatile struct HeartbeatCapture {
  uint8_t valid;
#if defined(DEBUG_LATENCY)
  uint32_t count;
#endif
} heartbeatCapture = {};

extern "C" void luaInitMainState() {}
void luaInit() {}
void luaClose() {}
void luaTask() {}
