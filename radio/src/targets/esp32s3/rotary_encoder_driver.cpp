/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "hal/rotary_encoder.h"
#include "board.h"
#include "esp32_gpio.h"

static rotenc_t s_raw = 0;
static int8_t s_accel = 0;

void rotaryEncoderInit()
{
  gpio_init(ROTARY_ENCODER_GPIO_A, GPIO_IN_PU, GPIO_PIN_SPEED_LOW);
  gpio_init(ROTARY_ENCODER_GPIO_B, GPIO_IN_PU, GPIO_PIN_SPEED_LOW);
}

rotenc_t rotaryEncoderGetValue()
{
  return s_raw / ROTARY_ENCODER_GRANULARITY;
}

int8_t rotaryEncoderGetAccel() { return s_accel; }
void rotaryEncoderResetAccel() { s_accel = 0; }
