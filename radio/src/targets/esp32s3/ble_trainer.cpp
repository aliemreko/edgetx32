/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "esp32_features.h"

#if defined(ESP_PLATFORM) && defined(ESP32_BLE_TRAINER)
#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"

static const char* TAG = "etx-ble";

extern "C" void esp32BleTrainerInit()
{
  ESP_LOGI(TAG, "BLE trainer transport placeholder (NimBLE)");
  // Expose CRSF/SBUS channel mirror as BLE UART-style service for wireless trainer link.
}

extern "C" void esp32BleTrainerSendChannels(const int16_t* channels, uint8_t n)
{
  (void)channels; (void)n;
}
#else
extern "C" void esp32BleTrainerInit() {}
extern "C" void esp32BleTrainerSendChannels(const int16_t*, uint8_t) {}
#endif
