/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "board.h"
#include "hal/serial_port.h"

const etx_serial_port_t* auxSerialGetPort(int port_nr)
{
  (void)port_nr;
  return nullptr; // optional GPS / aux — wire UART0/UART when needed
}
