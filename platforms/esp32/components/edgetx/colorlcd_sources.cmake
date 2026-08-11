# Real EdgeTX colorlcd + LVGL sources for the ESP-IDF component.
# Must stay scriptable during IDF early expansion (no SET_SOURCE_FILES_PROPERTIES).

set(COLORLCD_DIR ${RADIO_SRC}/gui/colorlcd)
set(LVGL_SRC_DIR ${RADIO_SRC}/thirdparty/lvgl/src)
set(FONT_DIR std)
set(UTILS_DIR ${EDGETX_ROOT}/radio/util)

# --- LVGL (software renderer only; skip STM32 DMA2D / SDL / unused extras) ---
file(GLOB LVGL_SOURCES
  ${LVGL_SRC_DIR}/core/*.c
  ${LVGL_SRC_DIR}/draw/*.c
  ${LVGL_SRC_DIR}/draw/sw/*.c
  ${LVGL_SRC_DIR}/font/lv_font.c
  ${LVGL_SRC_DIR}/font/lv_font_fmt_txt.c
  ${LVGL_SRC_DIR}/hal/*.c
  ${LVGL_SRC_DIR}/misc/*.c
  ${LVGL_SRC_DIR}/widgets/*.c
  ${LVGL_SRC_DIR}/extra/lv_extra.c
  ${LVGL_SRC_DIR}/extra/others/snapshot/*.c
  ${LVGL_SRC_DIR}/extra/layouts/grid/*.c
  ${LVGL_SRC_DIR}/extra/layouts/flex/*.c
  ${LVGL_SRC_DIR}/extra/libs/qrcode/*.c
  ${LVGL_SRC_DIR}/extra/libs/fsdrv/lv_fs_fatfs.c
  ${LVGL_SRC_DIR}/extra/widgets/tileview/*.c
  ${LVGL_SRC_DIR}/extra/widgets/keyboard/*.c
)
list(FILTER LVGL_SOURCES EXCLUDE REGEX "lv_draw_sw_dither|lv_log\\.c|lv_lru\\.c|lv_txt_ap\\.c")

# --- EN LVGL fonts for 480x272 (FONT_DIR=std) ---
file(GLOB COLORLCD_FONT_SRCS
  ${RADIO_SRC}/fonts/lvgl/${FONT_DIR}/lv_font_en_*.c
)

# --- colorlcd UI tree (feature-filtered to match enabled protocols) ---
file(GLOB COLORLCD_LIBUI_SRCS ${COLORLCD_DIR}/libui/*.cpp)
file(GLOB COLORLCD_THEMES_SRCS ${COLORLCD_DIR}/themes/*.cpp)
file(GLOB COLORLCD_LAYOUTS_SRCS ${COLORLCD_DIR}/layouts/*.cpp)
file(GLOB COLORLCD_WIDGETS_SRCS ${COLORLCD_DIR}/widgets/*.cpp)
file(GLOB COLORLCD_MAINVIEW_SRCS ${COLORLCD_DIR}/mainview/*.cpp)
file(GLOB COLORLCD_MENUS_SRCS ${COLORLCD_DIR}/setup_menus/*.cpp)
file(GLOB COLORLCD_CONTROLS_SRCS ${COLORLCD_DIR}/controls/*.cpp)

set(COLORLCD_CORE_SRCS
  ${COLORLCD_DIR}/bitmaps.cpp
  ${COLORLCD_DIR}/colors.cpp
  ${COLORLCD_DIR}/fonts.cpp
  ${COLORLCD_DIR}/lcd.cpp
  ${COLORLCD_DIR}/LvglWrapper.cpp
  ${COLORLCD_DIR}/startup_shutdown.cpp
  ${COLORLCD_DIR}/model/curveedit.cpp
  ${COLORLCD_DIR}/model/input_edit.cpp
  ${COLORLCD_DIR}/model/mixer_edit_adv.cpp
  ${COLORLCD_DIR}/model/mixer_edit.cpp
  ${COLORLCD_DIR}/model/model_curves.cpp
  ${COLORLCD_DIR}/model/model_flightmodes.cpp
  ${COLORLCD_DIR}/model/model_gvars.cpp
  ${COLORLCD_DIR}/model/model_inputs.cpp
  ${COLORLCD_DIR}/model/model_logical_switches.cpp
  ${COLORLCD_DIR}/model/model_mixes.cpp
  ${COLORLCD_DIR}/model/model_outputs.cpp
  ${COLORLCD_DIR}/model/model_select.cpp
  ${COLORLCD_DIR}/model/model_setup.cpp
  ${COLORLCD_DIR}/model/model_telemetry.cpp
  ${COLORLCD_DIR}/model/model_templates.cpp
  ${COLORLCD_DIR}/model/output_edit.cpp
  ${COLORLCD_DIR}/model/preflight_checks.cpp
  ${COLORLCD_DIR}/model/special_functions.cpp
  ${COLORLCD_DIR}/model/timer_setup.cpp
  ${COLORLCD_DIR}/model/trainer_setup.cpp
  ${COLORLCD_DIR}/module/custom_failsafe.cpp
  ${COLORLCD_DIR}/module/module_setup.cpp
  ${COLORLCD_DIR}/module/ppm_settings.cpp
  ${COLORLCD_DIR}/module/crossfire_settings.cpp
  ${COLORLCD_DIR}/module/bind_menu_d16.cpp
  ${COLORLCD_DIR}/radio/hw_extmodule.cpp
  ${COLORLCD_DIR}/radio/hw_inputs.cpp
  ${COLORLCD_DIR}/radio/hw_intmodule.cpp
  ${COLORLCD_DIR}/radio/hw_serial.cpp
  ${COLORLCD_DIR}/radio/preview_window.cpp
  ${COLORLCD_DIR}/radio/radio_calibration.cpp
  ${COLORLCD_DIR}/radio/radio_diaganas.cpp
  ${COLORLCD_DIR}/radio/radio_diagkeys.cpp
  ${COLORLCD_DIR}/radio/radio_hardware.cpp
  ${COLORLCD_DIR}/radio/radio_sdmanager.cpp
  ${COLORLCD_DIR}/radio/radio_setup.cpp
  ${COLORLCD_DIR}/radio/radio_theme.cpp
  ${COLORLCD_DIR}/radio/radio_tools.cpp
  ${COLORLCD_DIR}/radio/radio_trainer.cpp
  ${COLORLCD_DIR}/radio/radio_version.cpp
  ${COLORLCD_DIR}/radio/radio_gps_tool.cpp
  ${COLORLCD_DIR}/radio/radio_mic_recorder.cpp
)

# Embedded Lua (required by colorlcd tools / widgets / statistics)
set(LUA_DIR ${RADIO_SRC}/thirdparty/Lua/src)
set(LUA_CORE_SRCS
  ${LUA_DIR}/lapi.c
  ${LUA_DIR}/lcode.c
  ${LUA_DIR}/lctype.c
  ${LUA_DIR}/ldebug.c
  ${LUA_DIR}/ldo.c
  ${LUA_DIR}/ldump.c
  ${LUA_DIR}/lfunc.c
  ${LUA_DIR}/lgc.c
  ${LUA_DIR}/llex.c
  ${LUA_DIR}/lmem.c
  ${LUA_DIR}/lobject.c
  ${LUA_DIR}/lopcodes.c
  ${LUA_DIR}/lparser.c
  ${LUA_DIR}/lstate.c
  ${LUA_DIR}/lstring.c
  ${LUA_DIR}/ltable.c
  ${LUA_DIR}/ltm.c
  ${LUA_DIR}/lundump.c
  ${LUA_DIR}/lvm.c
  ${LUA_DIR}/lzio.c
  ${LUA_DIR}/linit.c
  ${LUA_DIR}/lbaselib.c
  ${LUA_DIR}/lmathlib.c
  ${LUA_DIR}/lbitlib.c
  ${LUA_DIR}/loadlib.c
  ${LUA_DIR}/lauxlib.c
  ${LUA_DIR}/ltablib.c
  ${LUA_DIR}/lcorolib.c
  ${LUA_DIR}/liolib.c
  ${LUA_DIR}/lstrlib.c
)
set(LUA_API_SRCS
  ${RADIO_SRC}/lua/interface.cpp
  ${RADIO_SRC}/lua/api_general.cpp
  ${RADIO_SRC}/lua/api_model.cpp
  ${RADIO_SRC}/lua/api_filesystem.cpp
  ${RADIO_SRC}/lua/lua_event.cpp
  ${RADIO_SRC}/lua/api_colorlcd.cpp
  ${RADIO_SRC}/lua/api_colorlcd_lvgl.cpp
  ${RADIO_SRC}/lua/widgets.cpp
  ${RADIO_SRC}/lua/lua_widget.cpp
  ${RADIO_SRC}/lua/lua_widget_factory.cpp
  ${RADIO_SRC}/lua/lua_lvgl_widget.cpp
  ${COLORLCD_DIR}/standalone_lua.cpp
  ${COLORLCD_DIR}/model/model_mixer_scripts.cpp
)

set(COLORLCD_SUPPORT_SRCS
  ${RADIO_SRC}/main.cpp
  ${RADIO_SRC}/gui/gui_common.cpp
  ${RADIO_SRC}/gui/screenshot.cpp
  ${RADIO_SRC}/model_init.cpp
  ${RADIO_SRC}/datastructs_model.cpp
  ${RADIO_SRC}/datastructs_radio.cpp
  ${RADIO_SRC}/logs.cpp
  ${RADIO_SRC}/stamp.cpp
  ${RADIO_SRC}/cfn_sort.cpp
  ${RADIO_SRC}/sbus.cpp
  ${RADIO_SRC}/gyro.cpp
  ${RADIO_SRC}/hal/imu.cpp
  ${RADIO_SRC}/thirdparty/lz4/lz4.c
  ${RADIO_SRC}/translations/tts/tts_en.cpp
  ${LUA_CORE_SRCS}
  ${LUA_API_SRCS}
)

set(COLORLCD_ALL_SRCS
  ${LVGL_SOURCES}
  ${COLORLCD_FONT_SRCS}
  ${COLORLCD_LIBUI_SRCS}
  ${COLORLCD_THEMES_SRCS}
  ${COLORLCD_LAYOUTS_SRCS}
  ${COLORLCD_WIDGETS_SRCS}
  ${COLORLCD_MAINVIEW_SRCS}
  ${COLORLCD_MENUS_SRCS}
  ${COLORLCD_CONTROLS_SRCS}
  ${COLORLCD_CORE_SRCS}
  ${COLORLCD_SUPPORT_SRCS}
)

set(COLORLCD_INCLUDE_DIRS
  ${COLORLCD_DIR}
  ${COLORLCD_DIR}/layouts
  ${COLORLCD_DIR}/libui
  ${COLORLCD_DIR}/model
  ${COLORLCD_DIR}/radio
  ${COLORLCD_DIR}/module
  ${COLORLCD_DIR}/mainview
  ${COLORLCD_DIR}/setup_menus
  ${COLORLCD_DIR}/themes
  ${COLORLCD_DIR}/controls
  ${COLORLCD_DIR}/widgets
  ${RADIO_SRC}/thirdparty/lvgl
  ${RADIO_SRC}/thirdparty/lz4
  ${RADIO_SRC}/fonts/lvgl
  ${RADIO_SRC}/gui
  ${RADIO_SRC}/gui/common
  ${LUA_DIR}
)

# Generate .lbm includes from PNGs into GEN_DIR (needed by bitmaps.cpp).
# Not scriptable during IDF early expansion.
set(BITMAP_LZ4_ARGS --size-format 2 --lz4)
set(COLORLCD_LBM_OUTPUTS "")
if(NOT CMAKE_BUILD_EARLY_EXPANSION)
  file(GLOB COLORLCD_PNGS
    ${RADIO_SRC}/bitmaps/480x272/bmp_*.png
    ${RADIO_SRC}/bitmaps/480x272/mask_*.png
  )
  foreach(png ${COLORLCD_PNGS})
    get_filename_component(_stem ${png} NAME_WE)
    set(_lbm "${GEN_DIR}/${_stem}.lbm")
    if(_stem MATCHES "^mask_")
      set(_fmt 8bits)
    else()
      set(_fmt "4/4/4/4")
    endif()
    add_custom_command(
      OUTPUT ${_lbm}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${GEN_DIR}
      COMMAND python3 ${UTILS_DIR}/encode-bitmap.py
              --format ${_fmt} ${BITMAP_LZ4_ARGS} ${png} ${_lbm}
      DEPENDS ${png} ${UTILS_DIR}/encode-bitmap.py
      COMMENT "Encoding ${_stem}.lbm"
      VERBATIM
    )
    list(APPEND COLORLCD_LBM_OUTPUTS ${_lbm})
  endforeach()
  add_custom_target(edgetx_colorlcd_bitmaps DEPENDS ${COLORLCD_LBM_OUTPUTS})
endif()
