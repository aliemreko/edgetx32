# EdgeTX32

**Open-source EdgeTX fork for ESP32-S3**

EdgeTX32 brings the EdgeTX radio-control firmware stack to Espressif ESP32-S3
using ESP-IDF — keeping the portable mixer, logical switches, Lua, color UI,
YAML models and UART module protocols (CRSF / ELRS, Multi, Ghost, …), while
replacing the STM32 HAL with an ESP32 board target.

> Based on [EdgeTX](https://github.com/EdgeTX/edgetx) (GPLv2).  
> This is an independent community fork focused on ESP32-S3 DIY radios.

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](LICENSE)
[![Target](https://img.shields.io/badge/MCU-ESP32--S3-orange.svg)](docs/esp32/PINOUT.md)
[![Build](https://img.shields.io/badge/build-ESP--IDF%20v5.1%2B-green.svg)](docs/esp32/BUILD.md)

---

## Why EdgeTX32?

| | Upstream EdgeTX | **EdgeTX32** |
|--|--|--|
| MCU | STM32 F4 / H7 families | **ESP32-S3** |
| RTOS | Vendored FreeRTOS | **ESP-IDF FreeRTOS** |
| UI RAM | SDRAM (H7) | **PSRAM / SPIRAM** |
| Connectivity | Optional BT module | **Wi‑Fi + BLE on-die** |
| Cores | Single application core | **Dual-core** (UI ↔ mixer) |
| DIY cost | Full radio BOM | **DevKit + ELRS module** |

Host side-by-side simulation (hwgen / mixer / LS / CRSF / ADC / scheduler models):

| Metric | Upstream | EdgeTX32 |
|--|--|--|
| Automated checks | **100%** | **100%** |
| Mixer headroom @ 250 Hz (medium) | ~99× | ~100× |
| Scheduler deadline misses (model) | ~0.02% | **0%** |
| HAL coverage (12 pieces) | Complete | Complete |
| CRSF / Multi / Ghost / … pulses | Present | Present (portable) |

Full write-up: [`docs/STATISTICS.md`](docs/STATISTICS.md) · PDF: [`reports/EdgeTX_vs_ESP32_Comparison_Report.pdf`](reports/EdgeTX_vs_ESP32_Comparison_Report.pdf)

---

## Quick start

```bash
# 1) Install ESP-IDF v5.1+ and export the environment
. $IDF_PATH/export.sh

# 2) Build
./tools/esp32/build.sh

# 3) Flash
cd platforms/esp32
idf.py -p /dev/ttyACM0 flash monitor
```

Hardware JSON / generator self-check (no IDF required):

```bash
./tools/esp32/verify_hw_gen.sh
```

More detail: [docs/esp32/BUILD.md](docs/esp32/BUILD.md) · [docs/esp32/PINOUT.md](docs/esp32/PINOUT.md) · [docs/esp32/ARCHITECTURE.md](docs/esp32/ARCHITECTURE.md)

---

## What changed vs upstream EdgeTX?

See **[CHANGELOG.md](CHANGELOG.md)** and **[docs/WHAT_CHANGED.md](docs/WHAT_CHANGED.md)**.

Short version:

1. New PCB target: `ESP32S3` → `radio/src/targets/esp32s3/`
2. ESP-IDF project: `platforms/esp32/`
3. Dual-core affinity, PSRAM framebuffer, Wi‑Fi telemetry mirror, BLE trainer stub
4. STM32 HAL/CMSIS trees omitted from this snapshot (restore from upstream if needed)
5. `RTOS_START()` is a no-op under ESP-IDF (scheduler already running)

Portable radio core (`mixer`, `pulses/`, `telemetry/`, `gui/colorlcd`, `lua`, `storage/yaml`) is intentionally kept.

---

## Repository layout

```
platforms/esp32/          ESP-IDF app + edgetx component
radio/src/targets/esp32s3 ESP32-S3 board HAL
radio/src/boards/hw_defs/esp32s3.json
docs/esp32/               Architecture, build, pinout
reports/                  Comparison PDF + JSON + charts
tools/esp32/              build / verify / compare helpers
```

---

## Status

| Area | State |
|--|--|
| HAL contracts for ESP32-S3 | Implemented |
| Mixer gptimer scheduler | Implemented |
| CRSF/ELRS UART module ports | Wired |
| Color UI path (`colorlcd`) | Linked — bring-up per panel |
| SD + YAML models | SPI SD diskio |
| Wi‑Fi / BLE extras | Present (stubs → extend) |
| Production OEM radio | Not a drop-in TX16S firmware |

EdgeTX32 is **early / DIY bring-up**. Expect to remap pins in `hal.h` + `esp32s3.json` for your PCB.

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). Issues and PRs that improve HAL drivers,
panel support (ST7796 / ILI9488), ELRS wiring guides, and Companion notes are welcome.

---

## License & attribution

- Firmware: **GPLv2** — same family as EdgeTX / OpenTX ([LICENSE](LICENSE))
- Upstream: [EdgeTX/edgetx](https://github.com/EdgeTX/edgetx)
- See [NOTICE](NOTICE) for third-party notes

---

## Türkçe özet

EdgeTX32, EdgeTX radyo yazılımının **ESP32-S3** üzerinde çalışan açık kaynak fork’udur.
Mixer, Lua, renkli arayüz ve CRSF/ELRS gibi protokoller korunur; donanım katmanı
ESP-IDF ile yeniden yazılmıştır. Kurulum: `docs/esp32/BUILD.md`. Değişiklikler:
`docs/WHAT_CHANGED.md`. Karşılaştırma raporu: `reports/`.
