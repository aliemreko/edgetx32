/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "board.h"
#include "touch.h"
#include "esp32_gpio.h"

#if defined(ESP_PLATFORM)
#include "driver/i2c_master.h"
#endif

static TouchState s_touchState = {};
TouchState touchState = {};

bool touchPanelEventOccured() { return false; }

TouchState touchPanelRead() { return s_touchState; }

TouchState getInternalTouchState() { return s_touchState; }

void touchPanelInit()
{
#if defined(ESP_PLATFORM)
  // Optional GT911 / CST816 bring-up on TOUCH_I2C_* pins.
#endif
}
