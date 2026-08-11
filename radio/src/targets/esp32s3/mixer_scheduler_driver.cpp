/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "mixer_scheduler.h"

#if defined(ESP_PLATFORM)
#include "driver/gptimer.h"
#include "esp_attr.h"
#include "esp_log.h"

static gptimer_handle_t s_timer = nullptr;
static bool s_trigger_enabled = true;

static IRAM_ATTR bool on_timer(gptimer_handle_t, const gptimer_alarm_event_data_t*, void*)
{
  if (s_trigger_enabled) {
    mixerSchedulerDisableTrigger();
    mixerSchedulerISRTrigger();
  }
  return true; // yield if higher prio woken
}
#endif

void mixerSchedulerStart()
{
#if defined(ESP_PLATFORM)
  if (s_timer) return;

  gptimer_config_t cfg = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1000000, // 1 us
  };
  ESP_ERROR_CHECK(gptimer_new_timer(&cfg, &s_timer));

  gptimer_event_callbacks_t cbs = { .on_alarm = on_timer };
  ESP_ERROR_CHECK(gptimer_register_event_callbacks(s_timer, &cbs, nullptr));
  ESP_ERROR_CHECK(gptimer_enable(s_timer));

  gptimer_alarm_config_t alarm = {
    .alarm_count = getMixerSchedulerPeriod(),
    .reload_count = 0,
    .flags = { .auto_reload_on_alarm = true },
  };
  ESP_ERROR_CHECK(gptimer_set_alarm_action(s_timer, &alarm));
  ESP_ERROR_CHECK(gptimer_start(s_timer));
  s_trigger_enabled = true;
#endif
}

void mixerSchedulerStop()
{
#if defined(ESP_PLATFORM)
  if (!s_timer) return;
  gptimer_stop(s_timer);
  gptimer_disable(s_timer);
  gptimer_del_timer(s_timer);
  s_timer = nullptr;
#endif
}

void mixerSchedulerEnableTrigger()
{
#if defined(ESP_PLATFORM)
  s_trigger_enabled = true;
  if (s_timer) {
    gptimer_alarm_config_t alarm = {
      .alarm_count = getMixerSchedulerPeriod(),
      .reload_count = 0,
      .flags = { .auto_reload_on_alarm = true },
    };
    gptimer_set_alarm_action(s_timer, &alarm);
  }
#endif
}

void mixerSchedulerDisableTrigger()
{
#if defined(ESP_PLATFORM)
  s_trigger_enabled = false;
#endif
}

void mixerSchedulerSoftTrigger()
{
#if defined(ESP_PLATFORM)
  mixerSchedulerISRTrigger();
#endif
}
