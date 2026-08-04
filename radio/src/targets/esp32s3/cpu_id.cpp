/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "board.h"
#include <stdio.h>

#if defined(ESP_PLATFORM)
#include "esp_mac.h"
#endif

void getCPUUniqueID(char* s)
{
#if defined(ESP_PLATFORM)
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  sprintf(s, "%02X%02X%02X-%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#else
  sprintf(s, "ESP32S3-SIM");
#endif
}
