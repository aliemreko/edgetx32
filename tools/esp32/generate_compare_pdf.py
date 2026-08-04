#!/usr/bin/env python3
"""Generate a comparison PDF from compare_sim.py JSON output."""

from __future__ import annotations

import json
import sys
from pathlib import Path

from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER, TA_LEFT
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import mm
from reportlab.platypus import (
    SimpleDocTemplate,
    Paragraph,
    Spacer,
    Table,
    TableStyle,
    PageBreak,
    KeepTogether,
    HRFlowable,
    Image,
)

ROOT = Path(__file__).resolve().parents[2]
REPORT_DIR = ROOT / "reports"
DEFAULT_JSON = REPORT_DIR / "edgetx_esp32_compare.json"
OUT_PDF = REPORT_DIR / "EdgeTX_vs_ESP32_Comparison_Report.pdf"


def styles():
    base = getSampleStyleSheet()
    return {
        "title": ParagraphStyle(
            "T",
            parent=base["Title"],
            fontName="Helvetica-Bold",
            fontSize=18,
            leading=22,
            alignment=TA_CENTER,
            spaceAfter=6,
        ),
        "sub": ParagraphStyle(
            "S",
            parent=base["Normal"],
            fontName="Helvetica",
            fontSize=10,
            leading=13,
            alignment=TA_CENTER,
            textColor=colors.HexColor("#444444"),
            spaceAfter=12,
        ),
        "h1": ParagraphStyle(
            "H1",
            parent=base["Heading1"],
            fontName="Helvetica-Bold",
            fontSize=13,
            leading=16,
            spaceBefore=10,
            spaceAfter=6,
            textColor=colors.HexColor("#1a1a1a"),
        ),
        "h2": ParagraphStyle(
            "H2",
            parent=base["Heading2"],
            fontName="Helvetica-Bold",
            fontSize=11,
            leading=14,
            spaceBefore=8,
            spaceAfter=4,
            textColor=colors.HexColor("#222222"),
        ),
        "body": ParagraphStyle(
            "B",
            parent=base["Normal"],
            fontName="Helvetica",
            fontSize=9,
            leading=12,
            alignment=TA_LEFT,
            spaceAfter=4,
        ),
        "small": ParagraphStyle(
            "SM",
            parent=base["Normal"],
            fontName="Helvetica",
            fontSize=8,
            leading=10,
            textColor=colors.HexColor("#333333"),
        ),
        "ok": ParagraphStyle("OK", parent=base["Normal"], fontName="Helvetica", fontSize=8, textColor=colors.HexColor("#0a7a32")),
        "fail": ParagraphStyle("FAIL", parent=base["Normal"], fontName="Helvetica", fontSize=8, textColor=colors.HexColor("#b00020")),
    }


def table(data, col_widths=None):
    t = Table(data, colWidths=col_widths, repeatRows=1)
    t.setStyle(
        TableStyle(
            [
                ("BACKGROUND", (0, 0), (-1, 0), colors.HexColor("#1f2937")),
                ("TEXTCOLOR", (0, 0), (-1, 0), colors.white),
                ("FONTNAME", (0, 0), (-1, 0), "Helvetica-Bold"),
                ("FONTSIZE", (0, 0), (-1, -1), 8),
                ("LEADING", (0, 0), (-1, -1), 10),
                ("GRID", (0, 0), (-1, -1), 0.3, colors.HexColor("#cccccc")),
                ("VALIGN", (0, 0), (-1, -1), "TOP"),
                ("LEFTPADDING", (0, 0), (-1, -1), 4),
                ("RIGHTPADDING", (0, 0), (-1, -1), 4),
                ("TOPPADDING", (0, 0), (-1, -1), 3),
                ("BOTTOMPADDING", (0, 0), (-1, -1), 3),
                ("ROWBACKGROUNDS", (0, 1), (-1, -1), [colors.white, colors.HexColor("#f5f7fa")]),
            ]
        )
    )
    return t


def cell(text, style):
    return Paragraph(str(text).replace("\n", "<br/>"), style)


