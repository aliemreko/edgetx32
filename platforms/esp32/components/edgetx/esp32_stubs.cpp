/*
 * Copyright (C) EdgeTX / EdgeTX-ESP32 contributors
 * Based on EdgeTX - https://github.com/EdgeTX/edgetx
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * Temporary link stubs for modules not yet ported to the ESP-IDF build
 * (colorlcd/LVGL GUI, model templates, optional telemetry helpers).
 */

#include "heartbeat_driver.h"
#include "hal/module_driver.h"
#include "edgetx_types.h"
#include "dataconstants.h"
#include "edgetx_constants.h"
#include "pulses/modules_constants.h"

#include <cstdint>
#include <cstring>
#include <string>

struct gtm;

volatile HeartbeatCapture heartbeatCapture = {};

uint8_t requiredSpeakerVolume = 255;
uint8_t currentSpeakerVolume = 255;
uint8_t requiredBacklightBright = 0;
uint8_t currentBacklightBright = 0;
uint8_t mainRequestFlags = 0;
uint8_t menuCalibrationState = 0;
uint16_t logDelay100ms = 0;

int16_t gyroScaledX() { return 0; }
int16_t gyroScaledY() { return 0; }
void gyroWakeup() {}

// rtc.cpp calls into the board RTC driver
void rtcdriver_settime(struct gtm*) {}
void rtcSetTime(const struct gtm*) {}

extern "C" void luaInitMainState() {}
void luaInit() {}
void luaClose() {}
void luaTask() {}

void perMain() {}
void checkAll(bool) {}
void logsInit() {}
void logsClose() {}
void startSplash() {}
void cancelShutdownAnimation() {}
void drawSleepBitmap() {}
void runFatalErrorScreen(const char*) {}
void POPUP_WARNING(const char*, const char*) {}
void setRequestedMainView(uint8_t) {}
extern "C" void initLvgl() {}

bool isInternalModuleAvailable(int moduleType)
{
  return moduleType == MODULE_TYPE_NONE || moduleType == MODULE_TYPE_CROSSFIRE;
}
bool isExternalModuleAvailable(int moduleType)
{
  return moduleType == MODULE_TYPE_NONE || moduleType == MODULE_TYPE_CROSSFIRE;
}
bool isTelemetryFieldAvailable(int) { return false; }

bool getStickInversion(int) { return false; }
void setStickInversion(int, bool) {}
uint8_t getPotType(int) { return 0; }
bool getPotInversion(int) { return false; }

void setModelDefaults(uint8_t) {}
void applyDefaultTemplate() {}

void frskyDSetDefault(int, uint16_t) {}
void frskySportSetDefault(int, uint16_t, uint8_t, uint8_t) {}
void spektrumSetDefault(int, uint16_t, uint8_t, uint8_t) {}

void trainer_stop_dsc() {}
void trainer_stop_module_cppm() {}
void sbusAuxSetEnabled(bool) {}
void sbusSetReceiveCtx(void*, const etx_serial_driver_t*) {}
void sbusAuxFrameReceived(void*) {}

// Minimal TTS language pack table (full multi-lang packs not linked yet).
struct LanguagePack {
  const char* id;
  const char* (*name)();
  void (*playNumber)(int, uint8_t, uint8_t, uint8_t, uint8_t);
  void (*playDuration)(int, uint8_t, uint8_t, uint8_t);
};
static const char* en_name() { return "English"; }
static void tts_noop_number(int, uint8_t, uint8_t, uint8_t, uint8_t) {}
static void tts_noop_duration(int, uint8_t, uint8_t, uint8_t) {}
static const LanguagePack enLanguagePack = {"en", en_name, tts_noop_number, tts_noop_duration};
extern const LanguagePack* currentLanguagePack;
const LanguagePack* currentLanguagePack = &enLanguagePack;
uint8_t currentLanguagePackIdx = 0;
extern const LanguagePack* const languagePacks[];
const LanguagePack* const languagePacks[] = {&enLanguagePack, nullptr};

// --- GUI / colorlcd stubs (mangled names must match production headers) ---

class MainWindow {
 public:
  static MainWindow* instance();
  void shutdown();
};
MainWindow* MainWindow::instance()
{
  static MainWindow win;
  return &win;
}
void MainWindow::shutdown() {}

