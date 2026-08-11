/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "hal/serial_driver.h"
#include "esp32_gpio.h"
#include "board.h"

#include <cstring>

#if defined(ESP_PLATFORM)
#include "esp32_idf_gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#endif

struct Esp32UartCtx {
  int port;
  uint32_t baud;
  gpio_t tx;
  gpio_t rx;
  bool half_duplex;
};

static Esp32UartCtx s_int = { INTMODULE_USART_PORT, 0, INTMODULE_TX_GPIO, INTMODULE_RX_GPIO, false };
static Esp32UartCtx s_ext = { EXTMODULE_USART_PORT, 0, EXTMODULE_TX_GPIO, EXTMODULE_RX_GPIO, false };

static void* uart_init(void* hw_def, const etx_serial_init* params)
{
  auto* ctx = (Esp32UartCtx*)hw_def;
  if (!ctx || !params) return nullptr;
#if defined(ESP_PLATFORM)
  uart_port_t port = (uart_port_t)ctx->port;
  uart_config_t cfg = {
    .baud_rate = (int)params->baudrate,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
  };
  if (params->encoding == ETX_Encoding_8E2) {
    cfg.parity = UART_PARITY_EVEN;
    cfg.stop_bits = UART_STOP_BITS_2;
  }
  uart_driver_install(port, 1024, 1024, 0, nullptr, 0);
  uart_param_config(port, &cfg);
  uart_set_pin(port, esp32_gpio_num(ctx->tx), esp32_gpio_num(ctx->rx),
               UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  if (params->polarity == ETX_Pol_Inverted) {
    uart_set_line_inverse(port, UART_SIGNAL_TXD_INV | UART_SIGNAL_RXD_INV);
  }
  ctx->baud = params->baudrate;
  ctx->half_duplex = (params->direction != ETX_Dir_TX_RX);
#else
  ctx->baud = params->baudrate;
#endif
  return ctx;
}

static void uart_deinit(void* c)
{
#if defined(ESP_PLATFORM)
  auto* ctx = (Esp32UartCtx*)c;
  if (ctx) uart_driver_delete((uart_port_t)ctx->port);
#else
  (void)c;
#endif
}

static void uart_send_byte(void* c, uint8_t byte)
{
#if defined(ESP_PLATFORM)
  auto* ctx = (Esp32UartCtx*)c;
  uart_write_bytes((uart_port_t)ctx->port, (const char*)&byte, 1);
#else
  (void)c; (void)byte;
#endif
}

static void uart_send_buffer(void* c, const uint8_t* data, uint32_t size)
{
#if defined(ESP_PLATFORM)
  auto* ctx = (Esp32UartCtx*)c;
  uart_write_bytes((uart_port_t)ctx->port, (const char*)data, size);
#else
  (void)c; (void)data; (void)size;
#endif
}

static bool uart_tx_completed(void* c)
{
#if defined(ESP_PLATFORM)
  auto* ctx = (Esp32UartCtx*)c;
  return uart_wait_tx_done((uart_port_t)ctx->port, 0) == ESP_OK;
#else
  (void)c; return true;
#endif
}

static void uart_wait_tx(void* c)
{
#if defined(ESP_PLATFORM)
  auto* ctx = (Esp32UartCtx*)c;
  uart_wait_tx_done((uart_port_t)ctx->port, pdMS_TO_TICKS(100));
#else
  (void)c;
#endif
}

static void uart_enable_rx(void* c)
{
  (void)c; // full-duplex UART; half-duplex boards can gate TX here
}

static int uart_get_byte(void* c, uint8_t* data)
{
#if defined(ESP_PLATFORM)
  auto* ctx = (Esp32UartCtx*)c;
  return uart_read_bytes((uart_port_t)ctx->port, data, 1, 0) == 1 ? 1 : 0;
#else
  (void)c; (void)data; return 0;
#endif
}

static int uart_get_last_byte(void* c, uint32_t idx, uint8_t* data)
{
  (void)c; (void)idx; (void)data; return 0;
}

static int uart_buffered(void* c)
{
#if defined(ESP_PLATFORM)
  auto* ctx = (Esp32UartCtx*)c;
  size_t n = 0;
  uart_get_buffered_data_len((uart_port_t)ctx->port, &n);
  return (int)n;
#else
  (void)c; return 0;
#endif
}

static int uart_copy_rx(void* c, uint8_t* buf, uint32_t len)
{
#if defined(ESP_PLATFORM)
  auto* ctx = (Esp32UartCtx*)c;
  return uart_read_bytes((uart_port_t)ctx->port, buf, len, 0);
#else
  (void)c; (void)buf; (void)len; return 0;
#endif
}

static void uart_clear_rx(void* c)
{
#if defined(ESP_PLATFORM)
  auto* ctx = (Esp32UartCtx*)c;
  uart_flush_input((uart_port_t)ctx->port);
#else
  (void)c;
#endif
}

static uint32_t uart_get_baud(void* c)
{
  return c ? ((Esp32UartCtx*)c)->baud : 0;
}

static void uart_set_baud(void* c, uint32_t baud)
{
  auto* ctx = (Esp32UartCtx*)c;
  if (!ctx) return;
  ctx->baud = baud;
#if defined(ESP_PLATFORM)
  uart_set_baudrate((uart_port_t)ctx->port, baud);
#endif
}

static void uart_set_polarity(void* c, uint8_t polarity)
{
#if defined(ESP_PLATFORM)
  auto* ctx = (Esp32UartCtx*)c;
  uart_set_line_inverse((uart_port_t)ctx->port,
    polarity == ETX_Pol_Inverted ? (UART_SIGNAL_TXD_INV | UART_SIGNAL_RXD_INV) : 0);
#else
  (void)c; (void)polarity;
#endif
}

const etx_serial_driver_t Esp32SerialDriver = {
  .init = uart_init,
  .deinit = uart_deinit,
  .sendByte = uart_send_byte,
  .sendBuffer = uart_send_buffer,
  .txCompleted = uart_tx_completed,
  .waitForTxCompleted = uart_wait_tx,
  .enableRx = uart_enable_rx,
  .getByte = uart_get_byte,
  .getLastByte = uart_get_last_byte,
  .getBufferedBytes = uart_buffered,
  .copyRxBuffer = uart_copy_rx,
  .clearRxBuffer = uart_clear_rx,
  .getBaudrate = uart_get_baud,
  .setBaudrate = uart_set_baud,
  .setPolarity = uart_set_polarity,
  .setHWOption = nullptr,
  .setReceiveCb = nullptr,
  .setIdleCb = nullptr,
  .setBaudrateCb = nullptr,
};

void* esp32InternalModuleHwDef() { return &s_int; }
void* esp32ExternalModuleHwDef() { return &s_ext; }
extern const etx_serial_driver_t Esp32SerialDriver;
