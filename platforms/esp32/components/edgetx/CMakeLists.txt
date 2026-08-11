# EdgeTX portable core + ESP32-S3 HAL as an ESP-IDF component.
#
# This component intentionally compiles the portable radio sources that are
# MCU-agnostic, plus the ESP32S3 board HAL under radio/src/targets/esp32s3.

set(EDGETX_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../..")
set(RADIO_SRC "${EDGETX_ROOT}/radio/src")
set(ESP32_TARGET "${RADIO_SRC}/targets/esp32s3")

# Generate portable HW definition includes (no ESP-IDF required for this step)
set(HW_JSON "${RADIO_SRC}/boards/hw_defs/esp32s3.json")
set(HW_GEN "${RADIO_SRC}/../util/hw_defs/generate_hw_def.py")
set(GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GEN_DIR})

# Version stamp header (normally produced by radio/src/CMakeLists.txt)
string(TIMESTAMP _ETX_DATE "%Y-%m-%d")
string(TIMESTAMP _ETX_TIME "%H:%M:%S")
string(TIMESTAMP BUILD_YEAR "%Y")
set(DATE "${_ETX_DATE}")
set(TIME "${_ETX_TIME}")
set(VERSION_MAJOR "3")
set(VERSION_MINOR "0")
set(VERSION_REVISION "0")
set(VERSION "3.0.0")
set(VERSION_PREFIX "pre-")
set(VERSION_SUFFIX "-esp32")
set(CODENAME "esp32s3")
set(GIT_STR "esp32")
# Optional tag left undefined for #cmakedefine
configure_file(
  "${RADIO_SRC}/stamp.h.in"
  "${GEN_DIR}/stamp.h"
  @ONLY
)

set(GEN_TEMPLATES
  hal_settings:hal_settings.h
  hal_keys:hal_keys.inc
  hal_adc_inputs:hal_adc_inputs.inc
  hal_keys_lock:hal_keys_lock.h
  yaml_inputs:yaml_inputs.inc
  lua_inputs:lua_inputs.inc
  lua_mixsrc:lua_mixsrc.inc
  lua_keys:lua_keys.inc
  simu_switches:simu_switches.inc
  esp32_keys:esp32_keys.inc
  esp32_switches:esp32_switches.inc
  esp32_adc_inputs:esp32_adc_inputs.inc
)

# ESP-IDF includes this file in script mode during early expansion to collect
# REQUIRES. Commands like add_custom_command are not allowed in that pass.
if(NOT CMAKE_BUILD_EARLY_EXPANSION)
  set(GEN_OUTPUTS "")
  foreach(pair ${GEN_TEMPLATES})
    string(REPLACE ":" ";" parts ${pair})
    list(GET parts 0 tmpl)
    list(GET parts 1 out)
    set(out_path "${GEN_DIR}/${out}")
    add_custom_command(
      OUTPUT ${out_path}
      COMMAND bash -c "python3 '${HW_GEN}' -t '${RADIO_SRC}/../util/hw_defs/${tmpl}.jinja' -T esp32s3 '${HW_JSON}' > '${out_path}'"
      DEPENDS ${HW_JSON} ${RADIO_SRC}/../util/hw_defs/${tmpl}.jinja ${HW_GEN}
      COMMENT "Generating ${out}"
      VERBATIM
    )
    list(APPEND GEN_OUTPUTS ${out_path})
  endforeach()
  add_custom_target(edgetx_hw_gen DEPENDS ${GEN_OUTPUTS})
endif()

