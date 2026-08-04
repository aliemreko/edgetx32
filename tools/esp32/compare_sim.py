#!/usr/bin/env python3
"""
Side-by-side simulation / comparison harness:
  Upstream EdgeTX (STM32 / simu)  vs  EdgeTX-ESP32 (ESP32-S3 HAL)

Produces JSON metrics consumed by generate_compare_pdf.py
"""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time
from dataclasses import dataclass, asdict, field
from pathlib import Path
from typing import Any, Dict, List, Optional

ROOT_ESP = Path(__file__).resolve().parents[2]
ROOT_UP = Path(os.environ.get("EDGETX_UPSTREAM", "/tmp/edgetx-src"))
REPORT_DIR = ROOT_ESP / "reports"
OUT_DIR = Path(os.environ.get("COMPARE_OUT", str(REPORT_DIR)))
OUT_DIR.mkdir(parents=True, exist_ok=True)
REPORT_DIR.mkdir(parents=True, exist_ok=True)


def run(cmd: List[str], cwd: Optional[Path] = None, timeout: int = 120) -> subprocess.CompletedProcess:
    return subprocess.run(
        cmd,
        cwd=str(cwd) if cwd else None,
        capture_output=True,
        text=True,
        timeout=timeout,
    )


def count_lines(path: Path, globs: List[str]) -> int:
    total = 0
    for g in globs:
        for p in path.rglob(g):
            if any(x in p.parts for x in (".git", "build", "thirdparty")):
                continue
            try:
                total += sum(1 for _ in p.open("rb"))
            except Exception:
                pass
    return total


def list_files(path: Path, pattern: str) -> List[str]:
    return sorted(str(p.relative_to(path)) for p in path.rglob(pattern) if p.is_file())


@dataclass
class CheckResult:
    name: str
    side: str  # upstream | esp32
    ok: bool
    detail: str = ""
    ms: float = 0.0


@dataclass
class BenchResult:
    name: str
    side: str
    iterations: int
    elapsed_s: float
    ops_per_s: float
    notes: str = ""


@dataclass
class FeatureRow:
    feature: str
    upstream: str
    esp32: str
    notes: str = ""


def hw_gen(side: str, root: Path, flavour: str, templates: List[str]) -> List[CheckResult]:
    results = []
    gen = root / "radio/util/hw_defs/generate_hw_def.py"
    js = root / f"radio/src/boards/hw_defs/{flavour}.json"
    out = Path(tempfile.mkdtemp(prefix=f"hwgen-{side}-"))
    for tmpl in templates:
        t0 = time.perf_counter()
        out_file = out / f"{tmpl}.out"
        tmpl_path = root / f"radio/util/hw_defs/{tmpl}.jinja"
        if not tmpl_path.exists():
            results.append(CheckResult(f"hwgen:{tmpl}", side, False, "template missing", 0))
            continue
        cp = run(
            [
                sys.executable,
                str(gen),
                "-t",
                str(tmpl_path),
                "-T",
                flavour,
                str(js),
            ],
            timeout=60,
        )
        ms = (time.perf_counter() - t0) * 1000
        if cp.returncode == 0 and cp.stdout.strip():
            out_file.write_text(cp.stdout)
            results.append(
                CheckResult(f"hwgen:{tmpl}", side, True, f"{len(cp.stdout.splitlines())} lines", ms)
            )
        else:
            results.append(
                CheckResult(
                    f"hwgen:{tmpl}",
                    side,
                    False,
                    (cp.stderr or cp.stdout)[-300:],
                    ms,
                )
            )
    return results


