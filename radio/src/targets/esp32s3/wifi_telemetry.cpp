/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "esp32_features.h"

#if defined(ESP_PLATFORM) && defined(ESP32_WIFI_TELEMETRY)
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/sockets.h"

static const char* TAG = "etx-wifi";
static bool s_ready = false;

extern "C" void esp32WifiTelemetryInit()
{
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    nvs_flash_init();
  }
  esp_netif_init();
  esp_event_loop_create_default();
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_mode(WIFI_MODE_STA);
  // Credentials come from radio settings / NVS in a later UI page.
  esp_wifi_start();
  s_ready = true;
  ESP_LOGI(TAG, "WiFi telemetry stack ready (STA)");
}

extern "C" bool esp32WifiTelemetryReady() { return s_ready; }

extern "C" void esp32WifiTelemetrySendJson(const char* json)
{
  if (!s_ready || !json) return;
  // UDP broadcast telemetry mirror — Companion / ground-station consumers.
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock < 0) return;
  sockaddr_in dest = {};
  dest.sin_family = AF_INET;
  dest.sin_port = htons(9070);
  dest.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (int[]){1}, sizeof(int));
  sendto(sock, json, strlen(json), 0, (sockaddr*)&dest, sizeof(dest));
  close(sock);
}
#else
extern "C" void esp32WifiTelemetryInit() {}
extern "C" bool esp32WifiTelemetryReady() { return false; }
extern "C" void esp32WifiTelemetrySendJson(const char*) {}
#endif
