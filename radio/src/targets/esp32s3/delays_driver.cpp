/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "delays_driver.h"

#if defined(ESP_PLATFORM)
#include "esp_timer.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

void delaysInit() {}

void delay_01us(uint32_t count)
{
#if defined(ESP_PLATFORM)
  // Busy-wait ~0.1us units using CPU cycle approximation at 240MHz
  uint32_t cycles = count * 24;
  esp_rom_delay_us((cycles + 239) / 240);
#else
  (void)count;
#endif
}

void delay_us(uint32_t count)
{
#if defined(ESP_PLATFORM)
  esp_rom_delay_us(count);
#else
  (void)count;
#endif
}

void delay_ms(uint32_t count)
{
#if defined(ESP_PLATFORM)
  vTaskDelay(pdMS_TO_TICKS(count));
#else
  (void)count;
#endif
}

uint32_t ticksNow()
{
#if defined(ESP_PLATFORM)
  return (uint32_t)(esp_timer_get_time() & 0xffffffffu);
#else
  return 0;
#endif
}
