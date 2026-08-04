/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "hal/audio_driver.h"
#include "board.h"
#include "esp32_gpio.h"

#if defined(ESP_PLATFORM)
#include "driver/i2s_std.h"
static i2s_chan_handle_t s_tx = nullptr;
#endif

bool audioHeadphoneDetect() { return false; }

void audioSetVolume(uint8_t volume)
{
  (void)volume;
}

void audioInit()
{
#if defined(ESP_PLATFORM)
  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
  i2s_new_channel(&chan_cfg, &s_tx, nullptr);

  i2s_std_config_t std_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(AUDIO_SAMPLE_RATE),
    .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
    .gpio_cfg = {
      .mclk = I2S_GPIO_UNUSED,
      .bclk = (gpio_num_t)esp32_gpio_num(AUDIO_I2S_BCK),
      .ws = (gpio_num_t)esp32_gpio_num(AUDIO_I2S_WS),
      .dout = (gpio_num_t)esp32_gpio_num(AUDIO_I2S_DOUT),
      .din = I2S_GPIO_UNUSED,
      .invert_flags = { false, false, false },
    },
  };
  i2s_channel_init_std_mode(s_tx, &std_cfg);
  i2s_channel_enable(s_tx);
#endif
}

// EdgeTX audio engine calls this to push PCM; hook into audioQueue consumer.
void audioConsumeCurrentBuffer()
{
  // Portable audioQueue fills a buffer; board driver drains to I2S.
  // Full wiring uses audioQueue.buffers — kept in portable audio.cpp.
}
