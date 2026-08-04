# Changelog — EdgeTX32

All notable changes for this ESP32-S3 fork relative to upstream EdgeTX.

## [0.1.0-alpha] — 2026-08-03

### Added

- **PCB `ESP32S3`** board family under `radio/src/targets/esp32s3/`
  - GPIO, ADC (oneshot), UART serial driver, module ports (INT/EXT)
  - Mixer scheduler via ESP-IDF `gptimer`
  - SPI LCD + LEDC backlight, I2S audio, haptic PWM
  - SPI SD `diskio`, keys/switches, rotary encoder, power/WDT stubs
  - Dual-core affinity helpers, Wi‑Fi telemetry mirror, BLE trainer stub
- **ESP-IDF project** `platforms/esp32/` (`app_main` → `edgeTxEsp32Main`)
- Hardware description `radio/src/boards/hw_defs/esp32s3.json`
- Jinja templates: `esp32_keys`, `esp32_switches`, `esp32_adc_inputs`
- Docs: architecture, build, pinout, what-changed, statistics
- Host tooling: `verify_hw_gen.sh`, `compare_sim.py`, comparison PDF

### Changed

- `radio/src/CMakeLists.txt`: recognize `PCB=ESP32S3`, ESP32 HW generators,
  early-return before ARM toolchain gate
- `radio/src/rtos.h`: `RTOS_START()` no-op on ESP-IDF
- `radio/src/edgetx.cpp`: `edgeTxEsp32Main()` entry (no conflict with `app_main`),
  skip STM32 `NVIC_SetPriorityGrouping` / MCU ID check on ESP32
- FreeRTOS include shims for IDF under `platforms/esp32/components/edgetx/FreeRTOS/`

### Removed (from this snapshot)

- STM32 HAL / CMSIS / USB device / Segger trees (not required for ESP32 builds).
  Restore from upstream EdgeTX if you also build STM32 targets in-tree.

### Upstream baseline

- Forked from EdgeTX `main` (shallow snapshot, August 2026)
- Portable protocols/UI intentionally retained
