/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "hal/module_port.h"
#include "hal/serial_driver.h"
#include "board.h"
#include "esp32_gpio.h"
#include "dataconstants.h"

extern const etx_serial_driver_t Esp32SerialDriver;
extern void* esp32InternalModuleHwDef();
extern void* esp32ExternalModuleHwDef();

static void int_set_pwr(uint8_t on)
{
  if (on) gpio_set(INTMODULE_PWR_GPIO); else gpio_clear(INTMODULE_PWR_GPIO);
}

static void ext_set_pwr(uint8_t on)
{
  if (on) gpio_set(EXTMODULE_PWR_GPIO); else gpio_clear(EXTMODULE_PWR_GPIO);
}

static const etx_module_port_t _internal_ports[] = {
  {
    .port = ETX_MOD_PORT_UART,
    .type = ETX_MOD_TYPE_SERIAL,
    .dir_flags = ETX_MOD_DIR_TX_RX | ETX_MOD_FULL_DUPLEX,
    .drv = { .serial = &Esp32SerialDriver },
    .hw_def = nullptr, // filled at init
    .set_inverted = nullptr,
  },
};

static const etx_module_port_t _external_ports[] = {
  {
    .port = ETX_MOD_PORT_UART,
    .type = ETX_MOD_TYPE_SERIAL,
    .dir_flags = ETX_MOD_DIR_TX_RX | ETX_MOD_FULL_DUPLEX,
    .drv = { .serial = &Esp32SerialDriver },
    .hw_def = nullptr,
    .set_inverted = nullptr,
  },
};

static etx_module_port_t s_int_ports[1];
static etx_module_port_t s_ext_ports[1];

static const etx_module_t s_internal = {
  .ports = s_int_ports,
  .set_pwr = int_set_pwr,
  .set_bootcmd = nullptr,
  .n_ports = 1,
};

static const etx_module_t s_external = {
  .ports = s_ext_ports,
  .set_pwr = ext_set_pwr,
  .set_bootcmd = nullptr,
  .n_ports = 1,
};

BEGIN_MODULES()
  &s_internal,
  &s_external,
END_MODULES()

void esp32ModulePortsInit()
{
  s_int_ports[0] = _internal_ports[0];
  s_int_ports[0].hw_def = esp32InternalModuleHwDef();
  s_ext_ports[0] = _external_ports[0];
  s_ext_ports[0].hw_def = esp32ExternalModuleHwDef();

  gpio_init(INTMODULE_PWR_GPIO, GPIO_OUT, GPIO_PIN_SPEED_LOW);
  gpio_init(EXTMODULE_PWR_GPIO, GPIO_OUT, GPIO_PIN_SPEED_LOW);
  int_set_pwr(0);
  ext_set_pwr(0);
}
