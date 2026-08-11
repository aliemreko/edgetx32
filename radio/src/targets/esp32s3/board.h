/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 *
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#pragma once

#include "definitions.h"
#include "edgetx_constants.h"
#include "board_common.h"
#include "hal.h"
#include "hal/serial_port.h"
#include "hal/watchdog_driver.h"

#define FLASHSIZE                       (8 * 1024 * 1024)
#define BOOTLOADER_SIZE                 0x10000
#define FIRMWARE_MAX_LEN                FLASHSIZE

#define LUA_MEM_EXTRA_MAX               (2 * 1024 * 1024)
#define LUA_MEM_MAX                     (6 * 1024 * 1024)

extern uint16_t sessionTimer;

#define SLAVE_MODE() (g_model.trainerData.mode == TRAINER_MODE_SLAVE)

#define LUA_DEFAULT_BAUDRATE 115200

void boardInit();
void boardOff();

#define LEN_CPU_UID                     (3 * 8 + 2)
void getCPUUniqueID(char* s);

void INTERNAL_MODULE_ON();
void INTERNAL_MODULE_OFF();
void EXTERNAL_MODULE_ON();
void EXTERNAL_MODULE_OFF();
#define EXTERNAL_MODULE_PWR_OFF         EXTERNAL_MODULE_OFF
#define IS_INTERNAL_MODULE_ON()         (true)
#define IS_EXTERNAL_MODULE_ON()         (true)

#define NUM_FUNCTIONS_SWITCHES          0
#define NUM_TRIMS                       6
#define DEFAULT_STICK_DEADZONE          2

#define BATTERY_WARN                    74
#define BATTERY_MIN                     68
#define BATTERY_MAX                     86

#define BACKLIGHT_LEVEL_MAX             100
#define BACKLIGHT_LEVEL_MIN             1
#define BACKLIGHT_FORCED_ON             (BACKLIGHT_LEVEL_MAX + 1)

void pwrInit();
void pwrOn();
void pwrOff();
uint32_t pwrCheck();
bool pwrPressed();
bool pwrOffPressed();
void pwrResetHandler();

void lcdInit();
void lcdOn();
void lcdOff();
void lcdCopy(void* dest, void* src);
void lcdSetInitalFrameBuffer(void* fb);

void backlightInit();
void backlightEnable(uint8_t level);
void backlightFullOn();
bool isBacklightEnabled();
#define BACKLIGHT_ENABLE() backlightEnable(g_eeGeneral.backlightBright)
#define BACKLIGHT_DISABLE() backlightEnable(g_eeGeneral.blOffBright)

void audioInit();
void audioConsumeCurrentBuffer();

void hapticInit();
void hapticDone();
void hapticOff();
void hapticOn(uint32_t pwmPercent);

void ledInit();
void ledOff();
void ledRed();
void ledGreen();
void ledBlue();

const etx_serial_port_t* auxSerialGetPort(int port_nr);

bool touchPanelEventOccured();
struct TouchState touchPanelRead();
struct TouchState getInternalTouchState();
void touchPanelInit();
