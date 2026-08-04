/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "hal/usb_driver.h"

static bool s_started = false;
static int s_mode = USB_UNSELECTED_MODE;

int usbPlugged() { return 0; }
void usbInit() {}
void usbStart() { s_started = true; }
void usbStop() { s_started = false; }
bool usbStarted() { return s_started; }
uint32_t usbSerialFreeSpace() { return 0; }
void usbJoystickRestart() {}
void usbJoystickUpdate() {}
int usbRegisterDFUMedia(const void*) { return 0; }

extern "C" int getSelectedUsbMode() { return s_mode; }
void setSelectedUsbMode(int mode) { s_mode = mode; }

const etx_serial_port_t UsbSerialPort = {
  .name = "USB",
  .uart = nullptr,
  .hw_def = nullptr,
  .set_pwr = nullptr,
};
