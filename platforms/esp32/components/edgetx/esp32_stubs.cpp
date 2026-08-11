/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * Link stubs for optional modules not yet built into the ESP-IDF firmware
 * (Lua runtime, gyro/IMU driver, multi-language TTS packs, protocol helpers).
 * Colorlcd / LVGL / MainWindow are linked from real sources.
 */

#include "heartbeat_driver.h"
#include "hal/module_driver.h"
#include "edgetx_types.h"
#include "dataconstants.h"
#include "edgetx_constants.h"
#include "pulses/modules_constants.h"
#include "translations/tts/tts.h"

#include <cstdint>

struct gtm;

volatile HeartbeatCapture heartbeatCapture = {};

int16_t gyroScaledX() { return 0; }
int16_t gyroScaledY() { return 0; }
void gyroWakeup() {}

void rtcdriver_settime(struct gtm*) {}
void rtcSetTime(const struct gtm*) {}

extern "C" void luaInitMainState() {}
void luaInit() {}
void luaClose() {}
void luaTask() {}

void frskyDSetDefault(int, uint16_t) {}
void frskySportSetDefault(int, uint16_t, uint8_t, uint8_t) {}
void spektrumSetDefault(int, uint16_t, uint8_t, uint8_t) {}

void trainer_stop_dsc() {}
void trainer_stop_module_cppm() {}
void sbusAuxSetEnabled(bool) {}
void sbusSetReceiveCtx(void*, const etx_serial_driver_t*) {}
void sbusAuxFrameReceived(void*) {}

// English TTS pack is linked from tts_en.cpp; only EN is available on ESP32 yet.
const LanguagePack* const languagePacks[] = {&enLanguagePack, nullptr};
