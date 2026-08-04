# EdgeTX-ESP32 Architecture

## Goal

Run the full EdgeTX radio stack (mixer, logical switches, special functions,
Lua, color LVGL UI, YAML models on SD, CRSF/ELRS and other UART module
protocols) on **ESP32-S3**, using ESP-IDF instead of STM32 HAL.

## Layering

```
┌─────────────────────────────────────────────┐
│ Color UI (LVGL) / Lua / Companion YAML      │  portable
├─────────────────────────────────────────────┤
│ Mixer · Inputs · Logical SW · Telemetry     │  portable
├─────────────────────────────────────────────┤
│ pulses/ (CRSF, Multi, Ghost, PPM, …)        │  portable
├─────────────────────────────────────────────┤
│ radio/src/hal/* contracts                   │  portable API
├─────────────────────────────────────────────┤
│ targets/esp32s3/*  ESP-IDF drivers          │  NEW
├─────────────────────────────────────────────┤
│ ESP32-S3 · FreeRTOS · PSRAM · WiFi · BLE    │
└─────────────────────────────────────────────┘
```

STM32-only trees (`targets/common/arm/stm32`, `boards/generic_stm32`, CMSIS,
STM32 HAL) are **not** linked for `PCB=ESP32S3`.

## Dual-core mapping

| Core | Workload |
|------|----------|
| 0 | UI (`menus`), LVGL, WiFi, BLE, audio helper |
| 1 | Mixer scheduler ISR + mixer task (realtime) |

Mixer cadence remains ~4 ms via ESP-IDF `gptimer` → `mixerSchedulerISRTrigger()`.

## ESP32-only enhancements

- **PSRAM framebuffer** for 480×272 RGB565 UI
- **WiFi telemetry mirror** (UDP :9070 JSON broadcast)
- **BLE trainer** transport stub (NimBLE)
- **USB** via TinyUSB (CDC/MSC/HID can be enabled in `sdkconfig`)
- **Task WDT** instead of STM32 IWDG

## Hardware description

`radio/src/boards/hw_defs/esp32s3.json` feeds the existing Jinja generators:

- portable: `hal_keys`, `hal_adc_inputs`, `yaml_inputs`, `lua_*`
- ESP32: `esp32_keys`, `esp32_switches`, `esp32_adc_inputs`

Pin mux for a DIY radio is documented in `PINOUT.md`. Remap in `hal.h` /
JSON for your PCB (IO expander recommended for dense switch panels).

## Build entry points

1. **ESP-IDF (primary):** `platforms/esp32` → `tools/esp32/build.sh`
2. **CMake PCB flag:** `-DPCB=ESP32S3` configures the EdgeTX tree and
   exports the board OBJECT library; firmware link is done by IDF.
