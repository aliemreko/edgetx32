/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * ESP32 has no STM32 intmodule heartbeat EXTI path; provide a no-op HAL.
 */

#pragma once

#include <stdint.h>

struct HeartbeatCapture {
  uint8_t valid;
#if defined(DEBUG_LATENCY)
  uint32_t count;
#endif
};

extern volatile HeartbeatCapture heartbeatCapture;

inline void init_intmodule_heartbeat() {}
inline void stop_intmodule_heartbeat() {}
