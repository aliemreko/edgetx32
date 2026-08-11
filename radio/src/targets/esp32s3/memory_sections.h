/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#pragma once

/* ESP32 has no STM32-style DMA/CCM sections; keep attributes as no-ops. */
#define __CCMRAM
#define __DMA
#define __DMA_NO_CACHE
#define __FLASH
#define __IRAM
#define __SDRAM
