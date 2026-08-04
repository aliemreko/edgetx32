/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 *
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#pragma once

#include "hal/gpio.h"
#include <stdint.h>

#define GPIO_UNDEF (0xffffffffu)

// ESP32: gpio_t stores the GPIO number directly.
#define GPIO_PIN(_port, n) ((gpio_t)(n))

#define _GPIO_MODE(io, pr, ot) ((io << 0) | (pr << 2) | (ot << 4))

enum {
  GPIO_IN    = _GPIO_MODE(0, 0, 0),
  GPIO_IN_PD = _GPIO_MODE(0, 2, 0),
  GPIO_IN_PU = _GPIO_MODE(0, 1, 0),
  GPIO_OUT   = _GPIO_MODE(1, 0, 0),
  GPIO_OD    = _GPIO_MODE(1, 0, 1),
  GPIO_OD_PU = _GPIO_MODE(1, 1, 1)
};

enum {
  GPIO_AF0 = 0,
  GPIO_AF_UNDEF = 0xff
};

enum {
  GPIO_PIN_SPEED_LOW = 0,
  GPIO_PIN_SPEED_MEDIUM = 1,
  GPIO_PIN_SPEED_HIGH = 2,
  GPIO_PIN_SPEED_VERY_HIGH = 3,
};

static inline int esp32_gpio_num(gpio_t pin) { return (int)(pin & 0xff); }