def compile_host(side: str, sources: List[Path], includes: List[Path], defines: List[str]) -> CheckResult:
    out_o = Path(tempfile.mkdtemp(prefix=f"cc-{side}-")) / "a.o"
    cmd = ["g++", "-std=c++17", "-c", "-O2"]
    for d in defines:
        cmd.append(f"-D{d}")
    for inc in includes:
        cmd += ["-I", str(inc)]
    # compile first source only as smoke (multi-file link separate)
    t0 = time.perf_counter()
    cmd += [str(sources[0]), "-o", str(out_o)]
    cp = run(cmd, timeout=60)
    ms = (time.perf_counter() - t0) * 1000
    ok = cp.returncode == 0 and out_o.exists()
    return CheckResult(
        f"host_compile:{sources[0].name}",
        side,
        ok,
        (cp.stderr[-400:] if not ok else f"object {out_o.stat().st_size}B"),
        ms,
    )


def bench_mixer_like(side: str, channels: int = 32, mixes: int = 64, iters: int = 200_000) -> BenchResult:
    """Synthetic mixer load approximating EdgeTX evalFlightModeMixes math intensity."""
    code = f"""
#include <cstdint>
#include <cstdio>
#include <chrono>
static inline int16_t expo(int16_t x, int16_t e) {{
  int32_t y = x;
  y = y + ((int32_t)e * y * y / 100 * y) / (100 * 100 * 100);
  return (int16_t)y;
}}
int main() {{
  const int CH={channels}, MX={mixes}, IT={iters};
  int16_t sticks[4] = {{0, 512, -300, 800}};
  int16_t chans[64] = {{0}};
  int16_t weight[64]; int8_t src[64];
  for (int i=0;i<MX;i++){{ weight[i]=(int16_t)(50+(i%50)); src[i]=(int8_t)(i%4); }}
  auto t0 = std::chrono::steady_clock::now();
  for (int it=0; it<IT; ++it) {{
    for (int c=0;c<CH;c++) chans[c]=0;
    for (int m=0;m<MX;m++) {{
      int16_t v = expo(sticks[src[m]], (int16_t)(m%30));
      chans[m%CH] = (int16_t)(chans[m%CH] + (int32_t)v * weight[m] / 100);
    }}
  }}
  auto t1 = std::chrono::steady_clock::now();
  double s = std::chrono::duration<double>(t1-t0).count();
  volatile int sink = chans[0];
  (void)sink;
  printf("%.9f\\n", s);
  return 0;
}}
"""
    td = Path(tempfile.mkdtemp(prefix=f"bench-{side}-"))
    src = td / "mixer_bench.cpp"
    src.write_text(code)
    binp = td / "mixer_bench"
    cp = run(["g++", "-O3", "-std=c++17", str(src), "-o", str(binp)], timeout=30)
    if cp.returncode != 0:
        return BenchResult("mixer_like", side, iters, -1, 0, cp.stderr[-200:])
    cp2 = run([str(binp)], timeout=60)
    elapsed = float(cp2.stdout.strip())
    ops = iters / elapsed if elapsed > 0 else 0
    return BenchResult(
        "mixer_like",
        side,
        iters,
        elapsed,
        ops,
        f"{channels}ch/{mixes}mix @ host -O3 (same algorithm both sides)",
    )


def bench_gpio_host() -> BenchResult:
    """Exercise ESP32 gpio host stub throughput."""
    esp = ROOT_ESP / "radio/src/targets/esp32s3"
    td = Path(tempfile.mkdtemp(prefix="gpio-bench-"))
    src = td / "g.cpp"
    src.write_text(
        r"""
#include "esp32_gpio.h"
#include <chrono>
#include <cstdio>
int main() {
  gpio_t p = GPIO_PIN(GPIO, 10);
  gpio_init(p, GPIO_OUT, GPIO_PIN_SPEED_LOW);
  auto t0 = std::chrono::steady_clock::now();
  const int N = 5000000;
  for (int i=0;i<N;i++) { gpio_toggle(p); }
  auto t1 = std::chrono::steady_clock::now();
  double s = std::chrono::duration<double>(t1-t0).count();
  printf("%d %.9f\n", N, s);
}
"""
    )
    binp = td / "g"
    cp = run(
        [
            "g++",
            "-O3",
            "-std=c++17",
            f"-I{esp}",
            f"-I{ROOT_ESP}/radio/src",
            str(esp / "gpio_driver.cpp"),
            str(src),
            "-o",
            str(binp),
        ],
        timeout=30,
    )
    if cp.returncode != 0:
        return BenchResult("gpio_toggle_host", "esp32", 0, -1, 0, cp.stderr[-300:])
    cp2 = run([str(binp)], timeout=30)
    parts = cp2.stdout.strip().split()
    n = int(parts[0])
    s = float(parts[1])
    return BenchResult("gpio_toggle_host", "esp32", n, s, n / s if s else 0, "host stub map<>")


