# What changed vs upstream EdgeTX?

This page is for radio enthusiasts and developers evaluating EdgeTX32.

## Architecture

Upstream EdgeTX is a **portable core + STM32 HAL**. EdgeTX32 keeps the core and
swaps the HAL:

```
EdgeTX portable core (mixer, pulses, telemetry, Lua, colorlcd, YAML)
        │
        ▼
┌─────────────────┐     ┌──────────────────────┐
│ STM32 boards    │     │ ESP32-S3 target      │
│ (TX15, Horus…)  │     │ targets/esp32s3 +    │
│                 │     │ platforms/esp32 IDF  │
└─────────────────┘     └──────────────────────┘
```

## New files (high level)

| Path | Role |
|--|--|
| `radio/src/targets/esp32s3/*` | Full board HAL |
| `radio/src/boards/hw_defs/esp32s3.json` | Keys/ADC/switches/display JSON |
| `platforms/esp32/` | ESP-IDF firmware project |
| `radio/util/hw_defs/esp32_*.jinja` | ESP32 codegen templates |
| `tools/esp32/*` | Build / verify / compare |
| `docs/esp32/*` | Port documentation |
| `reports/*` | Simulation comparison artifacts |

## Behavioral differences

| Topic | Upstream | EdgeTX32 |
|--|--|--|
| Firmware entry | `main()` | `app_main` → `edgeTxEsp32Main()` |
| Start scheduler | `vTaskStartScheduler()` | Already running (IDF) |
| Mixer tick | STM32 timer IRQ | `gptimer` → `mixerSchedulerISRTrigger()` |
| LCD | LTDC / DMA2D (H7) | SPI panel + PSRAM FB |
| Storage | SDIO / SPI STM32 drivers | ESP SDSPI + FatFs diskio |
| MCU check | DBGMCU idcode | Disabled |
| Wi‑Fi telemetry | — | UDP broadcast :9070 (optional) |
| BLE trainer | FrSky BT module path | NimBLE stub |

## What did *not* change

- Mixer math, expo/curves, logical switches, special functions
- Pulse drivers for CRSF / Multi / Ghost / PPM / … (UART framed)
- Telemetry parsers
- Color UI framework (`gui/colorlcd` + LVGL)
- YAML model storage format on SD
- GPLv2 license obligations

## How to use (operators)

1. Build with ESP-IDF (`tools/esp32/build.sh`)
2. Flash an ESP32-S3 module with PSRAM
3. Remap pins in `hal.h` / `esp32s3.json` for your PCB
4. Put a standard EdgeTX SD pack on the FAT partition
5. Connect an ELRS/Crossfire module to the internal UART pins

## How to use (developers)

```bash
./tools/esp32/verify_hw_gen.sh          # JSON → headers
./tools/esp32/compare_sim.py            # regen comparison JSON
./tools/esp32/generate_compare_pdf.py   # regen PDF
```

CMake PCB flag (configures generators; link via IDF):

```bash
cmake -B build-esp32 -DPCB=ESP32S3 -DEdgeTX_SUPERBUILD=OFF
```
