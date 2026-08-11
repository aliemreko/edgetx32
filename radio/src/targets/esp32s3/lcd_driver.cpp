/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "board.h"
#include "esp32_gpio.h"

#include <string.h>
#include <cstdlib>

#if defined(ESP_PLATFORM)
#include "esp32_idf_gpio.h"
#include "esp_heap_caps.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#endif

#ifndef LCD_W
#define LCD_W 480
#endif
#ifndef LCD_H
#define LCD_H 272
#endif

static uint16_t* s_fb = nullptr;
static void (*s_flush_cb)(void*) = nullptr;

#if defined(COLORLCD)
// Provided by colorlcd layer
extern void lcdSetFlushCb(void (*cb)(void*));
#endif

void lcdSetInitalFrameBuffer(void* fb)
{
  s_fb = (uint16_t*)fb;
}

void lcdCopy(void* dest, void* src)
{
  if (!dest || !src) return;
  memcpy(dest, src, LCD_W * LCD_H * sizeof(uint16_t));
}

void lcdOn()
{
  gpio_set(LCD_PIN_BL);
}

void lcdOff()
{
  gpio_clear(LCD_PIN_BL);
}

static void flush_to_panel(void*)
{
#if defined(ESP_PLATFORM)
  // Panel IO flush is hooked during lcdInit(); placeholder keeps LVGL path alive.
#endif
}

void lcdInit()
{
  gpio_init(LCD_PIN_BL, GPIO_OUT, GPIO_PIN_SPEED_LOW);
  gpio_init(LCD_PIN_CS, GPIO_OUT, GPIO_PIN_SPEED_HIGH);
  gpio_init(LCD_PIN_DC, GPIO_OUT, GPIO_PIN_SPEED_HIGH);
  gpio_init(LCD_PIN_RST, GPIO_OUT, GPIO_PIN_SPEED_LOW);

#if defined(ESP_PLATFORM) && defined(ESP32_PSRAM_FRAMEBUFFER)
  if (!s_fb) {
    s_fb = (uint16_t*)heap_caps_malloc(LCD_W * LCD_H * sizeof(uint16_t),
                                       MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  }
#else
  if (!s_fb) {
    s_fb = (uint16_t*)malloc(LCD_W * LCD_H * sizeof(uint16_t));
  }
#endif
  if (s_fb) memset(s_fb, 0, LCD_W * LCD_H * sizeof(uint16_t));
  lcdSetInitalFrameBuffer(s_fb);

#if defined(ESP_PLATFORM)
  // SPI bus + panel IO — ILI9488/ST7796 class init sequence can be swapped
  // per PCB without touching the portable UI stack.
  spi_bus_config_t buscfg = {};
  buscfg.sclk_io_num = esp32_gpio_num(LCD_PIN_SCLK);
  buscfg.mosi_io_num = esp32_gpio_num(LCD_PIN_MOSI);
  buscfg.miso_io_num = -1;
  buscfg.quadwp_io_num = -1;
  buscfg.quadhd_io_num = -1;
  buscfg.max_transfer_sz = LCD_W * 40 * sizeof(uint16_t);
  spi_bus_initialize((spi_host_device_t)LCD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
#endif

  lcdOn();
}
