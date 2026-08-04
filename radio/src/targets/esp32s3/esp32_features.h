/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void esp32WifiTelemetryInit();
bool esp32WifiTelemetryReady();
void esp32WifiTelemetrySendJson(const char* json);

void esp32BleTrainerInit();
void esp32BleTrainerSendChannels(const int16_t* channels, uint8_t n);

// Dual-core helpers
void esp32PinMixerToCore1();
void esp32PinUiToCore0();

#ifdef __cplusplus
}
#endif
