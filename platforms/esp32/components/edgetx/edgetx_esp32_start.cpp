/*
 * Bridge between ESP-IDF app_main and EdgeTX firmware main.
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "edgetx-start";

extern "C" void edgeTxEsp32Main();

static void edgetx_main_task(void*)
{
  ESP_LOGI(TAG, "edgeTxEsp32Main()");
  edgeTxEsp32Main();
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

extern "C" void edgetx_esp32_start()
{
  xTaskCreatePinnedToCore(
      edgetx_main_task, "edgetx", 8192, nullptr, 5, nullptr, 0);
}