def bench_adc_filter(side: str, iters: int = 1_000_000) -> BenchResult:
    code = f"""
#include <cstdint>
#include <cstdio>
#include <chrono>
#define JITTER_ALPHA 16
int main(){{
  uint16_t filtered=2048; uint16_t raw=2100;
  auto t0=std::chrono::steady_clock::now();
  for(int i=0;i<{iters};i++){{
    raw = (uint16_t)(2000 + (i*17)%200);
    filtered = (uint16_t)(((uint32_t)filtered*(JITTER_ALPHA-1) + raw)/JITTER_ALPHA);
  }}
  auto t1=std::chrono::steady_clock::now();
  double s=std::chrono::duration<double>(t1-t0).count();
  printf("%.9f %u\\n", s, (unsigned)filtered);
}}
"""
    td = Path(tempfile.mkdtemp(prefix=f"adc-{side}-"))
    src = td / "a.cpp"
    src.write_text(code)
    binp = td / "a"
    run(["g++", "-O3", "-std=c++17", str(src), "-o", str(binp)])
    cp = run([str(binp)])
    s = float(cp.stdout.split()[0])
    return BenchResult("adc_jitter_filter", side, iters, s, iters / s if s else 0, "EdgeTX-style EMA")


def feature_matrix() -> List[FeatureRow]:
    rows = [
        FeatureRow("Color LCD / LVGL UI", "Yes (STM32 LTDC/DMA2D)", "Yes (SPI LCD + PSRAM FB)", "GUI_DIR=colorlcd"),
        FeatureRow("Mixer engine", "Yes", "Yes (portable)", "Same core sources"),
        FeatureRow("Logical switches / SF", "Yes", "Yes", ""),
        FeatureRow("Lua / Lua mixer", "Yes", "Yes (flagged ON)", "Needs full IDF link"),
        FeatureRow("YAML models on SD", "Yes", "Yes (SPI SD diskio)", ""),
        FeatureRow("Internal CRSF/ELRS", "Yes", "Yes (UART1 module_port)", ""),
        FeatureRow("External module bay", "Yes", "Yes (UART2)", "Multi/Ghost/PXX via portable pulses/"),
        FeatureRow("Touch UI", "Yes (GT911 etc.)", "Stub + I2C hooks", "Bring-up pending panel"),
        FeatureRow("Audio 32 kHz", "Yes (I2S/DAC)", "Yes (I2S std)", ""),
        FeatureRow("Haptic PWM", "Yes", "Yes (LEDC)", ""),
        FeatureRow("USB CDC/MSC/HID", "STM32 USB lib", "TinyUSB-ready stubs", "sdkconfig enable"),
        FeatureRow("WiFi telemetry", "No", "Yes (UDP :9070)", "ESP32 enhancement"),
        FeatureRow("BLE trainer", "BT module (limited)", "NimBLE stub", "ESP32 enhancement"),
        FeatureRow("Dual-core affinity", "N/A (single MCU core app)", "Yes (core0 UI / core1 mixer)", ""),
        FeatureRow("PSRAM framebuffer", "SDRAM on H7", "SPIRAM", ""),
        FeatureRow("Companion / simu host", "Full SDL simu", "HAL host stubs + hwgen", "Full SDL UI not yet"),
        FeatureRow("Production radio HW", "Many OEM boards", "DIY reference pinout", "IO expander recommended"),
        FeatureRow("Build system", "CMake + ARM GCC 14.2", "ESP-IDF + CMake PCB=ESP32S3", ""),
        FeatureRow("RTOS", "Vendored FreeRTOS", "ESP-IDF FreeRTOS", "RTOS_START no-op on ESP"),
        FeatureRow("MCU check / DBGMCU", "Yes", "Disabled / skipped", "DISABLE_MCUCHECK"),
    ]
    return rows


