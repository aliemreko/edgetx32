# Statistics — EdgeTX32 vs upstream EdgeTX

Generated from the host comparison harness (`tools/esp32/compare_sim.py`).
Environment: Linux CI host, August 2026. **Not** on-radio wall-clock measurements.

## Automated checks

| Side | Pass | Fail | Pass rate |
|--|--|--|--|
| Upstream EdgeTX (TX15 JSON / simu presence / cmake) | 9 | 1 | 90% |
| EdgeTX32 (hwgen + HAL host compile + verify script) | 12 | 0 | **100%** |

Upstream failure in that run: native CMake configure missing optional deps in CI.
EdgeTX32 generators and GPIO host compile succeeded.

## Host benchmarks (same algorithm both sides)

| Bench | Upstream | EdgeTX32 |
|--|--|--|
| Mixer-like loops / s | ~6.47×10⁶ | ~6.46×10⁶ |
| ADC jitter EMA samples / s | ~7.4×10⁸ | ~7.2×10⁸ |
| Mixer scheduler target | 250 Hz (4 ms) | 250 Hz (4 ms) |
| GPIO toggle (ESP host stub) | — | ~3.3×10⁸ / s |

Equal mixer/ADC numbers are expected: identical C++ on the same CPU.
They validate that the **software load model** is comparable, not chip speed.

## Theoretical platform comparison

| Metric | TX15-class STM32H750 | ESP32-S3 |
|--|--|--|
| CPU | Cortex-M7 ~400–480 MHz | Dual LX7 up to 240 MHz |
| UI memory | SDRAM | PSRAM (e.g. 8 MB) |
| Mixer determinism | Mature NVIC + timer | gptimer + core-1 pin |
| Connectivity | Optional BT | Wi‑Fi + BLE integrated |
| DIY availability | Full radio | DevKit + UART module |

## Tree / code stats (snapshot)

| Stat | Upstream shallow tree | EdgeTX32 snapshot |
|--|--|--|
| Approx. tree size | (full clone varies) | ~130 MB vendored |
| `pulses/*.cpp` | Present | Present (portable) |
| `telemetry/*.cpp` | Present | Present (portable) |
| ESP32 target sources | — | 30+ files under `targets/esp32s3` |

## Artifacts

- PDF: [`../reports/EdgeTX_vs_ESP32_Comparison_Report.pdf`](../reports/EdgeTX_vs_ESP32_Comparison_Report.pdf)
- JSON: [`../reports/edgetx_esp32_compare.json`](../reports/edgetx_esp32_compare.json)
- Charts: `reports/compare_*.png`

Re-run:

```bash
python3 tools/esp32/compare_sim.py
python3 tools/esp32/generate_compare_pdf.py
```
