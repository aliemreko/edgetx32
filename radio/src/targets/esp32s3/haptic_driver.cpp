/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "board.h"
#include "esp32_gpio.h"

#if defined(ESP_PLATFORM)
#include "driver/ledc.h"
#endif

void hapticInit()
{
  gpio_init(HAPTIC_GPIO, GPIO_OUT, GPIO_PIN_SPEED_LOW);
#if defined(ESP_PLATFORM)
  ledc_timer_config_t timer = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_1,
    .freq_hz = 200,
    .clk_cfg = LEDC_AUTO_CLK,
  };
  ledc_timer_config(&timer);
  ledc_channel_config_t ch = {
    .gpio_num = esp32_gpio_num(HAPTIC_GPIO),
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_1,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = LEDC_TIMER_1,
    .duty = 0,
    .hpoint = 0,
  };
  ledc_channel_config(&ch);
#endif
}

void hapticDone() { hapticOff(); }

void hapticOff()
{
#if defined(ESP_PLATFORM)
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
#else
  gpio_clear(HAPTIC_GPIO);
#endif
}

void hapticOn(uint32_t pwmPercent)
{
  if (pwmPercent > 100) pwmPercent = 100;
#if defined(ESP_PLATFORM)
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, pwmPercent * 255 / 100);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
#else
  if (pwmPercent) gpio_set(HAPTIC_GPIO); else gpio_clear(HAPTIC_GPIO);
#endif
}
