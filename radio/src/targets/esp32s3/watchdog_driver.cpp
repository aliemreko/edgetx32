/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "hal/watchdog_driver.h"

#if defined(ESP_PLATFORM)
#include "esp_task_wdt.h"
#endif

void watchdogInit(unsigned int duration)
{
#if defined(ESP_PLATFORM)
  esp_task_wdt_config_t cfg = {
    .timeout_ms = duration ? duration : WDG_DURATION,
    .idle_core_mask = 0,
    .trigger_panic = true,
  };
  esp_task_wdt_reconfigure(&cfg);
  esp_task_wdt_add(nullptr);
#else
  (void)duration;
#endif
}

void watchdogReset()
{
#if defined(ESP_PLATFORM)
  esp_task_wdt_reset();
#endif
}
