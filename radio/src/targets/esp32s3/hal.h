/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 *
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#pragma once

#include "hal_settings.h"
#include "esp32_gpio.h"

// =============================================================================
// Reference DIY radio pin map for ESP32-S3 (valid GPIOs: 0-21, 26-48)
// Remap for your PCB. Octal-PSRAM modules (N16R8) must avoid GPIO 33-37.
// =============================================================================

#define STORAGE_USE_SDCARD_SPI 1

#define ADC_VREF_PREC2 330

// Power latch / switch
#define PWR_ON_GPIO                 GPIO_PIN(GPIO, 46)
#define PWR_SWITCH_GPIO             GPIO_PIN(GPIO, 0)

// Internal CRSF / ELRS module (UART1)
#define INTMODULE_USART_PORT        1
#define INTMODULE_TX_GPIO           GPIO_PIN(GPIO, 43)
#define INTMODULE_RX_GPIO           GPIO_PIN(GPIO, 44)
#define INTMODULE_PWR_GPIO          GPIO_PIN(GPIO, 3)
#define INTMODULE_FIFO_SIZE         512

// External module bay (UART2)
#define EXTMODULE_USART_PORT        2
#define EXTMODULE_TX_GPIO           GPIO_PIN(GPIO, 17)
#define EXTMODULE_RX_GPIO           GPIO_PIN(GPIO, 18)
#define EXTMODULE_PWR_GPIO          GPIO_PIN(GPIO, 8)
#define EXTMODULE_FIFO_SIZE         512

// SPI LCD (ILI9488 / ST7796 class) on SPI2
#define LCD_SPI_HOST                2
#define LCD_PIN_MOSI                GPIO_PIN(GPIO, 11)
#define LCD_PIN_SCLK                GPIO_PIN(GPIO, 12)
#define LCD_PIN_CS                  GPIO_PIN(GPIO, 10)
#define LCD_PIN_DC                  GPIO_PIN(GPIO, 13)
#define LCD_PIN_RST                 GPIO_PIN(GPIO, 9)
#define LCD_PIN_BL                  GPIO_PIN(GPIO, 47)

// SPI SD card on SPI3
#define SD_SPI_HOST                 3
#define SD_PIN_MOSI                 GPIO_PIN(GPIO, 38)
#define SD_PIN_MISO                 GPIO_PIN(GPIO, 39)
#define SD_PIN_SCLK                 GPIO_PIN(GPIO, 40)
#define SD_PIN_CS                   GPIO_PIN(GPIO, 41)

// I2S audio
#define AUDIO_I2S_BCK               GPIO_PIN(GPIO, 26)
#define AUDIO_I2S_WS                GPIO_PIN(GPIO, 27)
#define AUDIO_I2S_DOUT              GPIO_PIN(GPIO, 28)

// Haptic (LEDC PWM)
#define HAPTIC_GPIO                 GPIO_PIN(GPIO, 48)

// Rotary encoder
#define ROTARY_ENCODER_GPIO_A       GPIO_PIN(GPIO, 19)
#define ROTARY_ENCODER_GPIO_B       GPIO_PIN(GPIO, 20)

// Touch panel I2C (optional)
#define TOUCH_I2C_PORT              0
#define TOUCH_I2C_SDA               GPIO_PIN(GPIO, 15)
#define TOUCH_I2C_SCL               GPIO_PIN(GPIO, 16)

#define MIXER_SCHEDULER_TIMER_FREQ  1000000u

// ESP32 platform enhancements
#define ESP32_DUAL_CORE             1
#define ESP32_PSRAM_FRAMEBUFFER     1
#define ESP32_WIFI_TELEMETRY        1
#define ESP32_BLE_TRAINER           1

#define SPORT_MAX_BAUDRATE          400000
