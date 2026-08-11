/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * Link stubs for optional modules not yet built into the ESP-IDF firmware
 * (trainer jack ports, firmware flashers, FatFs RPATH, multi-language TTS).
 * Colorlcd / LVGL / MainWindow / gyro are linked from real sources.
 */

#include "edgetx.h"
#include "heartbeat_driver.h"
#include "io/bootloader_flash.h"
#include "io/frsky_firmware_update.h"
#include "io/multi_firmware_update.h"
#include "translations/tts/tts.h"
#include "FatFs/ff.h"

#include <cstdint>
#include <cstring>

struct gtm;

volatile HeartbeatCapture heartbeatCapture = {};

void rtcdriver_settime(struct gtm*) {}
void rtcSetTime(const struct gtm*) {}

void frskyDSetDefault(int, uint16_t) {}
void frskySportSetDefault(int, uint16_t, uint8_t, uint8_t) {}
void spektrumSetDefault(int, uint16_t, uint8_t, uint8_t) {}

// Trainer jack / module CPPM ports are not wired on the ESP32 board yet.
bool trainer_dsc_available() { return false; }
void trainer_init_dsc_out() {}
void trainer_init_dsc_in() {}
void trainer_stop_dsc() {}
void trainer_init_module_cppm() {}
void trainer_stop_module_cppm() {}

void sbusAuxSetEnabled(bool) {}
void sbusSetReceiveCtx(void*, const etx_serial_driver_t*) {}
void sbusAuxFrameReceived(void*) {}

// English TTS pack is linked from tts_en.cpp; only EN is available on ESP32 yet.
const LanguagePack* const languagePacks[] = {&enLanguagePack, nullptr};

// IDF FatFs is linked without relative-path support; provide no-op RPATH APIs.
extern "C" {
FRESULT f_chdir(const TCHAR* path)
{
  (void)path;
  return FR_OK;
}

FRESULT f_getcwd(TCHAR* buff, UINT len)
{
  if (buff && len) {
    buff[0] = '/';
    if (len > 1) buff[1] = '\0';
  }
  return FR_OK;
}
}

// Firmware update / bootloader flash paths are STM32-oriented for now.
bool isBootloader(const char*) { return false; }

void BootloaderFirmwareUpdate::flashFirmware(const char*, ProgressHandler) {}

const char* readFrSkyFirmwareInformation(const char*, FrSkyFirmwareInformation&)
{
  return "Not supported";
}

const char* FrskyDeviceFirmwareUpdate::flashFirmware(const char*, ProgressHandler)
{
  return "Not supported";
}

bool MultiDeviceFirmwareUpdate::flashFirmware(const char*, ProgressHandler)
{
  return false;
}
