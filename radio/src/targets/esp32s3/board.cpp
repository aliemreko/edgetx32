/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "board.h"

#include "hal/adc_driver.h"
#include "hal/abnormal_reboot.h"
#include "hal/key_driver.h"
#include "hal/switch_driver.h"
#include "hal/rotary_encoder.h"
#include "hal/storage.h"
#include "hal/module_port.h"
#include "hal/gpio.h"
#include "hal/usb_driver.h"

#include "esp32_features.h"
#include "esp32_gpio.h"

#include "delays_driver.h"

extern const etx_hal_adc_driver_t esp32_adc_driver;
extern void esp32ModulePortsInit();
extern void touchPanelInit();
extern void timersInit();

void INTERNAL_MODULE_ON()  { gpio_set(INTMODULE_PWR_GPIO); }
void INTERNAL_MODULE_OFF() { gpio_clear(INTMODULE_PWR_GPIO); }
void EXTERNAL_MODULE_ON()  { gpio_set(EXTMODULE_PWR_GPIO); }
void EXTERNAL_MODULE_OFF() { gpio_clear(EXTMODULE_PWR_GPIO); }

void boardInit()
{
  delaysInit();
  timersInit();
  pwrInit();
  abnormalRebootEnableDetection();

  ledInit();
  keysInit();
  switchInit();
  rotaryEncoderInit();

  adcInit(&esp32_adc_driver);
  storageInit();
  esp32ModulePortsInit();
  modulePortInit();

  backlightInit();
  lcdInit();
  hapticInit();
  audioInit();
  touchPanelInit();
  usbInit();

#if defined(ESP32_WIFI_TELEMETRY)
  esp32WifiTelemetryInit();
#endif
#if defined(ESP32_BLE_TRAINER)
  esp32BleTrainerInit();
#endif

  // UI / networking on core 0; mixer task should call esp32PinMixerToCore1().
  esp32PinUiToCore0();
}
