#!/usr/bin/env bash
# Build EdgeTX ESP32-S3 firmware with ESP-IDF
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
IDF_PROJECT="${ROOT}/platforms/esp32"

if [[ -z "${IDF_PATH:-}" ]]; then
  echo "IDF_PATH is not set. Install ESP-IDF v5.1+ and run: . \$IDF_PATH/export.sh"
  exit 1
fi

cd "${IDF_PROJECT}"
idf.py set-target esp32s3
idf.py build "$@"

echo
echo "Firmware binary: ${IDF_PROJECT}/build/edgetx-esp32s3.bin"
echo "Flash: idf.py -p /dev/ttyUSB0 flash monitor"
