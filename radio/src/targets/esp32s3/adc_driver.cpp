/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "hal/adc_driver.h"
#include "board.h"
#include "esp32_gpio.h"

#include "hal_adc_inputs.inc"

#if __has_include("esp32_adc_inputs.inc")
#include "esp32_adc_inputs.inc"
#else
// Fallback when generator has not run yet
static const int _esp32_adc_gpios[] = {1, 2, 4, 5, 6, 7, 8, -1};
static const bool _esp32_adc_inverted[] = {false, true, false, true, false, false, false, false};
constexpr unsigned _esp32_adc_count = 8;
#endif

#if defined(ESP_PLATFORM)
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

static adc_oneshot_unit_handle_t s_adc = nullptr;
static adc_channel_t s_channels[16];
static adc_unit_t s_units[16];
static bool s_valid[16];
#endif

static bool esp32_adc_init()
{
#if defined(ESP_PLATFORM)
  adc_oneshot_unit_init_cfg_t unit_cfg = { .unit_id = ADC_UNIT_1 };
  if (adc_oneshot_new_unit(&unit_cfg, &s_adc) != ESP_OK) return false;

  adc_oneshot_chan_cfg_t chan_cfg = {
    .atten = ADC_ATTEN_DB_12,
    .bitwidth = ADC_BITWIDTH_12,
  };

  for (unsigned i = 0; i < _esp32_adc_count && i < 16; i++) {
    s_valid[i] = false;
    if (_esp32_adc_gpios[i] < 0) continue;
    if (adc_oneshot_io_to_channel(_esp32_adc_gpios[i], &s_units[i], &s_channels[i]) != ESP_OK)
      continue;
    if (s_units[i] != ADC_UNIT_1) continue;
    if (adc_oneshot_config_channel(s_adc, s_channels[i], &chan_cfg) != ESP_OK) continue;
    s_valid[i] = true;
  }
  return true;
#else
  return true;
#endif
}

static bool esp32_adc_start()
{
  int max_input = adcGetMaxInputs(ADC_INPUT_ALL);
  for (int i = 0; i < max_input; i++) {
    uint16_t raw = 2048;
#if defined(ESP_PLATFORM)
    if (i < (int)_esp32_adc_count && s_valid[i]) {
      int v = 0;
      if (adc_oneshot_read(s_adc, s_channels[i], &v) == ESP_OK) raw = (uint16_t)v;
    }
#endif
    if (i < (int)_esp32_adc_count && _esp32_adc_inverted[i])
      raw = ADC_INVERT_VALUE(raw);
    setAnalogValue(i, raw);
  }
  return true;
}

extern const etx_hal_adc_driver_t esp32_adc_driver;

const etx_hal_adc_driver_t esp32_adc_driver = {
  .inputs = _hal_inputs,
  .default_pots_cfg = _pot_default_config,
  .init = esp32_adc_init,
  .deinit = nullptr,
  .start_conversion = esp32_adc_start,
  .wait_completion = nullptr,
  .set_input_mask = nullptr,
  .get_input_mask = nullptr,
};

void enableVBatBridge() {}
void disableVBatBridge() {}
bool isVBatBridgeEnabled() { return false; }

uint16_t getBatteryVoltage()
{
  if (adcGetMaxInputs(ADC_INPUT_VBAT) < 1) return 0;
  // Scale 12-bit reading through divider assumption (~3.3V full-scale -> pack volts * 10)
  uint16_t raw = getAnalogValue(adcGetInputOffset(ADC_INPUT_VBAT));
  return (uint16_t)((uint32_t)raw * BATTERY_MAX / ADC_MAX_VALUE);
}

uint16_t getRTCBatteryVoltage() { return 300; }
