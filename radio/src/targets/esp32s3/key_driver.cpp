/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "hal/key_driver.h"
#include "hal/gpio.h"
#include "esp32_gpio.h"
#include "board.h"
#include "dataconstants.h"

#include "hal_keys.inc"

#if __has_include("esp32_keys.inc")
#include "esp32_keys.inc"
#else
static inline void _init_keys() {}
static inline uint32_t _read_keys() { return 0; }
static inline void _init_trims() {}
static inline uint32_t _read_trims() { return 0; }
#endif

void keysInit()
{
  _init_keys();
  _init_trims();
}

uint32_t readKeys()
{
  return _read_keys();
}

uint32_t readTrims()
{
  return _read_trims();
}
