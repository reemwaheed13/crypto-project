#!/usr/bin/env python3
"""A4 kill experiment: spreadsheet model of PQ-signed windows of periodic CAN FD frames (no crypto code).
Writes model.csv and prints markdown tables. Every input is labelled KNOWN (cited) or ASSUMPTION.

Scheme per window (most favourable to A4, so a kill here is conclusive): the sender buffers N periodic 64-B frames,
signs H(frames), then sends the signature in ceil(sig/64) extra CAN FD frames. The receiver keeps all N frames and
recomputes the hash itself, so there are no per-frame Merkle paths (they would only add bus load). Hashing cost is
ignored (it is far below signing cost).
Worst-case authentication latency of the FIRST frame of a window:
    L(N) = (N-1)*P + t_sign + n_sig*T_frame/(1-U_bg) + t_verify
The 1/(1-U_bg) factor is a crude M/D/1-style inflation for background traffic (ASSUMPTION; a real CAN schedule
analysis would replace it). Feasible N also needs signer throughput: t_sign <= N*P (one signer, no backlog)."""
import csv, math, os

# KNOWN: pqm4 benchmarks.md @ commit 0dd10ee5 (Cortex-M4, NUCLEO-L4R5ZI per pqm4), cycles.
# ePrint 2026/1915 row: abstract only (average sign), verify assumed equal to FN-DSA m4f.
# Signature sizes: FIPS 204 (ML-DSA-44 2,420 B), Falcon spec padded format (FN-DSA-512 666 B), FIPS 205 (SLH-DSA 128f 17,088 B).
SCHEMES = [
    ("ML-DSA-44 (pqm4 m4f)",             3_943_121, 17_009_165, 1_421_623, 2420),
    ("FN-DSA-512 (pqm4 m4f)",           22_469_685, 22_594_542,   396_949,  666),
    ("FN-DSA-512 low-RAM (2026/1915)*", 13_450_000, None,         396_949,  666),
    ("SLH-DSA-SHA2-128f (pqm4 clean)", 368_575_228, 368_575_246, 21_923_628, 17088),
]
CLOCKS = [80e6, 240e6]        # ASSUMPTION: ECU-class Cortex-M at 80 MHz (EFR32-like) and 240 MHz (high-end M7-class);
                              # M4 cycle counts used unchanged (M33/M7 not calibrated)
DATA_RATES = [2e6, 5e6]       # ASSUMPTION: CAN FD 500 kbit/s arbitration, 2 or 5 Mbit/s data phase
NOMINAL = 500e3
U_BG = [0.4, 0.8]             # KNOWN: background-load range used by app16083705 (Applied Sciences 16(8):3705)
PERIODS = [1e-3, 10e-3, 100e-3]  # KNOWN range from the A4 card (periodic frames, 1-100 ms)
DEADLINES = [10e-3, 50e-3, 100e-3]  # 10 ms is the card's claim; 50/100 ms show where batching starts to work

def t_frame(rd):
    """One 64-B CAN FD frame, 11-bit ID, worst-case bit stuffing (ASSUMPTION: field widths per ISO 11898-1:2015).
    Nominal-rate bits: SOF..BRS 17 (+4 stuff), CRC delimiter 1, ACK 2, EOF 7, IFS 3 -> 34.
    Data-rate bits: ESI 1 + DLC 4 + 512 data (+129 worst-case stuff) + stuff count 4 + CRC-21 21 (+7 fixed stuff) -> 678."""
    return 34 / NOMINAL + 678 / rd

