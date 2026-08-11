/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "hal/switch_driver.h"
#include "hal/gpio.h"
#include "esp32_gpio.h"
#include "definitions.h"

#include <string.h>

struct hw_switch_def {
  const char*   name;
  SwitchHwType  type;
  SwitchConfig  defaultType;
};

#include "simu_switches.inc"

#if __has_include("esp32_switches.inc")
#include "esp32_switches.inc"
#else
static inline void _init_switches() {}
#endif

static uint8_t s_forced[MAX_SWITCHES]; // 0=hw, 1=up, 2=mid, 3=down for host test

void boardInitSwitches()
{
  memset(s_forced, 0, sizeof(s_forced));
  _init_switches();
}

static SwitchHwPos read_gpio_switch(uint8_t idx)
{
  // Decode from JSON pin map when available (SA..SF on reference DIY board)
  // Active-low inputs: both high => UP, high-only => MID, low-only => DOWN for 3POS
  (void)idx;
  return SWITCH_HW_UP;
}

SwitchHwPos boardSwitchGetPosition(uint8_t idx)
{
  if (idx >= n_switches) return SWITCH_HW_UP;
  if (s_forced[idx]) {
    return (SwitchHwPos)(s_forced[idx] - 1);
  }
  return read_gpio_switch(idx);
}

const char* boardSwitchGetName(uint8_t idx)
{
  return _switch_defs[idx].name;
}

SwitchHwType boardSwitchGetType(uint8_t idx)
{
  return _switch_defs[idx].type;
}

uint8_t boardGetMaxSwitches() { return n_switches; }

SwitchConfig boardSwitchGetDefaultConfig(uint8_t idx)
{
  return _switch_defs[idx].defaultType;
}