set(ESP32_HAL_SRCS
  ${ESP32_TARGET}/board.cpp
  ${ESP32_TARGET}/gpio_driver.cpp
  ${ESP32_TARGET}/adc_driver.cpp
  ${ESP32_TARGET}/serial_driver.cpp
  ${ESP32_TARGET}/module_ports.cpp
  ${ESP32_TARGET}/mixer_scheduler_driver.cpp
  ${ESP32_TARGET}/timers_driver.cpp
  ${ESP32_TARGET}/delays_driver.cpp
  ${ESP32_TARGET}/lcd_driver.cpp
  ${ESP32_TARGET}/backlight_driver.cpp
  ${ESP32_TARGET}/audio_driver.cpp
  ${ESP32_TARGET}/haptic_driver.cpp
  ${ESP32_TARGET}/key_driver.cpp
  ${ESP32_TARGET}/switch_driver.cpp
  ${ESP32_TARGET}/sd_diskio.cpp
  ${ESP32_TARGET}/pwr_driver.cpp
  ${ESP32_TARGET}/watchdog_driver.cpp
  ${ESP32_TARGET}/cpu_id.cpp
  ${ESP32_TARGET}/abnormal_reboot.cpp
  ${ESP32_TARGET}/led_driver.cpp
  ${ESP32_TARGET}/rotary_encoder_driver.cpp
  ${ESP32_TARGET}/touch_driver.cpp
  ${ESP32_TARGET}/usb_driver.cpp
  ${ESP32_TARGET}/aux_serial.cpp
  ${ESP32_TARGET}/wifi_telemetry.cpp
  ${ESP32_TARGET}/ble_trainer.cpp
  ${ESP32_TARGET}/esp32_affinity.cpp
  ${CMAKE_CURRENT_LIST_DIR}/edgetx_esp32_start.cpp
  ${CMAKE_CURRENT_LIST_DIR}/esp32_stubs.cpp
)

# Portable HAL helpers
# Note: hal/storage.cpp pulls STM32 diskio_spi — ESP32 uses sd_diskio.cpp instead.
list(APPEND ESP32_HAL_SRCS
  ${RADIO_SRC}/hal/adc_driver.cpp
  ${RADIO_SRC}/hal/key_driver.cpp
  ${RADIO_SRC}/hal/switch_driver.cpp
  ${RADIO_SRC}/hal/module_port.cpp
  ${RADIO_SRC}/hal/fatfs_diskio.cpp
  ${RADIO_SRC}/hal/usb_driver.cpp
)

# Core radio engine (feature-complete portable subset)
set(EDGETX_CORE_SRCS
  ${RADIO_SRC}/edgetx.cpp
  ${RADIO_SRC}/mixer.cpp
  ${RADIO_SRC}/mixes.cpp
  ${RADIO_SRC}/curves.cpp
  ${RADIO_SRC}/gvars.cpp
  ${RADIO_SRC}/switches.cpp
  ${RADIO_SRC}/keys.cpp
  ${RADIO_SRC}/functions.cpp
  ${RADIO_SRC}/analogs.cpp
  ${RADIO_SRC}/maths.cpp
  ${RADIO_SRC}/crc.cpp
  ${RADIO_SRC}/strhelpers.cpp
  ${RADIO_SRC}/audio.cpp
  ${RADIO_SRC}/haptic.cpp
  ${RADIO_SRC}/timers.cpp
  ${RADIO_SRC}/tasks.cpp
  ${RADIO_SRC}/tasks/mixer_task.cpp
  ${RADIO_SRC}/mixer_scheduler.cpp
  ${RADIO_SRC}/serial.cpp
  ${RADIO_SRC}/trainer.cpp
  ${RADIO_SRC}/sdcard.cpp
  ${RADIO_SRC}/lib_file.cpp
  ${RADIO_SRC}/debug.cpp
  ${RADIO_SRC}/os/async_freertos.cpp
  ${RADIO_SRC}/os/sleep_freertos.cpp
  ${RADIO_SRC}/os/task_freertos.cpp
  ${RADIO_SRC}/os/time_freertos.cpp
  ${RADIO_SRC}/os/timer_freertos.cpp
)