rows = []
for name, cs, cs_max, cv, sig in SCHEMES:
    nsig = math.ceil(sig / 64)
    for f in CLOCKS:
        ts, tv = cs / f, cv / f
        ts_max = cs_max / f if cs_max else None
        for rd in DATA_RATES:
            tf = t_frame(rd)
            for u in U_BG:
                tx = nsig * tf / (1 - u)
                for P in PERIODS:
                    lmin = ts + tx + tv
                    for D in DEADLINES:
                        # largest N with L(N) <= D, and smallest N with t_sign <= N*P
                        nmax = math.floor((D - ts - tx - tv) / P) + 1 if D >= lmin else 0
                        nmin = max(1, math.ceil(ts / P))
                        feasible = nmax >= nmin
                        n = nmax if feasible else None
                        extra = nsig * tf / (n * P) if n else None
                        rows.append(dict(scheme=name, clock_MHz=f / 1e6, data_Mbps=rd / 1e6, bg_load=u, period_ms=P * 1e3,
                                         deadline_ms=D * 1e3, sign_ms=round(ts * 1e3, 2),
                                         sign_worst_ms=round(ts_max * 1e3, 2) if ts_max else "",
                                         verify_ms=round(tv * 1e3, 2), sig_frames=nsig, sig_tx_ms=round(tx * 1e3, 2),
                                         min_latency_ms=round(lmin * 1e3, 2), N_min_throughput=nmin,
                                         N_max_deadline=nmax, feasible=feasible, N_star=n or "",
                                         extra_bus_load_pct=round(100 * extra, 2) if extra else ""))

here = os.path.dirname(os.path.abspath(__file__))
with open(os.path.join(here, "model.csv"), "w", newline="") as fh:
    w = csv.DictWriter(fh, fieldnames=list(rows[0])); w.writeheader(); w.writerows(rows)

print("## Table A. Fastest possible authentication (N = 1, any period) vs the 10 ms deadline\n")
print("| Scheme | Clock | Sign ms (avg / worst seen) | Verify ms | Sig frames | Sig on bus ms (2 Mb/s, 40% bg → 5 Mb/s, 40%) | Min latency ms (best case: 5 Mb/s, 40% bg) | Meets 10 ms? |")
print("|---|---|---|---|---|---|---|---|")
for name, cs, cs_max, cv, sig in SCHEMES:
    for f in CLOCKS:
        r2 = next(r for r in rows if r["scheme"] == name and r["clock_MHz"] == f / 1e6 and r["data_Mbps"] == 2 and r["bg_load"] == 0.4)
        r5 = next(r for r in rows if r["scheme"] == name and r["clock_MHz"] == f / 1e6 and r["data_Mbps"] == 5 and r["bg_load"] == 0.4)
        print(f"| {name} | {f/1e6:.0f} MHz | {r5['sign_ms']} / {r5['sign_worst_ms'] or 'n/a'} | {r5['verify_ms']} | {r5['sig_frames']} | "
              f"{r2['sig_tx_ms']} → {r5['sig_tx_ms']} | {r5['min_latency_ms']} | {'yes' if r5['min_latency_ms'] <= 10 else 'no'} |")

print("\n## Table B. Largest window N* meeting the deadline, and its extra bus load (5 Mb/s data phase, 40% background)\n")
print("| Scheme | Clock | Period ms | N* @ 10 ms | N* @ 50 ms (extra load) | N* @ 100 ms (extra load) |")
print("|---|---|---|---|---|---|")
def cell(r): return f"{r['N_star']} ({r['extra_bus_load_pct']}%)" if r["feasible"] else "none"
for name, *_ in SCHEMES:
    for f in CLOCKS:
        for P in PERIODS:
            sel = {r["deadline_ms"]: r for r in rows if r["scheme"] == name and r["clock_MHz"] == f / 1e6 and r["data_Mbps"] == 5
                   and r["bg_load"] == 0.4 and r["period_ms"] == P * 1e3}
            print(f"| {name} | {f/1e6:.0f} MHz | {P*1e3:g} | {cell(sel[10.0])} | {cell(sel[50.0])} | {cell(sel[100.0])} |")
print(f"\nOne 64-B CAN FD frame: {t_frame(2e6)*1e6:.0f} µs at 2 Mb/s, {t_frame(5e6)*1e6:.0f} µs at 5 Mb/s (worst-case stuffing).")
n_ok = sum(1 for r in rows if r["deadline_ms"] == 10.0 and r["feasible"])
print(f"Configurations meeting the 10 ms deadline: {n_ok} of {sum(1 for r in rows if r['deadline_ms'] == 10.0)}.")
