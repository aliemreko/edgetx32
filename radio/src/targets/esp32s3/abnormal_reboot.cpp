/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "hal/abnormal_reboot.h"

#if defined(ESP_PLATFORM)
#include "esp_system.h"
#include "esp_attr.h"
static RTC_NOINIT_ATTR uint32_t s_reboot_cmd;
#endif

static uint32_t s_cause = ARC_None;

void abnormalRebootEnableDetection()
{
#if defined(ESP_PLATFORM)
  esp_reset_reason_t r = esp_reset_reason();
  if (r == ESP_RST_PANIC || r == ESP_RST_INT_WDT || r == ESP_RST_TASK_WDT || r == ESP_RST_WDT)
    s_cause = ARC_Watchdog;
  else if (r == ESP_RST_SW)
    s_cause = ARC_Software;
  else
    s_cause = ARC_None;
#endif
}

void abnormalRebootDisableDetection()
{
  s_cause = ARC_None;
}

uint32_t abnormalRebootGetCause() { return s_cause; }

uint32_t abnormalRebootGetCmd()
{
#if defined(ESP_PLATFORM)
  return s_reboot_cmd;
#else
  return 0;
#endif
}

void abnormalRebootResetCmd()
{
#if defined(ESP_PLATFORM)
  s_reboot_cmd = 0;
#endif
}
