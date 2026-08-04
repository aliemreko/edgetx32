# Building EdgeTX for ESP32-S3

## Requirements

- ESP-IDF **v5.1+** (v5.2/v5.3 recommended)
- Python 3.10+ with `pydantic`, `jinja2`
- ESP32-S3 module with **≥8 MB flash** and **PSRAM** (N8R8 / N16R8)

## Quick start

```bash
. $IDF_PATH/export.sh
./tools/esp32/build.sh
cd platforms/esp32 && idf.py -p /dev/ttyACM0 flash monitor
```

## CMake PCB integration

```bash
cmake -B build-esp32 -DPCB=ESP32S3 -DEdgeTX_SUPERBUILD=OFF -DNATIVE_BUILD=OFF
```

This configures generators and the `board` object library. The linkable
firmware image is produced by the ESP-IDF project under `platforms/esp32`.

## Feature flags (target CMake)

| Flag | Default | Meaning |
|------|---------|---------|
| `ESP32_WIFI_TELEMETRY` | ON | UDP telemetry mirror |
| `ESP32_BLE_TRAINER` | ON | NimBLE trainer stub |
| `INTERNAL_MODULES` | CRSF | Internal ELRS/Crossfire UART |
| `LUA` / `LUA_MIXER` | ON | Full Lua |
| `STORAGE_MODELSLIST` | YES | SD YAML models |

## SD card layout

Same as upstream EdgeTX color radios: place the SD pack under `/` of the FAT
filesystem (`MODELS`, `RADIO`, `SOUNDS`, `SCRIPTS`, …).
