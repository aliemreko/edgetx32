/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#pragma once

#include "esp_attr.h"

/* ESP32 has no STM32-style DMA/CCM sections; keep attributes as no-ops,
 * except place large "SDRAM" buffers into external PSRAM. */
#define __CCMRAM
#define __DMA
#define __DMA_NO_CACHE
#define __FLASH
#define __IRAM
#define __SDRAM EXT_RAM_BSS_ATTR
#define __ALIGNED(x) __attribute__((aligned(x)))
