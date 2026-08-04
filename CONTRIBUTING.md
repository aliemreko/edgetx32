# Contributing to EdgeTX32

Thanks for helping make EdgeTX on ESP32 better.

## Ground rules

- Respect **GPLv2** — contributions are licensed under the same terms
- Keep portable EdgeTX core changes minimal and upstream-friendly when possible
- Prefer HAL / board / docs fixes in `targets/esp32s3`, `platforms/esp32`, `docs/`

## Dev loop

```bash
./tools/esp32/verify_hw_gen.sh
. $IDF_PATH/export.sh && ./tools/esp32/build.sh
```

## Good first issues

- Panel bring-up notes (ST7796, ILI9488, GC9A01, …)
- Pin mux for popular DIY shells
- ELRS internal module wiring diagrams
- Touch controller drivers (GT911 / CST816)
- Hardening Wi‑Fi telemetry / BLE trainer beyond stubs

## Code style

Follow existing EdgeTX conventions in touched files (clang-format config in tree).

## Upstream relationship

EdgeTX32 is a **fork**, not an official EdgeTX product. Please do not file ESP32
issues on the upstream EdgeTX tracker unless they reproduce on STM32.
