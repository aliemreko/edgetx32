/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * Symbols normally provided by main.cpp / lua until those modules are linked.
 */

#include "heartbeat_driver.h"

#include <stdint.h>

uint8_t requiredSpeakerVolume = 255;

volatile HeartbeatCapture heartbeatCapture = {};

extern "C" void luaInitMainState() {}
void luaInit() {}
void luaClose() {}
void luaTask() {}
