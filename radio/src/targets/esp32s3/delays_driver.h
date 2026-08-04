/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 *
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void delaysInit();
void delay_01us(uint32_t count);
void delay_us(uint32_t count);
void delay_ms(uint32_t count);
uint32_t ticksNow();

#ifdef __cplusplus
}
#endif