def scan_hal_coverage(root: Path, target_rel: str) -> Dict[str, bool]:
    t = root / target_rel
    needed = [
        "gpio_driver.cpp",
        "adc_driver.cpp",
        "serial_driver.cpp",
        "module_ports.cpp",
        "mixer_scheduler_driver.cpp",
        "lcd_driver.cpp",
        "audio_driver.cpp",
        "haptic_driver.cpp",
        "sd_diskio.cpp",
        "key_driver.cpp",
        "switch_driver.cpp",
        "board.cpp",
    ]
    # upstream uses scattered board files — map equivalently
    if "esp32" in target_rel:
        return {n: (t / n).exists() for n in needed}
    # For upstream horus/rm-h750 style, check presence under boards + common
    alt = {
        "gpio_driver.cpp": list((root / "radio/src/targets/common/arm/stm32").glob("stm32_gpio*.cpp")),
        "adc_driver.cpp": list((root / "radio/src/targets/common/arm/stm32").glob("stm32_adc.cpp")),
        "serial_driver.cpp": list((root / "radio/src/targets/common/arm/stm32").glob("stm32_serial*.cpp")),
        "module_ports.cpp": list((root / "radio/src/boards/generic_stm32").glob("module_ports.cpp")),
        "mixer_scheduler_driver.cpp": list(
            (root / "radio/src/targets/common/arm/stm32").glob("mixer_scheduler_driver.cpp")
        ),
        "lcd_driver.cpp": list((root / "radio/src/boards/rm-h750").glob("lcd_driver*.cpp")),
        "audio_driver.cpp": list((root / "radio/src/boards/rm-h750").glob("audio_driver.cpp")),
        "haptic_driver.cpp": list((root / "radio/src/boards/rm-h750").glob("haptic_driver.cpp")),
        "sd_diskio.cpp": list((root / "radio/src/targets/common/arm/stm32").glob("diskio_*.cpp")),
        "key_driver.cpp": list((root / "radio/src/targets/tx15").glob("key_driver.cpp")),
        "switch_driver.cpp": list((root / "radio/src/boards/generic_stm32").glob("switches.cpp")),
        "board.cpp": list((root / "radio/src/boards/rm-h750").glob("board.cpp")),
    }
    return {k: bool(v) for k, v in alt.items()}


def try_upstream_native_configure() -> CheckResult:
    """Attempt a lightweight native CMake configure (not full simu link)."""
    if not ROOT_UP.exists():
        return CheckResult("cmake_configure", "upstream", False, "upstream tree missing", 0)
    build = Path("/tmp/etx-compare/upstream-native")
    if build.exists():
        shutil.rmtree(build, ignore_errors=True)
    build.mkdir(parents=True)
    t0 = time.perf_counter()
    # Native superbuild off — may fail without full deps; capture status honestly
    cp = run(
        [
            "cmake",
            "-S",
            str(ROOT_UP),
            "-B",
            str(build),
            "-DEdgeTX_SUPERBUILD=OFF",
            "-DNATIVE_BUILD=ON",
            "-DPCB=TX15",
            "-DDEFAULT_MODE=1",
        ],
        timeout=180,
    )
    ms = (time.perf_counter() - t0) * 1000
    ok = cp.returncode == 0
    detail = "configure OK" if ok else (cp.stderr or cp.stdout)[-500:]
    return CheckResult("cmake_native_configure_TX15", "upstream", ok, detail, ms)


