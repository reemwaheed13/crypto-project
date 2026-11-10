#!/usr/bin/env python3
"""analyze.py <build dir>: turns QEMU plugin + harness logs into the D3 kill-experiment tables (markdown on stdout).

Measured: instruction counts (QEMU TCG plugin), stack (painting), heap (malloc wrapper), code size (arm-none-eabi-size).
Everything else is a labelled model: cycles = instructions x CPI (ASSUMPTION, uncalibrated bracket), energy from the
EFR32MG24 datasheet constants recorded in research/direction-mining-2026-11-05.md ("Target device")."""
import math, os, re, sys

B = sys.argv[1]
CPI = (1.0, 2.0)            # ASSUMPTION: Cortex-M33 cycles/instruction bracket, NOT calibrated (no published M33/M4 point run)
NJ_PER_CYC = (0.100, 0.147)  # derived from datasheet Table 4.5 (33.3 / 49.1 uA/MHz x 3.0 V)
UJ_PER_TX_BYTE = 0.50        # derived: 5.2 mA x 3.0 V x 32 us (Table 4.6, 0 dBm, 250 kbps)
FRAME_PAYLOAD, FRAME_OVERHEAD = 102, 31  # ASSUMPTION: 802.15.4 127-B PSDU, 25 B MAC hdr+FCS (worst case) + 6 B PHY

# SoK ePrint 2025/669 Table 4 (p. 20), client cycles per block on a Xeon, as recorded in research/papers/sok-transciphering.md.
# Our build -> their row. FiLIP: the SoK row is "FiLIP-DSM"; both framework FiLIPs are DSM instances (mapping unverified).
XEON = {"aes_ctr_sw": ("AES-SW", 1.86e3), "hera_5": ("HERA-5", 2.86e4), "kreyvium_13": ("Kreyvium-13", 5.27e4),
        "pasta2_4": ("Pastav2-4", 1.92e5), "pasta_4": ("Pasta-4", 4.70e5), "dasta_6_packed": ("Dasta-6", 7.76e5),
        "filip_1280": ("FiLIP-DSM", 2.56e7), "rasta_5_packed": ("Rasta-5", 1.81e8)}

def read(p):
    try: return open(p).read()
    except OSError: return ""

def windows(d):
    out = []
    for line in read(f"{d}/run.insn").splitlines():
        f = line.split(); w = {"total": int(f[3])}
        for i in range(4, len(f) - 1, 2): w[f[i]] = int(f[i + 1])
        out.append(w)
    return out

def elf_text(d):
    m = re.search(r"^\s*(\d+)\s+\d+\s+\d+", read(f"{d}/elf_size.txt").split("\n", 1)[-1], re.M)
    return int(m.group(1)) if m else None

rows = []
for line in read(f"{B}/ciphers.txt").splitlines():
    name, kind, _ = line.split()
    d, log = f"{B}/{name}", read(f"{B}/{name}/run.log")
    ok = "DONE fails 0" in log
    m = re.search(r"exit=(\d+)", log); rc = m.group(1) if m else "none"
    w = windows(d)
    wl = [l for l in log.splitlines() if l.startswith("WIN")]
    if not ok or len(w) < 4:
        rows.append(dict(name=name, kind=kind, ok=False, note=f"exit={rc}, KAT {'pass' if ok else 'fail/not reached'}")); continue
    if kind == "zp":
        t = int(re.search(r" t (\d+) ", log).group(1)); bits = int(re.search(r" bits (\d+) ", log).group(1))
        pay = 2 * t; wire = math.ceil(t * bits / 8)       # ASSUMPTION: 16-bit readings, ciphertext packed at 17 bit/elt
        blk = f"t={t}, p=65537"
    else:
        pb = int(re.search(r"plain_bits (\d+)", log).group(1)); pay = wire = pb / 8; blk = f"{pb} bit"
    one = [x["total"] for x in w[:3]]; four = w[3]
    # cost(nb blocks in one call) = fix + marg * nb, fitted on the 1-block and 4-block windows. If the fit gives a negative
    # fixed part (per-block cost varies with the counter, e.g. Agrasta/Rasta rejection-sample invertible matrices),
    # fall back to fix = 0, marg = 4-block mean.
    marg = (four["total"] - one[0]) / 3; fix = one[0] - marg
    if fix < 0: fix, marg = 0.0, four["total"] / 4
    per = one[0]
    st = max(int(re.search(r"stack (\d+)", l).group(1)) for l in wl)
    hp = max(int(re.search(r"heap (\d+)", l).group(1)) for l in wl)
    kec = sum(v for k, v in four.items() if k.startswith("Keccak")); div = four.get("__udivmoddi4", 0) + four.get("__aeabi_uldivmod", 0)
    rows.append(dict(name=name, kind=kind, ok=True, blk=blk, pay=pay, wire=wire, one=one, per=per, per4=four["total"] / 4, fix=fix, marg=marg, ratio4=four["total"] / one[0],
                     kec=kec / four["total"], div=div / four["total"], st=st, hp=hp, text=elf_text(d)))

# symmetric baselines: per-size windows
sym = {}
for n in ("ascon_armv7m", "ascon_opt32", "aes_ctr_sw"):
    d = f"{B}/sym/{n}"; log = read(f"{d}/run.log"); w = windows(d)
    sizes = [int(re.search(r"bytes (\d+)", l).group(1)) for l in log.splitlines() if l.startswith("WIN")]
    ov = 16 if n.startswith("ascon") else 0
    if "DONE fails 0" in log and w:
        pts = sorted({s: x["total"] for s, x in zip(sizes, w)}.items())
        sym[n] = dict(pts=pts, ov=ov, text=elf_text(d),
                      st=max(int(re.search(r"stack (\d+)", l).group(1)) for l in log.splitlines() if l.startswith("WIN")))

