/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include <stdint.h>

#if defined(ESP_PLATFORM)
#include "esp_timer.h"
#endif

static uint32_t g_ms_tick = 0;

extern "C" uint32_t timersGetMsTick()
{
#if defined(ESP_PLATFORM)
  return (uint32_t)(esp_timer_get_time() / 1000ULL);
#else
  return g_ms_tick;
#endif
}

extern "C" uint32_t timersGetUsTick()
{
#if defined(ESP_PLATFORM)
  return (uint32_t)esp_timer_get_time();
#else
  return g_ms_tick * 1000;
#endif
}

extern "C" void timersInit()
{
  // FreeRTOS + esp_timer provide system time; 10ms EdgeTX timer is OS-level.
}
