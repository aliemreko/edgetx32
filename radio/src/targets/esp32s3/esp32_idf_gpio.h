/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * EdgeTX defines gpio_mode_t as uint8_t; ESP-IDF uses an enum of the same name.
 * Include this header (instead of driver/gpio.h directly) after EdgeTX gpio
 * headers so both APIs can coexist in one translation unit.
 */

#pragma once

#if defined(ESP_PLATFORM)
#define gpio_mode_t esp_idf_gpio_mode_t
#include "driver/gpio.h"
#undef gpio_mode_t
#endif
