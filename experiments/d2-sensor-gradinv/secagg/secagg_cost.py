#!/usr/bin/env python3
"""secagg_cost.py [x25519_cycles]: per-round client cost of secure aggregation on top of D2's DP-SGD, on the EFR32MG24.
Every input is labelled. Nothing here is run on hardware; it combines MEASURED QEMU instruction counts with
KNOWN datasheet values and ASSUMPTION parameters. Masking follows Bonawitz et al. (CCS 2017): one pairwise mask per
peer plus one self mask (double masking), each a PRG expansion of the whole quantised update."""
import math, sys

D = 16_608                  # parameters (D2 FCN)                                      KNOWN (our model)
CHACHA_INSN_PER_BLOCK = 1226  # ChaCha20 64-B block on the M33 (QEMU)                    MEASURED (n1 sampler.c)
DP_STEP_INSN = 5.63e6       # one DP-SGD step, B = 4 (QEMU)                              MEASURED (d2 dpstep.c)
PLAIN_STEP_INSN = 1.16e6    #                                                          MEASURED
STEPS = 5
CPI = (1.0, 2.0)            # cycles per instruction, uncalibrated bracket               ASSUMPTION
NJ_CYC = (0.100, 0.147)     # nJ per cycle (datasheet Table 4.5 x 3.0 V)                 KNOWN-derived
CLK = 78e6                  # Hz                                                         KNOWN (Table 4.2)
UJ_TX_BYTE = 0.50           # µJ per on-air byte at 0 dBm (Table 4.6, 250 kbit/s)        KNOWN-derived
FRAME_PAY, FRAME_OVH = 102, 31   # 802.15.4 payload / overhead per frame                 ASSUMPTION
AES_SE_US_PER_KB = 474      # AES-128-CTR via SE Manager, 1 kB                           KNOWN (datasheet Table 4.53)
SE_POWER_MW = (33.3e-6 * 78 * 3.0 * 1e3, 49.1e-6 * 78 * 3.0 * 1e3)  # assume core-active power during SE ops  ASSUMPTION
X25519_CYC = float(sys.argv[1]) if len(sys.argv) > 1 else 411_061   # Cortex-M33 (DSP), Lenngren ePrint 2025/523   KNOWN (abstract only)
MASK_PER_ELEM_INSN = 3      # load + add + store per element per mask                     ASSUMPTION

def mj(insn):    return tuple(insn * c * e * 1e-6 for c, e in zip(CPI, NJ_CYC))   # mJ (lo, hi)
def radio_mj(nbytes): f = math.ceil(nbytes / FRAME_PAY); return (nbytes + f * FRAME_OVH) * UJ_TX_BYTE / 1e3, f

def secagg_plus_k(n):       # neighbours in SecAgg+ (Bell et al. CCS 2020): k = O(log n). Constant 2 chosen  ASSUMPTION
    return min(n - 1, math.ceil(2 * math.log2(n)))

dp_round = DP_STEP_INSN * STEPS
print(f"DP-SGD per round (5 steps): {dp_round/1e6:.1f} M instr = {mj(dp_round)[0]:.2f}-{mj(dp_round)[1]:.2f} mJ  [MEASURED instr x ASSUMPTION/KNOWN energy]")
for bits in (32, 16):
    nbytes = D * bits // 8; up_mj, frames = radio_mj(nbytes)
    blocks = math.ceil(nbytes / 64); mask_insn = blocks * CHACHA_INSN_PER_BLOCK + D * MASK_PER_ELEM_INSN
    aes_ms = nbytes / 1024 * AES_SE_US_PER_KB / 1e3
    print(f"\n### Update quantised to {bits}-bit integers: {nbytes:,} B, {frames} frames, radio upload {up_mj:.1f} mJ [KNOWN-derived]")
    print(f"One mask expansion: ChaCha20 SW {mask_insn/1e6:.2f} M instr ({mask_insn/CLK*1e3:.1f}-{2*mask_insn/CLK*1e3:.1f} ms, "
          f"{mj(mask_insn)[0]:.3f}-{mj(mask_insn)[1]:.3f} mJ) [ESTIMATE]; AES-CTR on SE {aes_ms:.1f} ms, "
          f"{aes_ms*SE_POWER_MW[0]/1e3:.3f}-{aes_ms*SE_POWER_MW[1]/1e3:.3f} mJ [KNOWN time, ASSUMPTION power]")
    print("| N | scheme | masks/round | mask compute (M instr) | mask energy mJ | x DP round | key agreements | X25519 mJ | radio upload mJ |")
    print("|---|---|---|---|---|---|---|---|---|")
    for n in (10, 40, 100):
        for name, peers in (("full pairwise (Bonawitz)", n - 1), ("SecAgg+ (k=2log2N)", secagg_plus_k(n))):
            masks = peers + 1; ins = masks * mask_insn; e = mj(ins)
            nx = 2 * peers + 2   # 2 shared secrets per peer (encryption + mask keys) + 2 key generations   KNOWN protocol (Bonawitz)
            xs = f"{nx * X25519_CYC * NJ_CYC[0] * 1e-6:.2f}-{nx * X25519_CYC * NJ_CYC[1] * 1e-6:.2f} ({nx * X25519_CYC / 1e6:.1f} M cyc)"
            print(f"| {n} | {name} | {masks} | {ins/1e6:.1f} | {e[0]:.2f}-{e[1]:.2f} | {ins/dp_round:.2f} | {2*peers+2} X25519 | {xs} | {up_mj:.1f} |")
