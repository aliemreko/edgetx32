/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "board.h"
#include "esp32_gpio.h"

#if defined(ESP_PLATFORM)
#include "esp_system.h"
#include "esp_timer.h"
#endif

void pwrInit()
{
  gpio_init(PWR_ON_GPIO, GPIO_OUT, GPIO_PIN_SPEED_LOW);
  gpio_init(PWR_SWITCH_GPIO, GPIO_IN_PU, GPIO_PIN_SPEED_LOW);
  pwrOn();
}

void pwrOn() { gpio_set(PWR_ON_GPIO); }
void pwrOff() { gpio_clear(PWR_ON_GPIO); }

bool pwrPressed()
{
  return gpio_read(PWR_SWITCH_GPIO) == 0;
}

bool pwrOffPressed() { return pwrPressed(); }

// pwrPressedDuration() is implemented in edgetx.cpp when PWR_BUTTON_PRESS is set.

uint32_t pwrCheck()
{
  // e_power_on — keep running; full FSM uses EdgeTX power helpers.
  return 1;
}

void pwrResetHandler() {}

void boardOff()
{
  pwrOff();
#if defined(ESP_PLATFORM)
  esp_restart();
#endif
}