def build(report: dict, out: Path):
    st = styles()
    doc = SimpleDocTemplate(
        str(out),
        pagesize=A4,
        leftMargin=14 * mm,
        rightMargin=14 * mm,
        topMargin=12 * mm,
        bottomMargin=12 * mm,
        title=report["meta"]["title"],
        author="EdgeTX-ESP32 compare harness",
    )
    story = []
    meta = report["meta"]
    story.append(Paragraph(meta["title"], st["title"]))
    story.append(Paragraph(f"Generated: {meta['generated_utc']}", st["sub"]))
    story.append(HRFlowable(width="100%", thickness=1, color=colors.HexColor("#1f2937")))
    story.append(Spacer(1, 6))
    story.append(Paragraph(meta["note"], st["body"]))
    story.append(
        Paragraph(
            f"<b>Upstream:</b> {meta['upstream_path']}<br/>"
            f"<b>ESP32 fork:</b> {meta['esp32_path']}",
            st["small"],
        )
    )

    # Executive summary
    story.append(Paragraph("1. Executive summary", st["h1"]))
    su = report["summary"]["upstream"]
    se = report["summary"]["esp32"]
    story.append(
        Paragraph(
            "Bu rapor, orijinal EdgeTX (TX15/STM32 referans + simu kaynakları) ile "
            "EdgeTX-ESP32 fork’unu aynı host ortamda yan yana simülasyon / statik analiz "
            "ve sentetik performans ölçümleriyle karşılaştırır.",
            st["body"],
        )
    )
    summary_data = [
        [cell("Metric", st["small"]), cell("Upstream EdgeTX", st["small"]), cell("EdgeTX-ESP32", st["small"])],
        [
            cell("Automated checks pass rate", st["small"]),
            cell(f"{su['checks_ok']}/{su['checks_total']} ({su['pass_rate_pct']}%)", st["small"]),
            cell(f"{se['checks_ok']}/{se['checks_total']} ({se['pass_rate_pct']}%)", st["small"]),
        ],
        [
            cell("Tree size (MB)", st["small"]),
            cell(str(report["sizes"]["upstream"]["tree_mb"]), st["small"]),
            cell(str(report["sizes"]["esp32"]["tree_mb"]), st["small"]),
        ],
        [
            cell("radio/*.cpp count", st["small"]),
            cell(str(report["sizes"]["upstream"]["radio_cpp"]), st["small"]),
            cell(str(report["sizes"]["esp32"]["radio_cpp"]), st["small"]),
        ],
        [
            cell("Production maturity", st["small"]),
            cell(report["runtime_readiness"]["upstream"]["production_maturity"], st["small"]),
            cell(report["runtime_readiness"]["esp32"]["production_maturity"], st["small"]),
        ],
        [
            cell("WiFi / BLE extras", st["small"]),
            cell("Limited / module", st["small"]),
            cell("Yes (fork)", st["small"]),
        ],
        [
            cell("Full device UI runtime in this env", st["small"]),
            cell("Not run (no SDL full build)", st["small"]),
            cell("Not run (no ESP-IDF)", st["small"]),
        ],
    ]
    story.append(table(summary_data, [45 * mm, 65 * mm, 65 * mm]))

    chart_pass = REPORT_DIR / "compare_pass_rate.png"
    chart_mixer = REPORT_DIR / "compare_mixer.png"
    chart_hal = REPORT_DIR / "compare_hal.png"
    chart_jitter = REPORT_DIR / "compare_scheduler_jitter.png"
    chart_thru = REPORT_DIR / "compare_sim_throughput.png"
    chart_loc = REPORT_DIR / "compare_module_loc.png"
    if chart_pass.exists():
        story.append(Spacer(1, 6))
        story.append(Image(str(chart_pass), width=160 * mm, height=78 * mm))
    if chart_hal.exists():
        story.append(Image(str(chart_hal), width=160 * mm, height=78 * mm))

    derived = report.get("derived") or {}
    if derived.get("mixer_headroom_x"):
        story.append(Paragraph("1.1 Mixer simulation headroom @ 250 Hz", st["h2"]))
        h_rows = [
            [
                cell("Profile", st["small"]),
                cell("Upstream ×", st["small"]),
                cell("EdgeTX32 ×", st["small"]),
            ]
        ]
        for prof, sides in derived["mixer_headroom_x"].items():
            h_rows.append(
                [
                    cell(prof, st["small"]),
                    cell(str(sides.get("upstream")), st["small"]),
                    cell(str(sides.get("esp32")), st["small"]),
                ]
            )
        story.append(table(h_rows, [40 * mm, 60 * mm, 60 * mm]))
        if derived.get("scheduler_miss_pct"):
            story.append(
                Paragraph(
                    f"Scheduler deadline-miss model: upstream "
                    f"{derived['scheduler_miss_pct']['upstream']}% · "
                    f"EdgeTX32 {derived['scheduler_miss_pct']['esp32']}% "
                    f"(1-core UI interference vs dual-core pin).",
                    st["body"],
                )
            )

    # Verdict box
    story.append(Paragraph("Verdict", st["h2"]))
    story.append(
        Paragraph(
            "<b>Upstream EdgeTX</b> production radio yazılımı olarak olgun: TX15 benzeri "
            "hedeflerde donanım sürücüleri ve simu yolu kanıtlanmış.<br/>"
            "<b>EdgeTX-ESP32</b> portable çekirdeği koruyarak ESP32-S3 HAL + ESP-IDF iskeletini "
            "getiriyor; host hwgen/HAL smoke testleri geçiyor, WiFi/BLE/dual-core avantajları var. "
            "Tam radyo UI + protokol runtime’ı için ESP-IDF ile cihaz bring-up şart.",
            st["body"],
        )
    )

    # Feature matrix
    story.append(Paragraph("2. Feature matrix (çalışır / destek)", st["h1"]))
    feat_rows = [[cell("Feature", st["small"]), cell("Upstream", st["small"]), cell("ESP32 fork", st["small"]), cell("Notes", st["small"])]]
    for f in report["features"]:
        feat_rows.append(
            [
                cell(f["feature"], st["small"]),
                cell(f["upstream"], st["small"]),
                cell(f["esp32"], st["small"]),
                cell(f.get("notes") or "", st["small"]),
            ]
        )
    story.append(table(feat_rows, [40 * mm, 45 * mm, 45 * mm, 45 * mm]))

    # HAL coverage
    story.append(Paragraph("3. HAL coverage checklist", st["h1"]))
    story.append(Paragraph("Upstream: rm-h750 / generic_stm32 eşlenikleri · ESP32: targets/esp32s3", st["body"]))
    keys = sorted(report["hal_coverage"]["esp32s3"].keys())
    hal_rows = [[cell("HAL piece", st["small"]), cell("Upstream", st["small"]), cell("ESP32", st["small"])]]
    for k in keys:
        u = "OK" if report["hal_coverage"]["upstream_tx15_like"].get(k) else "MISSING"
        e = "OK" if report["hal_coverage"]["esp32s3"].get(k) else "MISSING"
        hal_rows.append([cell(k, st["small"]), cell(u, st["ok"] if u == "OK" else st["fail"]), cell(e, st["ok"] if e == "OK" else st["fail"])])
    story.append(table(hal_rows, [70 * mm, 50 * mm, 50 * mm]))

    # Checks
    story.append(Paragraph("4. Simulation / automated checks", st["h1"]))
    chk_rows = [[cell("Check", st["small"]), cell("Side", st["small"]), cell("Result", st["small"]), cell("Time ms", st["small"]), cell("Detail", st["small"])]]
    for c in report["checks"]:
        status = "PASS" if c["ok"] else "FAIL"
        sty = st["ok"] if c["ok"] else st["fail"]
        detail = (c.get("detail") or "").replace("<", "&lt;")[:180]
        chk_rows.append(
            [
                cell(c["name"], st["small"]),
                cell(c["side"], st["small"]),
                cell(status, sty),
                cell(f'{c["ms"]:.1f}', st["small"]),
                cell(detail, st["small"]),
            ]
        )
    story.append(table(chk_rows, [45 * mm, 20 * mm, 15 * mm, 18 * mm, 77 * mm]))

    # Benchmarks / simulations
    story.append(PageBreak())
    story.append(Paragraph("5. Simulations & host benchmarks", st["h1"]))
    story.append(
        Paragraph(
            "Python simulations exercise mixer (light/medium/heavy), logical switches, CRSF CRC, "
            "ADC EMA, UI+mixer concurrency, and a scheduler jitter model. Equal ops/s across sides "
            "is expected on the same host CPU — they validate algorithmic parity. Dual-core / jitter "
            "rows illustrate architecture, not chip MHz. Optional C++ benches appear when a compiler is present.",
            st["body"],
        )
    )
    b_rows = [[cell("Bench", st["small"]), cell("Side", st["small"]), cell("Iters", st["small"]), cell("Seconds", st["small"]), cell("Ops/s", st["small"]), cell("Notes", st["small"])]]
    for b in report["benchmarks"]:
        b_rows.append(
            [
                cell(b["name"], st["small"]),
                cell(b["side"], st["small"]),
                cell(str(b["iterations"]), st["small"]),
                cell(f'{b["elapsed_s"]:.6f}' if b["elapsed_s"] >= 0 else "n/a", st["small"]),
                cell(f'{b["ops_per_s"]:,.0f}' if b["ops_per_s"] else "n/a", st["small"]),
                cell(b.get("notes") or "", st["small"]),
            ]
        )
    story.append(table(b_rows, [32 * mm, 18 * mm, 22 * mm, 25 * mm, 28 * mm, 50 * mm]))
    for ch in (chart_mixer, chart_jitter, chart_thru, chart_loc):
        if ch.exists():
            story.append(Spacer(1, 4))
            story.append(Image(str(ch), width=160 * mm, height=72 * mm))

    hw = report.get("hw_defs") or {}
    if hw:
        story.append(Paragraph("5.2 Hardware JSON complexity", st["h2"]))
        hw_rows = [[cell("Board", st["small"]), cell("ADC inputs", st["small"]), cell("Keys", st["small"]), cell("Switches", st["small"])]]
        for label, key in (("Upstream TX15", "upstream_tx15"), ("EdgeTX32 ESP32-S3", "esp32s3")):
            d = hw.get(key) or {}
            hw_rows.append(
                [
                    cell(label, st["small"]),
                    cell(str(d.get("adc_inputs", "—")), st["small"]),
                    cell(str(d.get("keys", "—")), st["small"]),
                    cell(str(d.get("switches", "—")), st["small"]),
                ]
            )
        story.append(table(hw_rows, [55 * mm, 40 * mm, 40 * mm, 40 * mm]))

    story.append(Paragraph("5.1 Theoretical MCU / platform performance", st["h2"]))
    t_rows = [[cell("Metric", st["small"]), cell("Upstream TX15-class", st["small"]), cell("ESP32-S3", st["small"]), cell("Edge", st["small"])]]
    for row in report["theoretical_perf"]:
        t_rows.append(
            [
                cell(row["metric"], st["small"]),
                cell(row["upstream_tx15"], st["small"]),
                cell(row["esp32s3"], st["small"]),
                cell(row["winner"], st["small"]),
            ]
        )
    story.append(table(t_rows, [32 * mm, 50 * mm, 50 * mm, 43 * mm]))

    # Runtime readiness
    story.append(Paragraph("6. Runtime readiness / çalışıp çalışmama", st["h1"]))
    rr = report["runtime_readiness"]
    r_rows = [[cell("Item", st["small"]), cell("Upstream", st["small"]), cell("ESP32", st["small"])]]
    keys = sorted(set(list(rr["upstream"].keys()) + list(rr["esp32"].keys())))
    for k in keys:
        r_rows.append(
            [
                cell(k, st["small"]),
                cell(str(rr["upstream"].get(k, "—")), st["small"]),
                cell(str(rr["esp32"].get(k, "—")), st["small"]),
            ]
        )
    story.append(table(r_rows, [55 * mm, 60 * mm, 60 * mm]))

    story.append(Paragraph("6.1 Interpretation", st["h2"]))
    story.append(
        Paragraph(
            "• <b>PASS</b> hwgen: her iki tarafta JSON→C include üretimi sağlam (ESP32 şablonları dahil).<br/>"
            "• <b>ESP32 verify_hw_gen.sh</b>: fork’un jeneratör + host GPIO derlemesi yeşil.<br/>"
            "• <b>Upstream native CMake</b>: ortam bağımlılıklarına göre başarısız olabilir; kaynak/simu varlığı ayrı kontrol edildi.<br/>"
            "• <b>Ops/s eşitliği</b> mixer/ADC host bench’te beklenen sonuçtur — aynı binary algoritması.<br/>"
            "• Cihazda ESP32’nin gerçek avantajı dual-core ayrımı, PSRAM UI ve WiFi/BLE; H750’nin avantajı "
            "yüksek tek-çekirdek frekansı + olgun radyo PMIC/LCD/DMA yolu.",
            st["body"],
        )
    )

    # Stats appendix
    story.append(Paragraph("7. Repository statistics", st["h1"]))
    s_rows = [[cell("Stat", st["small"]), cell("Upstream", st["small"]), cell("ESP32 fork", st["small"])]]
    for key in ("tree_mb", "radio_cpp", "radio_h", "pulses_cpp", "telemetry_cpp"):
        s_rows.append(
            [
                cell(key, st["small"]),
                cell(str(report["sizes"]["upstream"][key]), st["small"]),
                cell(str(report["sizes"]["esp32"][key]), st["small"]),
            ]
        )
    s_rows.append(
        [
            cell("targets/", st["small"]),
            cell(", ".join(report["sizes"]["upstream"]["targets"][:12]) + ("…" if len(report["sizes"]["upstream"]["targets"]) > 12 else ""), st["small"]),
            cell(", ".join(report["sizes"]["esp32"]["targets"][:12]) + ("…" if len(report["sizes"]["esp32"]["targets"]) > 12 else ""), st["small"]),
        ]
    )
    story.append(table(s_rows, [40 * mm, 70 * mm, 65 * mm]))

    story.append(Spacer(1, 10))
    story.append(HRFlowable(width="100%", thickness=0.5, color=colors.grey))
    story.append(
        Paragraph(
            "Artifacts: reports/EdgeTX_vs_ESP32_Comparison_Report.pdf · "
            "reports/edgetx_esp32_compare.json",
            st["small"],
        )
    )

    doc.build(story)
    print(f"Wrote {out}")


def main():
    src = Path(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_JSON
    if not src.exists():
        print("JSON not found", src, file=sys.stderr)
        return 1
    report = json.loads(src.read_text())
    OUT_PDF.parent.mkdir(parents=True, exist_ok=True)
    build(report, OUT_PDF)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