class ThemePersistance {
 public:
  static ThemePersistance themePersistance;
  void loadDefaultTheme();
};
ThemePersistance ThemePersistance::themePersistance;
void ThemePersistance::loadDefaultTheme() {}

class LayoutFactory {
 public:
  static void loadCustomScreens();
  static void loadDefaultLayout();
};
void LayoutFactory::loadCustomScreens() {}
void LayoutFactory::loadDefaultLayout() {}

enum WidgetOptionValueEnum {
  WOV_Unsigned = 0,
  WOV_Signed,
  WOV_Bool,
  WOV_String,
};

class WidgetPersistentData {
 public:
  bool hasOption(int);
  WidgetOptionValueEnum getType(int);
  void setType(int, WidgetOptionValueEnum);
  int32_t getSignedValue(int);
  void setSignedValue(int, int32_t);
  uint32_t getUnsignedValue(int);
  void setUnsignedValue(int, uint32_t);
  std::string getString(int);
  void setString(int, const char*);
};
bool WidgetPersistentData::hasOption(int) { return false; }
WidgetOptionValueEnum WidgetPersistentData::getType(int) { return WOV_Unsigned; }
void WidgetPersistentData::setType(int, WidgetOptionValueEnum) {}
int32_t WidgetPersistentData::getSignedValue(int) { return 0; }
void WidgetPersistentData::setSignedValue(int, int32_t) {}
uint32_t WidgetPersistentData::getUnsignedValue(int) { return 0; }
void WidgetPersistentData::setUnsignedValue(int, uint32_t) {}
std::string WidgetPersistentData::getString(int) { return {}; }
void WidgetPersistentData::setString(int, const char*) {}

class LayoutPersistentData {
 public:
  WidgetPersistentData* getWidgetData(int);
};
static WidgetPersistentData s_widgetData;
WidgetPersistentData* LayoutPersistentData::getWidgetData(int)
{
  return &s_widgetData;
}

class TopBarPersistentData {
 public:
  WidgetPersistentData* getWidgetData(int);
  void setWidgetName(int, const char*);
  bool hasWidget(int);
};
WidgetPersistentData* TopBarPersistentData::getWidgetData(int)
{
  return &s_widgetData;
}
void TopBarPersistentData::setWidgetName(int, const char*) {}
bool TopBarPersistentData::hasWidget(int) { return false; }

struct CustomScreenData;
class ModelData {
 public:
  SwitchConfig getSwitchType(uint8_t);
  bool hasScreenData(int);
  CustomScreenData* getScreenData(int);
  TopBarPersistentData* getTopbarData();
  LayoutPersistentData* getScreenLayoutData(int);
  void resetScreenData();
};
SwitchConfig ModelData::getSwitchType(uint8_t) { return SWITCH_NONE; }
bool ModelData::hasScreenData(int) { return false; }
CustomScreenData* ModelData::getScreenData(int) { return nullptr; }
static TopBarPersistentData s_topbar;
TopBarPersistentData* ModelData::getTopbarData() { return &s_topbar; }
static LayoutPersistentData s_layout;
LayoutPersistentData* ModelData::getScreenLayoutData(int) { return &s_layout; }
void ModelData::resetScreenData() {}

enum QMPage { QM_NONE = 0 };
class RadioData {
 public:
  void defaultKeyShortcuts();
  uint16_t getKeyShortcut(uint16_t);
  std::string getKeyToolName(uint16_t);
  void setKeyShortcut(uint16_t, QMPage);
  void setKeyToolName(uint16_t, std::string);
  std::string getFavoriteToolName(int);
  int getKeyShortcutEvent(int);
  void setFavoriteToolName(int, std::string);
};
void RadioData::defaultKeyShortcuts() {}
uint16_t RadioData::getKeyShortcut(uint16_t) { return 0; }
std::string RadioData::getKeyToolName(uint16_t) { return {}; }
void RadioData::setKeyShortcut(uint16_t, QMPage) {}
void RadioData::setKeyToolName(uint16_t, std::string) {}
std::string RadioData::getFavoriteToolName(int) { return {}; }
int RadioData::getKeyShortcutEvent(int) { return -1; }
void RadioData::setFavoriteToolName(int, std::string) {}
