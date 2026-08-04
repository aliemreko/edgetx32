/*
 * EdgeTX ESP32-S3 entry point (ESP-IDF)
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

static const char* TAG = "edgetx";

// EdgeTX firmware entry (linked from edgetx component)
extern "C" void edgetx_esp32_start();

extern "C" void app_main(void)
{
  ESP_LOGI(TAG, "EdgeTX-ESP32 starting");
  ESP_LOGI(TAG, "Internal heap: %u  PSRAM: %u",
           (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
           (unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

  edgetx_esp32_start();

  // EdgeTX starts its own FreeRTOS tasks; keep app_main alive as idle helper.
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