# Protocols + telemetry: only sources matching enabled feature flags.
# (A full GLOB pulls Multi/PXX/AFHDS/etc. that need extra defines and STM32 deps.)
set(PULSES_SRCS
  ${RADIO_SRC}/pulses/pulses.cpp
  ${RADIO_SRC}/pulses/modules_helpers.cpp
  ${RADIO_SRC}/pulses/crossfire.cpp
)
set(TELEMETRY_SRCS
  ${RADIO_SRC}/telemetry/telemetry.cpp
  ${RADIO_SRC}/telemetry/telemetry_sensors.cpp
  ${RADIO_SRC}/telemetry/crossfire.cpp
)
# yaml_datastructs_<board>.cpp files are #included by yaml_datastructs.cpp — do not compile them alone.
set(STORAGE_SRCS
  ${RADIO_SRC}/storage/modelslist.cpp
  ${RADIO_SRC}/storage/rlc.cpp
  ${RADIO_SRC}/storage/rtc_backup.cpp
  ${RADIO_SRC}/storage/sdcard_common.cpp
  ${RADIO_SRC}/storage/sdcard_yaml.cpp
  ${RADIO_SRC}/storage/storage_common.cpp
  ${RADIO_SRC}/storage/yaml/yaml_bits.cpp
  ${RADIO_SRC}/storage/yaml/yaml_datastructs.cpp
  ${RADIO_SRC}/storage/yaml/yaml_labelslist.cpp
  ${RADIO_SRC}/storage/yaml/yaml_modelslist.cpp
  ${RADIO_SRC}/storage/yaml/yaml_parser.cpp
  ${RADIO_SRC}/storage/yaml/yaml_tree_walker.cpp
)

idf_component_register(
  SRCS
    ${ESP32_HAL_SRCS}
    ${EDGETX_CORE_SRCS}
    ${PULSES_SRCS}
    ${TELEMETRY_SRCS}
    ${STORAGE_SRCS}
  INCLUDE_DIRS
    ${RADIO_SRC}
    ${ESP32_TARGET}
    ${GEN_DIR}
    ${RADIO_SRC}/thirdparty
    ${RADIO_SRC}/thirdparty/FatFs
    ${RADIO_SRC}/gui
    ${RADIO_SRC}/gui/colorlcd
    ${RADIO_SRC}/gui/colorlcd/layouts
    ${RADIO_SRC}/gui/colorlcd/libui
    ${RADIO_SRC}/gui/colorlcd/model
    ${RADIO_SRC}/gui/colorlcd/radio
    ${RADIO_SRC}/gui/colorlcd/module
    ${RADIO_SRC}/gui/colorlcd/mainview
    ${RADIO_SRC}/gui/colorlcd/setup_menus
    ${RADIO_SRC}/gui/colorlcd/themes
    ${RADIO_SRC}/gui/colorlcd/controls
    ${RADIO_SRC}/gui/common
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/include
    ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS
  REQUIRES
    driver esp_timer esp_adc esp_lcd fatfs nvs_flash esp_wifi bt esp_psram
  PRIV_REQUIRES
    heap
)

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
  add_dependencies(${COMPONENT_LIB} edgetx_hw_gen)

  target_compile_definitions(${COMPONENT_LIB} PUBLIC
    FREE_RTOS
    ESP_PLATFORM
    PCBESP32S3
    PCBHORUS
    # Reuse Horus/X10 YAML + storage layout until ESP32 gets its own schema.
    PCBX10
    COLORLCD
    HARDWARE_TOUCH
    HARDWARE_KEYS
    HARDWARE_INTERNAL_MODULE
    HARDWARE_EXTERNAL_MODULE
    SOFTWARE_KEYBOARD
    RTCLOCK
    USE_WATCHDOG
    DISABLE_MCUCHECK
    ESP32_DUAL_CORE
    ESP32_PSRAM_FRAMEBUFFER
    ESP32_WIFI_TELEMETRY
    ESP32_BLE_TRAINER
    INTERNAL_MODULE_CRSF
    DEFAULT_INTERNAL_MODULE=MODULE_TYPE_CROSSFIRE
    FLAVOUR=\"esp32s3\"
    TRANSLATIONS_EN
    TRANSLATIONS=\"EN\"
    TRANSLATION_IS_RTL=0
    THREADSAFE_MALLOC
    CORRECT_NEGATIVE_SHIFTS
    STORAGE_MODELSLIST
    AUDIO
    GVARS
    HAPTIC
    CROSSFIRE
    TIMERS=3
  )

  # EdgeTX still vendors FreeRTOS headers in some paths — prefer IDF's.
  target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-unused-parameter
    -Wno-missing-field-initializers
    -Wno-error=format
    # BitField<> templates can trip -Warray-bounds under inlining on GCC 13.
    -Wno-error=array-bounds
    -Wno-array-bounds
  )
endif()
