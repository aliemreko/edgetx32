# Third-party for EdgeTX-ESP32

- **lvgl**, **Lua**, **FatFs**, **stb**, **uf2**, **AccessDenied**, **lz4**: vendored
- **FreeRTOS**: provided by ESP-IDF (see `platforms/esp32/components/edgetx/FreeRTOS`)
- **STM32 HAL / CMSIS / USB / Segger**: omitted — restore from upstream EdgeTX if you
  also need STM32 targets in this tree
