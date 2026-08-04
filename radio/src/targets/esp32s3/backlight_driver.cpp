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

static bool s_on = false;
static uint8_t s_level = 100;

void backlightInit()
{
  gpio_init(LCD_PIN_BL, GPIO_OUT, GPIO_PIN_SPEED_LOW);
#if defined(ESP_PLATFORM)
  ledc_timer_config_t timer = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_8_BIT,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 5000,
    .clk_cfg = LEDC_AUTO_CLK,
  };
  ledc_timer_config(&timer);
  ledc_channel_config_t ch = {
    .gpio_num = esp32_gpio_num(LCD_PIN_BL),
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = LEDC_TIMER_0,
    .duty = 255,
    .hpoint = 0,
  };
  ledc_channel_config(&ch);
#endif
  backlightFullOn();
}

void backlightEnable(uint8_t level)
{
  s_level = level;
  s_on = level > 0;
#if defined(ESP_PLATFORM)
  // EdgeTX bright is inverted on many radios; map 0..100-ish to duty
  uint32_t duty = (100 - (level > 100 ? 100 : level)) * 255 / 100;
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
#else
  if (s_on) gpio_set(LCD_PIN_BL); else gpio_clear(LCD_PIN_BL);
#endif
}

void backlightFullOn()
{
  backlightEnable(0);
}

bool isBacklightEnabled() { return s_on; }
