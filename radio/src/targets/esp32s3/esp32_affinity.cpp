/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "esp32_features.h"

#if defined(ESP_PLATFORM) && defined(ESP32_DUAL_CORE)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char* TAG = "etx-affinity";

static void pin_current(BaseType_t core)
{
  TaskHandle_t self = xTaskGetCurrentTaskHandle();
  // ESP-IDF: vTaskCoreAffinitySet when available; else recreate pinned.
#if defined(configUSE_CORE_AFFINITY) && configUSE_CORE_AFFINITY
  vTaskCoreAffinitySet(self, 1 << core);
  ESP_LOGI(TAG, "Pinned %s to core %d", pcTaskGetName(self), (int)core);
#else
  (void)self; (void)core;
#endif
}

void esp32PinMixerToCore1() { pin_current(1); }
void esp32PinUiToCore0() { pin_current(0); }
#else
void esp32PinMixerToCore1() {}
void esp32PinUiToCore0() {}
#endif
