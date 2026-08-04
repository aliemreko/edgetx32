# EdgeTX ESP32-S3 target
# Full color radio feature set with ESP-IDF backed HAL.

option(UNEXPECTED_SHUTDOWN "Enable the Unexpected Shutdown screen" ON)
option(PXX1 "PXX1 protocol support" ON)
option(PXX2 "PXX2 protocol support" ON)
option(AFHDS3 "AFHDS3 TX Module" ON)
option(MULTIMODULE "DIY Multiprotocol TX Module" ON)
option(GHOST "Ghost TX Module" ON)
option(MODULE_SIZE_STD "Standard size TX Module" ON)
option(LUA_MIXER "Enable LUA mixer/model scripts support" ON)
option(DISK_CACHE "Enable SD card disk cache" ON)
option(INTERNAL_GPS "Support for internal GPS" OFF)
option(ESP32_WIFI_TELEMETRY "WiFi telemetry mirror" ON)
option(ESP32_BLE_TRAINER "BLE wireless trainer" ON)

set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")
set(CPU_TYPE ESP32S3)
set(CPU_TYPE_FULL ESP32S3)
set(CPU_FAMILY ESP32)
set(ARCH ESP32)
set(SDCARD YES)
set(STORAGE_MODELSLIST YES)
set(HAPTIC YES)
set(GUI_DIR colorlcd)
set(BITMAPS_DIR 480x272)
set(TARGET_DIR esp32s3)
set(PPM_LIMITS_SYMETRICAL YES)
set(USB_SERIAL ON CACHE BOOL "Enable USB serial (CDC)")
set(HARDWARE_EXTERNAL_MODULE YES)
set(ROTARY_ENCODER YES)
set(BOOTLOADER OFF CACHE BOOL "ESP32 uses IDF bootloader" FORCE)

add_definitions(-DPCBESP32S3 -DPCBHORUS -DCOLORLCD)
add_definitions(-DESP32 -DESP32S3 -DESP_PLATFORM_EDGE)
add_definitions(-DHARDWARE_TOUCH -DHARDWARE_KEYS -DSOFTWARE_KEYBOARD)
add_definitions(-DRTCLOCK)
add_definitions(-DPWR_BUTTON_${PWR_BUTTON})
add_definitions(-DTHREADSAFE_MALLOC)
add_definitions(-DUSE_WATCHDOG)
add_definitions(-DDISABLE_MCUCHECK)

if(ESP32_WIFI_TELEMETRY)
  add_definitions(-DESP32_WIFI_TELEMETRY)
endif()
if(ESP32_BLE_TRAINER)
  add_definitions(-DESP32_BLE_TRAINER)
endif()
add_definitions(-DESP32_DUAL_CORE -DESP32_PSRAM_FRAMEBUFFER)

set(FLAVOUR esp32s3)
add_definitions(-DMANUFACTURER_EDGETX_ESP32)

set(INTERNAL_MODULES CRSF CACHE STRING "Internal modules")
set(DEFAULT_INTERNAL_MODULE CROSSFIRE CACHE STRING "Default internal module")

set(BITMAPS_TARGET bm480_bitmaps)
set(FONTS_TARGET x12_fonts)
set(HARDWARE_TOUCH YES)
set(RADIO_DEPENDENCIES ${RADIO_DEPENDENCIES} ${BITMAPS_TARGET})
set(FIRMWARE_DEPENDENCIES datacopy)
set(SOFTWARE_KEYBOARD ON)
set(AUX_SERIAL YES)

if(NOT UNEXPECTED_SHUTDOWN)
  add_definitions(-DNO_UNEXPECTED_SHUTDOWN)
endif()

if(DISK_CACHE)
  set(SRC ${SRC} disk_cache.cpp)
  add_definitions(-DDISK_CACHE)
endif()

set(AFHDS3 ON)
set(ENABLE_SERIAL_PASSTHROUGH ON CACHE BOOL "Enable serial passthrough")
set(CLI ON CACHE BOOL "Enable CLI")

include_directories(targets/${TARGET_DIR})
include_directories(${RADIO_SRC_DIR}/fonts/colorlcd gui/${GUI_DIR} gui/${GUI_DIR}/layouts)

set(TARGET_SRC_DIR targets/${TARGET_DIR})

set(BOARD_COMMON_SRC
  ${TARGET_SRC_DIR}/board.cpp
  ${TARGET_SRC_DIR}/gpio_driver.cpp
  ${TARGET_SRC_DIR}/adc_driver.cpp
  ${TARGET_SRC_DIR}/serial_driver.cpp
  ${TARGET_SRC_DIR}/module_ports.cpp
  ${TARGET_SRC_DIR}/mixer_scheduler_driver.cpp
  ${TARGET_SRC_DIR}/timers_driver.cpp
  ${TARGET_SRC_DIR}/delays_driver.cpp
  ${TARGET_SRC_DIR}/lcd_driver.cpp
  ${TARGET_SRC_DIR}/backlight_driver.cpp
  ${TARGET_SRC_DIR}/audio_driver.cpp
  ${TARGET_SRC_DIR}/haptic_driver.cpp
  ${TARGET_SRC_DIR}/key_driver.cpp
  ${TARGET_SRC_DIR}/switch_driver.cpp
  ${TARGET_SRC_DIR}/sd_diskio.cpp
  ${TARGET_SRC_DIR}/pwr_driver.cpp
  ${TARGET_SRC_DIR}/watchdog_driver.cpp
  ${TARGET_SRC_DIR}/cpu_id.cpp
  ${TARGET_SRC_DIR}/abnormal_reboot.cpp
  ${TARGET_SRC_DIR}/led_driver.cpp
  ${TARGET_SRC_DIR}/rotary_encoder_driver.cpp
  ${TARGET_SRC_DIR}/touch_driver.cpp
  ${TARGET_SRC_DIR}/usb_driver.cpp
  ${TARGET_SRC_DIR}/aux_serial.cpp
  ${TARGET_SRC_DIR}/wifi_telemetry.cpp
  ${TARGET_SRC_DIR}/ble_trainer.cpp
  ${TARGET_SRC_DIR}/esp32_affinity.cpp
)

# Firmware board library (consumed by classic EdgeTX CMake and exported to ESP-IDF)
add_library(board OBJECT EXCLUDE_FROM_ALL ${BOARD_COMMON_SRC})
add_library(board_bl OBJECT EXCLUDE_FROM_ALL ${BOARD_COMMON_SRC})
add_library(board_lib INTERFACE)
add_library(minimal_board_lib INTERFACE)

# Placeholder so shared CMake paths that expect STM32 objects do not fail
add_library(stm32_drivers_w_dbg_fw INTERFACE)

set(FIRMWARE_SRC ${FIRMWARE_SRC} $<TARGET_OBJECTS:board>)

set(SRC
  ${SRC}
  io/frsky_firmware_update.cpp
  io/multi_firmware_update.cpp
)

if(MULTIMODULE)
  add_definitions(-DMULTI_PROTOLIST)
  set(SRC ${SRC} io/multi_protolist.cpp)
endif()

# Export source list for ESP-IDF component
set(ESP32_BOARD_SOURCES ${BOARD_COMMON_SRC} PARENT_SCOPE)
set(ESP32_TARGET_DIR ${TARGET_SRC_DIR} PARENT_SCOPE)
