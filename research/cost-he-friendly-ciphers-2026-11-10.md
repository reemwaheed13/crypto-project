# D3 kill experiment: client cost of HE-friendly ciphers on a Cortex-M33 (2026-11-10)

**Verdict: both D3 claims fail the kill test as worded.** By the rule agreed in `decision-summary.md` ("if both come out
flat, switch to D2"), the recommendation is to **switch to D2**. Two smaller findings survive and are listed at the end.
Labels: **MEASURED** (from the emulator or harness), **KNOWN** (cited), **CONJECTURE** (ours), **ASSUMPTION** (a
modelling choice). Emulator numbers are instruction counts, not cycle counts.

## What was run

- **Application and threat model:** unchanged from the D3 card (`direction-mining-2026-11-05.md`). A sensor encrypts its
  readings with an HE-friendly cipher so that an honest-but-curious cloud can transcipher and compute on them. We
  measure only the sensor's cost.
- **Code:** IAIK hybrid-HE-framework "plain" (client) implementations, commit `dafd7e7`. Baselines: ascon-c Ascon-AEAD128
  (`armv7m` and `opt32`, commit `446347f`), and tiny-AES-c AES-128-CTR (software, no tag, commit `2385675`). The SoK used
  the same tiny-AES-c.
- **Target:** `arm-none-eabi-gcc 13.2.1 -mcpu=cortex-m33 -O3`, run under QEMU 8.2.2 `mps2-an505` (Cortex-M33). Our own
  TCG plugin counts the instructions executed between two marker calls, and attributes them to Keccak and to the 64-bit
  division helper. Stack is measured by painting, heap by a malloc wrapper.
- **Correctness:** every number below comes from a build whose known-answer tests pass, for both encryption and
  decryption (`DONE fails 0`). The tested parameter set is p = 65537 only (17-bit). The 33-bit and 60-bit test vectors were
  skipped, because our 32-bit port cannot represent them (see *Porting changes*).
- **Coverage:** 16 cipher settings plus 3 baselines. Not covered:
  - LowMC, and the unpacked Agrasta/Rasta/Dasta builds: these need the external M4RI library, which was not ported. The
    packed builds of the same ciphers did run.
  - Dasta-5: its 4-block run exhausted the emulator's 4 MB of RAM (1.69 MB of heap per block).
  - Not in the framework: Trivium, FRAST, Masta-6, Rubato, Elisabeth, Transistor.
- **Scripts:** `experiments/d3-transciphering-client/mcu/` (`fetch_deps.sh`, `run_all.sh`, `build_sym.sh`, `analyze.py`).
  Raw logs and the full tables are in `mcu/results/2026-11-10/`.

## Claim (a′): does the Cortex-M33 ranking differ from the Xeon ranking? **No.**

Latency of encrypting one block, in a single call. Xeon column: SoK ePrint 2025/669, Table 4 (KNOWN, as recorded in
`papers/sok-transciphering.md`). M33 column: MEASURED instructions.

| SoK row | Our build | Xeon cycles/block | M33 instructions/block | M33 ÷ Xeon | Xeon rank | M33 rank |
|---|---|---|---|---|---|---|
| AES-SW | tiny-AES-c CTR (16 B) | 1.86×10³ | 3.65×10³ | 2.0 | 1 | 1 |
| HERA-5 | hera_5 | 2.86×10⁴ | 2.39×10⁵ | 8.3 | 2 | 3 |
| Kreyvium-13 | kreyvium_13 | 5.27×10⁴ | 1.62×10⁵ | 3.1 | 3 | 2 |
| Pastav2-4 | pasta2_4 | 1.92×10⁵ | 1.61×10⁶ | 8.4 | 4 | 4 |
| Pasta-4 | pasta_4 | 4.70×10⁵ | 3.84×10⁶ | 8.2 | 5 | 5 |
| Dasta-6 | dasta_6_packed | 7.76×10⁵ | 1.70×10⁷ | 21.9 | 6 | 6 |
| FiLIP-DSM | filip_1280 | 2.56×10⁷ | 4.48×10⁸ | 17.5 | 7 | 7 |
| Rasta-5 | rasta_5_packed | 1.81×10⁸ | 6.88×10⁸ | 3.8 | 8 | 8 |

**Spearman rank correlation: ρ = 0.976** over the 8 shared settings. The only change is one adjacent swap, HERA-5 and
Kreyvium-13. That swap is not robust (CONJECTURE): the two counts differ by 1.5×, and the CPI difference between bit
operations and multiply-heavy code is not calibrated.

**Robustness check (MEASURED, then arithmetic).** On the M33, the reference code for the F_p ciphers spends 47–66% of
its instructions in software 64-bit division (`__udivmoddi4`). That comes from its `uint64 % p`, which a 32-bit core
cannot do in hardware. Keccak takes only 4–24% (HERA-5: 61%). So the *bottleneck* differs from the "PRNG-dominated" story
expected for the Xeon. Even so, deleting every division instruction, a lower bound for any optimised reduction, leaves the
order unchanged: HERA-5 1.9×10⁵, Pastav2-4 6.6×10⁵, Pasta-4 2.0×10⁶, Dasta-6 1.7×10⁷. The ratio of M33 to Xeon cost
varies between 2× and 22× across ciphers, but never enough to reorder neighbours.

**Caveats:**
- The SoK's moduli for its Table 4 rows are not recorded in our notes. HERA-5's 511 cycles/byte implies a 28-bit modulus
  in the SoK, against 17-bit here.
- The FiLIP-DSM → `filip_1280` mapping is unverified. `filip_1216` costs 4× more and would not change the rank.
- The full Table 4 (23 rows) could not be re-read here (no PDF renderer), so only the 8 rows in our notes were compared.

## Claim (b): is there a payload crossover against an ordinary AEAD? **No, there is no crossover.** Every HE-friendly cipher costs more.

Energy per upload, MEASURED instructions × model. Model constants:
- Compute: instructions × CPI 1.0–2.0 (ASSUMPTION, uncalibrated) × 0.100–0.147 nJ/cycle.
- Radio: 0.50 µJ per byte on air at 0 dBm, counting 31 B of 802.15.4 overhead per 102-byte frame (ASSUMPTION).

Datasheet constants are from the *Target device* table in `direction-mining-2026-11-05.md`.

| Scheme | 64 B upload: total µJ | ×Ascon | 1024 B upload: total µJ | ×Ascon | Compute ÷ radio (1024 B) | Peak heap |
|---|---|---|---|---|---|---|
| Ascon-AEAD128 (armv7m) | 56–57 | 1 | 694–702 | 1 | 0.005–0.016 | 0 |
| AES-128-CTR (software) | 49–51 | 0.9 | 702–739 | 1.0 | 0.03–0.08 | 0 |
| Kreyvium (streaming, 46-bit blocks) | 71–111 | 1.3–2.0 | 801–1,026 | 1.2–1.5 | 0.17–0.5 | 63 B |
| HERA-5 | 97–188 | 1.7–3.3 | 1,464–2,917 | 2.1–4.2 | 1.0–3.1 | 4.8 kB |
| Pastav2-4 | 211–524 | 3.8–9.2 | 3,297–8,307 | 4.8–12 | 3.6–11 | 2.6 kB |
| Pasta-4 | 437–1,188 | 7.8–21 | 6,908–18,922 | 10–27 | 8.7–25 | 3.3 kB |
| Dasta-6 (packed) | 3,490–10,144 | 62–179 | 41,860–121,716 | 60–173 | 59–170 | **3.6 MB** |
| Rasta-5 (packed) | 68,840–202,296 | 1,230–3,560 | 1.0–3.1 ×10⁶ | 1,500–4,400 | 1,500–4,400 | **390 kB** |
| FiLIP-1280 | 0.36–1.05 ×10⁶ | 6,400–18,600 | 5.7–16.9 ×10⁶ | 8,300–24,000 | 8,400–25,000 | 16 kB |

The full table (16, 64, 256 and 1024 B, all 16 settings) is in `mcu/results/2026-11-10/tables.md`.

- **There is no crossover.** The HE-friendly ciphers cost more per byte than Ascon (36 instructions per byte, MEASURED),
  and both scale linearly with the payload. So the HE-friendly cipher costs more at *every* payload size, not only below
  some size. Claim (b) as worded assumed a crossing that cannot happen against an AEAD. Its gap ("privacy against the
  server costs the sensor X× more") is real but is a constant factor, not a crossover.
- **"Radio dominates" is inverted (MEASURED + model), but only for some ciphers.** At 0 dBm, compute is about equal to or
  larger than radio for HERA-5, and 10–25,000× larger for the rest. Kreyvium is the only HE-friendly cipher where the
  radio still dominates, and only for uploads of 256 B or more. At 10 dBm (1.84 µJ/B), HERA-5 drops back to compute < radio (about 0.3–0.9× for 1024 B).
- **RAM rules some out (MEASURED).** Two settings exceed the EFR32MG24's 256 kB RAM as written: Dasta-6 needs 3.6 MB of heap
  and Rasta-5 needs 390 kB. MASTA-4 needs 142 kB. The others need under 40 kB.
- **Time (arithmetic, CPI 1–2 at 78 MHz):**

  | Cipher | Time per block | Block size |
  |---|---|---|
  | Pasta-4 | 49–98 ms | 64 B |
  | FiLIP-1216 | 23–45 s | 8 B |
  | Rasta-5 | 9–18 s | 66 B |

## Plain-words summary

Making the sensor's data computable by the cloud costs the sensor between 1.2× (Kreyvium) and tens of thousands of times
(FiLIP) the energy of ordinary authenticated encryption. The penalty does not shrink for small or large messages. The
order of the ciphers on the microcontroller is the same as on a server CPU. That is despite a different bottleneck: here
it is the slow 64-bit division in the reference code, where on the server it was random-number generation. The two
things D3 hoped to find, a different ranking and a payload crossover, are not there.

## Calibration and sanity checks

- **No cycle calibration was possible.** We have no published Cortex-M cycle count for any code we ran. The ascon-c
  README has no Cortex-M rows, and the paper notes have none for these implementations. Cycles are therefore an
  *uncalibrated* CPI bracket of 1.0–2.0. The conclusions above are ratios of 10× or more, or rank orders, so they do not
  depend on that bracket. HERA-5 vs Kreyvium-13 and AES vs Ascon are the exceptions.
- **Repeats:** 3 repetitions per window. The spread is ≤0.1%. An emulator is deterministic, so this only
  checks that the measurement is stable, not that the timing is accurate.
- **Per-block cost depends on the block counter** for ciphers that sample random invertible matrices. For Agrasta, 4
  blocks cost 10.8× one block. The tables fit cost = fixed + marginal × blocks from the 1-block and 4-block windows, or use
  the 4-block mean when the fit fails. Kreyvium amortises its 1,152-round warm-up within one call (fixed 138,735
  instructions, then 991 per payload byte). That is why its streaming cost is low.
- **Parameters:** the KATs confirm each cipher's own parameter set. Only p = 65537 was exercised.

## Porting changes to the third-party code (all in our harness, none in the fetched sources)

1. `harness/port.h`, force-included in every build:
   - Byte-swap builtins for `htobe64` and friends.
   - `uint128_t := uint64_t`. This is exact for p < 2³². Larger moduli are skipped, and the KAT would catch an error.
2. Known-answer test stubs: the modulus is widened to `uint64_t`, because `size_t` is 32-bit on ARM.
3. **Upstream bug found:** in the `*_packed` ciphers, `encrypt(pt, n)` reads `n` as bytes, while `decrypt` reads it as
   bits. The framework's own tests only call `decrypt`, so this was never exercised. Our bench compensates
   (`-DENC_LEN_IS_BYTES`). Worth reporting upstream.
4. FiLIP: `-DOC_ENABLE_PORTABLE_AES` (there is no AES-NI on Cortex-M).
5. PASTA2: linked against full newlib, because a debug `std::cout` needs wide-char printf. This inflates its ELF size
   (464 kB) but not the instruction counts.

## What survives (CONJECTURE, for the group to weigh. Neither has been collision-checked)

- **Transciphering vs direct HE, with a reversed crossover.** Against *direct* BFV/CKKS client encryption, the crossover
  is real, and it runs the opposite way to the usual story. Zeppelin's seeded BFV ciphertext is 65,572 B (KNOWN, M55,
  full HTML read). On air at 0 dBm that is about 43 mJ per ciphertext. Our per-byte totals put the break-even upload at
  roughly 2–6 kB for Pasta-4, 15–30 kB for HERA-5 and 43–55 kB for Kreyvium. Above that, *direct* HE would be cheaper
  for the sensor, provided one ciphertext holds that much data. That needs Zeppelin's slot count and SEAL-Embedded's
  ciphertext size. Neither is in our notes; both come from abstracts or summaries only.
- **A first MCU feasibility table** (RAM, flash, time per block on a 256 kB part), plus the upstream encrypt-length bug.
  These make a useful workshop note, not a research contribution.

## Effect on the D3 score

Novelty drops from 3 to **2**: (a′) is falsified and (b) has no crossover. Total goes from 27 to **26**, which ties D2.
The pre-agreed rule says to switch to D2. The only reason to stay with D3 is the direct-HE reversal above, and that
would need its own collision check first.