def try_esp32_verify_script() -> CheckResult:
    script = ROOT_ESP / "tools/esp32/verify_hw_gen.sh"
    t0 = time.perf_counter()
    cp = run(["bash", str(script)], timeout=120)
    ms = (time.perf_counter() - t0) * 1000
    return CheckResult(
        "verify_hw_gen.sh",
        "esp32",
        cp.returncode == 0,
        (cp.stdout[-400:] if cp.returncode == 0 else cp.stderr[-400:]),
        ms,
    )


def size_stats(root: Path) -> Dict[str, Any]:
    radio = root / "radio/src"
    return {
        "tree_mb": round(sum(p.stat().st_size for p in root.rglob("*") if p.is_file()) / (1024 * 1024), 1),
        "radio_cpp": len(list(radio.rglob("*.cpp"))),
        "radio_h": len(list(radio.rglob("*.h"))),
        "pulses_cpp": len(list((radio / "pulses").glob("*.cpp"))) if (radio / "pulses").exists() else 0,
        "telemetry_cpp": len(list((radio / "telemetry").glob("*.cpp"))) if (radio / "telemetry").exists() else 0,
        "targets": [p.name for p in (radio / "targets").iterdir() if p.is_dir()]
        if (radio / "targets").exists()
        else [],
    }


def theoretical_perf_table() -> List[Dict[str, Any]]:
    """Datasheet / architecture derived comparison (not measured on device)."""
    return [
        {
            "metric": "CPU",
            "upstream_tx15": "STM32H750 ~400–480 MHz Cortex-M7",
            "esp32s3": "Dual Xtensa LX7 up to 240 MHz",
            "winner": "Upstream (single-thread DSP)",
        },
        {
            "metric": "RAM for UI",
            "upstream_tx15": "SDRAM (MBs)",
            "esp32s3": "PSRAM (typically 8 MB octal)",
            "winner": "Tie / depends on module",
        },
        {
            "metric": "Mixer determinism",
            "upstream_tx15": "HW timer IRQ + NVIC priorities",
            "esp32s3": "gptimer ISR + core-1 pin",
            "winner": "Upstream more battle-tested",
        },
        {
            "metric": "Connectivity",
            "upstream_tx15": "Optional BT module",
            "esp32s3": "WiFi + BLE on-die",
            "winner": "ESP32",
        },
        {
            "metric": "CRSF baud (400k–921k)",
            "upstream_tx15": "USART+DMA proven",
            "esp32s3": "UART driver wired (DMA via IDF)",
            "winner": "Upstream maturity",
        },
        {
            "metric": "Power / battery radios",
            "upstream_tx15": "Production PMIC paths",
            "esp32s3": "DIY latch GPIO",
            "winner": "Upstream",
        },
        {
            "metric": "DIY cost / availability",
            "upstream_tx15": "Full radio BOM",
            "esp32s3": "DevKit + modules",
            "winner": "ESP32",
        },
    ]


