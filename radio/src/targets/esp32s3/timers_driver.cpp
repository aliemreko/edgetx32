/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "timers_driver.h"

#if defined(ESP_PLATFORM)
#include "esp_timer.h"
#endif

uint32_t timersGetMsTick()
{
#if defined(ESP_PLATFORM)
  return (uint32_t)(esp_timer_get_time() / 1000ULL);
#else
  return (uint32_t)g_tmr10ms * 10u;
#endif
}

uint32_t timersGetUsTick()
{
#if defined(ESP_PLATFORM)
  return (uint32_t)esp_timer_get_time();
#else
  return (uint32_t)g_tmr10ms * 10000u;
#endif
}

void timersInit()
{
  // FreeRTOS + esp_timer provide system time; 10ms EdgeTX tick is OS-level.
  g_tmr10ms = 0;
}

void watchdogSuspend(uint32_t timeout)
{
  (void)timeout;
}