def sym_insn(n, L):  # piecewise-linear in payload length, extrapolated from the last segment
    p = sym[n]["pts"]
    for (a, ia), (b, ib) in zip(p, p[1:]):
        if L <= b or (b == p[-1][0]): return ia + (ib - ia) * (L - a) / (b - a)

def frames(wire): n = math.ceil(wire / FRAME_PAYLOAD); return n, wire + n * FRAME_OVERHEAD

def energy(insn, wire):  # (compute_lo, compute_hi, radio) in uJ
    nf, onair = frames(wire)
    return insn * CPI[0] * NJ_PER_CYC[0] / 1e3, insn * CPI[1] * NJ_PER_CYC[1] / 1e3, onair * UJ_PER_TX_BYTE

good = sorted([r for r in rows if r["ok"]], key=lambda r: r["marg"] / r["pay"])
print("## Table 1. Client encryption cost on Cortex-M33 (QEMU mps2-an505), reference code, -O3\n")
print("| Cipher (framework dir) | Block | Payload B/block | Wire B/block | Instr, 1 block (3 reps) | Instr, 4 blocks in one call | Fitted fixed + marginal/block | Marginal instr per payload byte | Keccak share (4-blk) | 64-bit div share (4-blk) | Peak stack B | Peak heap B | ELF .text B |")
print("|---|---|---|---|---|---|---|---|---|---|---|---|---|")
for r in good:
    reps = "/".join(f"{x:,}" for x in r["one"])
    print(f"| {r['name']} | {r['blk']} | {r['pay']:g} | {r['wire']:g} | {reps} | {4*r['per4']:,.0f} | {r['fix']:,.0f} + {r['marg']:,.0f} | {r['marg']/r['pay']:,.0f} | "
          f"{100*r['kec']:.0f}% | {100*r['div']:.0f}% | {r['st']:,} | {r['hp']:,} | {r['text']:,} |")
for n, s in sym.items():
    L, I = s["pts"][1]
    (a, ia), (b, ib) = s["pts"][0], s["pts"][-1]; mg = (ib - ia) / (b - a)
    print(f"| **{n}** (baseline) | {L} B frame | {L} | {L + s['ov']} | {I:,} | — | {ia - mg * a:,.0f} + {mg:,.0f}/B | {mg:,.0f} | — | — | ≤{s['st']} | 0 | {s['text']:,} |")
print("\nNot run / not valid:")
for r in rows:
    if not r["ok"]: print(f"- {r['name']}: {r['note']}")

print("\n## Table 2. M33 ranking vs Xeon ranking (SoK 2025/669 Table 4), latency of one block (single call)\n")
pairs = []
for n, (lab, xc) in XEON.items():
    if n == "aes_ctr_sw":
        m = sym.get(n, {}).get("pts", [(0, None)])[0][1]
    else:
        r = next((r for r in good if r["name"] == n), None); m = r["per"] if r else None
    if m: pairs.append((lab, n, xc, m))
def ranks(v): s = sorted(range(len(v)), key=lambda i: v[i]); r = [0] * len(v); [r.__setitem__(i, k + 1) for k, i in enumerate(s)]; return r
rx, rm = ranks([p[2] for p in pairs]), ranks([p[3] for p in pairs])
print("| SoK row | Our build | Xeon cycles/block (SoK) | M33 instr/block (ours) | M33 instr ÷ Xeon cycles | Xeon rank | M33 rank |")
print("|---|---|---|---|---|---|---|")
for (lab, n, xc, m), a, b in sorted(zip(pairs, rx, rm), key=lambda z: z[1]):
    print(f"| {lab} | {n} | {xc:.3g} | {m:.3g} | {m/xc:.1f} | {a} | {b} |")
k = len(pairs); rho = 1 - 6 * sum((a - b) ** 2 for a, b in zip(rx, rm)) / (k * (k * k - 1)) if k > 1 else float("nan")
print(f"\nSpearman rank correlation over {k} shared settings: **ρ = {rho:.3f}**")

print("\n## Table 3. Energy for one sensor upload (model; compute bracket = CPI 1.0–2.0 × 0.100–0.147 nJ/cycle; radio 0 dBm)\n")
print("| Payload B | Scheme | Blocks | Wire B | 802.15.4 frames | Compute µJ (lo–hi) | Radio µJ | Total µJ (lo–hi) | Compute ÷ radio (lo–hi) | Total ÷ Ascon (lo–hi) |")
print("|---|---|---|---|---|---|---|---|---|---|")
for L in (16, 64, 256, 1024):
    lines = []
    for n in ("ascon_armv7m", "aes_ctr_sw"):
        if n in sym: lines.append((n, 1, L + sym[n]["ov"], sym_insn(n, L)))
    for r in good:
        nb = math.ceil(L / r["pay"]); lines.append((r["name"], nb, nb * r["wire"], r["fix"] + nb * r["marg"]))
    a = energy(sym_insn("ascon_armv7m", L), L + 16); alo, ahi = a[0] + a[2], a[1] + a[2]
    for n, nb, wire, insn in lines:
        clo, chi, rad = energy(insn, wire); nf, _ = frames(wire)
        print(f"| {L} | {n} | {nb} | {wire:,.0f} | {nf} | {clo:,.1f}–{chi:,.1f} | {rad:,.1f} | {clo+rad:,.1f}–{chi+rad:,.1f} | "
              f"{clo/rad:.2g}–{chi/rad:.2g} | {(clo+rad)/alo:.3g}–{(chi+rad)/ahi:.3g} |")
