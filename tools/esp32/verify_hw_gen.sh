#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
OUT="${TMPDIR:-/tmp}/etx-esp32-gen"
mkdir -p "$OUT"
TEMPLATES=(
  hal_settings:hal_settings.h
  hal_keys:hal_keys.inc
  hal_adc_inputs:hal_adc_inputs.inc
  simu_switches:simu_switches.inc
  esp32_keys:esp32_keys.inc
  esp32_switches:esp32_switches.inc
  esp32_adc_inputs:esp32_adc_inputs.inc
  yaml_inputs:yaml_inputs.inc
  lua_keys:lua_keys.inc
  lua_inputs:lua_inputs.inc
  lua_mixsrc:lua_mixsrc.inc
  hal_keys_lock:hal_keys_lock.h
)
for pair in "${TEMPLATES[@]}"; do
  tmpl="${pair%%:*}"; out="${pair##*:}"
  python3 "$ROOT/radio/util/hw_defs/generate_hw_def.py" \
    -t "$ROOT/radio/util/hw_defs/${tmpl}.jinja" -T esp32s3 \
    "$ROOT/radio/src/boards/hw_defs/esp32s3.json" > "$OUT/$out"
  echo "OK $out"
done
g++ -std=c++17 -c -I "$ROOT/radio/src" -I "$ROOT/radio/src/targets/esp32s3" \
  "$ROOT/radio/src/targets/esp32s3/gpio_driver.cpp" -o "$OUT/gpio_driver.o"
echo "OK host gpio_driver.o"
echo "All ESP32 hardware-definition checks passed."