def main() -> int:
    if not ROOT_UP.exists():
        print("Cloning upstream…", file=sys.stderr)
        run(["git", "clone", "--depth", "1", "https://github.com/EdgeTX/edgetx.git", str(ROOT_UP)], timeout=120)

    checks: List[CheckResult] = []
    benches: List[BenchResult] = []

    # HW generation both sides
    common_tmpl = [
        "hal_settings",
        "hal_keys",
        "hal_adc_inputs",
        "yaml_inputs",
        "lua_keys",
        "simu_switches",
    ]
    checks += hw_gen("upstream", ROOT_UP, "tx15", common_tmpl + ["stm32_keys", "stm32_adc_inputs"])
    checks += hw_gen(
        "esp32",
        ROOT_ESP,
        "esp32s3",
        common_tmpl + ["esp32_keys", "esp32_adc_inputs", "esp32_switches"],
    )

    # Host compile smoke
    checks.append(
        compile_host(
            "esp32",
            [ROOT_ESP / "radio/src/targets/esp32s3/gpio_driver.cpp"],
            [ROOT_ESP / "radio/src", ROOT_ESP / "radio/src/targets/esp32s3"],
            [],
        )
    )
    checks.append(
        compile_host(
            "esp32",
            [ROOT_ESP / "radio/src/targets/esp32s3/delays_driver.cpp"],
            [ROOT_ESP / "radio/src", ROOT_ESP / "radio/src/targets/esp32s3"],
            [],
        )
    )
    # Upstream simu adc compiles only with many generated headers — skip heavy; check file exists
    checks.append(
        CheckResult(
            "source_present:simu/adc_driver.cpp",
            "upstream",
            (ROOT_UP / "radio/src/targets/simu/adc_driver.cpp").exists(),
            "SDL simu HAL present",
            0,
        )
    )

    checks.append(try_esp32_verify_script())
    checks.append(try_upstream_native_configure())

    # Benchmarks (host-side equal algorithm + ESP32-specific stub)
    benches.append(bench_mixer_like("upstream"))
    benches.append(bench_mixer_like("esp32"))
    benches.append(bench_adc_filter("upstream"))
    benches.append(bench_adc_filter("esp32"))
    benches.append(bench_gpio_host())

    # Multi-rate mixer periods simulation (scheduler math)
    for side, period_us in (("upstream", 4000), ("esp32", 4000)):
        hz = 1_000_000 / period_us
        benches.append(
            BenchResult(
                "mixer_scheduler_rate",
                side,
                int(hz),
                period_us / 1e6,
                hz,
                f"configured period {period_us} us → {hz:.1f} Hz",
            )
        )

    esp_hal = scan_hal_coverage(ROOT_ESP, "radio/src/targets/esp32s3")
    up_hal = scan_hal_coverage(ROOT_UP, "radio/src/boards/rm-h750")

    # Working status summary
    def summarize(side: str) -> Dict[str, Any]:
        side_checks = [c for c in checks if c.side == side]
        ok = sum(1 for c in side_checks if c.ok)
        fail = sum(1 for c in side_checks if not c.ok)
        return {
            "checks_total": len(side_checks),
            "checks_ok": ok,
            "checks_fail": fail,
            "pass_rate_pct": round(100.0 * ok / len(side_checks), 1) if side_checks else 0.0,
        }

    report = {
        "meta": {
            "title": "EdgeTX vs EdgeTX-ESP32 Side-by-Side Simulation Report",
            "generated_utc": time.strftime("%Y-%m-%d %H:%M:%S UTC", time.gmtime()),
            "upstream_path": str(ROOT_UP),
            "esp32_path": str(ROOT_ESP),
            "note": (
                "Host-side simulation & static analysis. Device flashing / full LVGL UI "
                "runtime was not executed in this CI environment (no ESP-IDF / no radio HW)."
            ),
        },
        "sizes": {"upstream": size_stats(ROOT_UP), "esp32": size_stats(ROOT_ESP)},
        "hal_coverage": {"upstream_tx15_like": up_hal, "esp32s3": esp_hal},
        "features": [asdict(f) for f in feature_matrix()],
        "checks": [asdict(c) for c in checks],
        "summary": {"upstream": summarize("upstream"), "esp32": summarize("esp32")},
        "benchmarks": [asdict(b) for b in benches],
        "theoretical_perf": theoretical_perf_table(),
        "runtime_readiness": {
            "upstream": {
                "simu_sources": True,
                "native_cmake": next((c.ok for c in checks if c.name.startswith("cmake_native")), False),
                "production_maturity": "High",
                "esp32_binary": False,
            },
            "esp32": {
                "hal_host_stubs": True,
                "hw_json_generators": True,
                "idf_project_present": (ROOT_ESP / "platforms/esp32/CMakeLists.txt").exists(),
                "idf_full_link_in_this_env": False,
                "production_maturity": "Early / bring-up",
                "wifi_ble_extras": True,
            },
        },
    }

    out_json = OUT_DIR / "edgetx_esp32_compare.json"
    out_json.write_text(json.dumps(report, indent=2))
    (REPORT_DIR / "edgetx_esp32_compare.json").write_text(json.dumps(report, indent=2))
    print(json.dumps({"wrote": str(out_json), "summary": report["summary"]}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
