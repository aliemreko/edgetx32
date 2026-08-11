/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "esp32_gpio.h"

#if defined(ESP_PLATFORM)
#include "driver/gpio.h"
#include "esp_attr.h"
using etx_pin_mode_t = etx_gpio_mode_t;
#else
// Host / unit-test stubs
#include <map>
static std::map<int, int> g_levels;
static std::map<int, int> g_modes;
using etx_pin_mode_t = gpio_mode_t;
#endif

void gpio_init(gpio_t pin, etx_pin_mode_t mode, gpio_speed_t speed)
{
  (void)speed;
  if (pin == GPIO_UNDEF) return;
  int n = esp32_gpio_num(pin);
#if defined(ESP_PLATFORM)
  gpio_config_t cfg = {};
  cfg.pin_bit_mask = 1ULL << n;
  cfg.intr_type = GPIO_INTR_DISABLE;
  switch (mode) {
    case GPIO_IN_PU:
      cfg.mode = GPIO_MODE_INPUT;
      cfg.pull_up_en = GPIO_PULLUP_ENABLE;
      cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
      break;
    case GPIO_IN_PD:
      cfg.mode = GPIO_MODE_INPUT;
      cfg.pull_up_en = GPIO_PULLUP_DISABLE;
      cfg.pull_down_en = GPIO_PULLDOWN_ENABLE;
      break;
    case GPIO_OD:
    case GPIO_OD_PU:
      cfg.mode = GPIO_MODE_OUTPUT_OD;
      cfg.pull_up_en = (mode == GPIO_OD_PU) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
      cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
      break;
    case GPIO_OUT:
      cfg.mode = GPIO_MODE_OUTPUT;
      cfg.pull_up_en = GPIO_PULLUP_DISABLE;
      cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
      break;
    default:
      cfg.mode = GPIO_MODE_INPUT;
      cfg.pull_up_en = GPIO_PULLUP_DISABLE;
      cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
      break;
  }
  gpio_config(&cfg);
#else
  g_modes[n] = mode;
#endif
}

void gpio_init_af(gpio_t pin, gpio_af_t af, gpio_speed_t speed)
{
  (void)pin; (void)af; (void)speed;
  // Peripheral drivers configure AF via ESP-IDF drivers directly.
}

void gpio_init_int(gpio_t pin, etx_pin_mode_t mode, gpio_flank_t flank, gpio_cb_t cb)
{
#if defined(ESP_PLATFORM)
  gpio_init(pin, mode, GPIO_PIN_SPEED_LOW);
  int n = esp32_gpio_num(pin);
  gpio_set_intr_type((gpio_num_t)n,
                     flank == GPIO_RISING ? GPIO_INTR_POSEDGE :
                     flank == GPIO_FALLING ? GPIO_INTR_NEGEDGE : GPIO_INTR_ANYEDGE);
  gpio_isr_handler_add((gpio_num_t)n, (gpio_isr_t)cb, nullptr);
  gpio_intr_enable((gpio_num_t)n);
#else
  (void)pin; (void)mode; (void)flank; (void)cb;
#endif
}

void gpio_init_analog(gpio_t pin)
{
  (void)pin; // ADC driver owns the pin
}

void gpio_int_disable(gpio_t pin)
{
#if defined(ESP_PLATFORM)
  if (pin == GPIO_UNDEF) return;
  gpio_intr_disable((gpio_num_t)esp32_gpio_num(pin));
#else
  (void)pin;
#endif
}

void gpio_set_af(gpio_t pin, gpio_af_t af) { (void)pin; (void)af; }

etx_pin_mode_t gpio_get_mode(gpio_t pin)
{
#if defined(ESP_PLATFORM)
  (void)pin;
  return GPIO_IN;
#else
  return (etx_pin_mode_t)g_modes[esp32_gpio_num(pin)];
#endif
}

int gpio_read(gpio_t pin)
{
  if (pin == GPIO_UNDEF) return 0;
#if defined(ESP_PLATFORM)
  return gpio_get_level((gpio_num_t)esp32_gpio_num(pin));
#else
  return g_levels[esp32_gpio_num(pin)];
#endif
}

void gpio_set(gpio_t pin)
{
  if (pin == GPIO_UNDEF) return;
#if defined(ESP_PLATFORM)
  gpio_set_level((gpio_num_t)esp32_gpio_num(pin), 1);
#else
  g_levels[esp32_gpio_num(pin)] = 1;
#endif
}

void gpio_clear(gpio_t pin)
{
  if (pin == GPIO_UNDEF) return;
#if defined(ESP_PLATFORM)
  gpio_set_level((gpio_num_t)esp32_gpio_num(pin), 0);
#else
  g_levels[esp32_gpio_num(pin)] = 0;
#endif
}

void gpio_write(gpio_t pin, int value)
{
  if (value) gpio_set(pin); else gpio_clear(pin);
}

void gpio_toggle(gpio_t pin)
{
  gpio_write(pin, !gpio_read(pin));
}
