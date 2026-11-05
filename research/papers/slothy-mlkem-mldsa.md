# Paper note: SLOTHY ML-KEM & ML-DSA Cortex-M4 → M7

## Verified identity (STEP 0 — VERIFIED, matches)
- **Title (exact):** "Enabling Microarchitectural Agility: Taking ML-KEM & ML-DSA from Cortex-M4 to M7 with SLOTHY"
- **Authors:** Amin Abdulrahman (Max Planck Institute for Security and Privacy, MPI-SP, Bochum, Germany); Matthias J. Kannwischer (Quantum Safe Migration Center, Chelpis Quantum Corp, Taipei, Taiwan); Thing-Han Lim (same, Chelpis/QSMC Taipei).
- **ePrint ID:** IACR ePrint 2025/366 — CONFIRMED. The candidate's two framings ("Taking ML-KEM & ML-DSA…" and "Enabling Microarchitectural Agility…") are the same paper; the full title has both parts.
- **Venue:** Published at **ACM AsiaCCS 2025** (ASIA CCS '25, Aug 25–29, 2025, Hanoi, Vietnam). DOI 10.1145/3708821.3736210. CC-BY 4.0.
  - NOTE: the task brief said "ACM/TCHES 2025". That is **not** correct — the venue is **AsiaCCS 2025**, not TCHES. (The authors have *separate* TCHES papers that this one cites, e.g. ref [2] TCHES 2024, but this paper itself is AsiaCCS.)
- **Year:** 2025.

## Core claim
KNOWN (from paper): Heavily hand-optimized Cortex-M4 PQC assembly (ML-KEM, ML-DSA, Keccak) can be automatically re-optimized for the dual-issue Cortex-M7 using an extended version of the SLOTHY constraint-programming superoptimizer (which does instruction scheduling, register allocation, and software pipelining), instead of re-hand-tuning. The result is the first open-source ML-KEM/ML-DSA implementation specifically tuned for Cortex-M7, with full-scheme speed-ups of **11–33%** over pqm4's M4 code running on the M7.

## Platform & what was ACTUALLY measured
- **Real board, not emulator-only.** Benchmarks on a **Nucleo-F767ZI** with **STM32F767ZI** (Arm Cortex-M7, Armv7E-M), 2048 KiB flash, 512 KiB SRAM of which 128 KiB is tightly-coupled DTCM; max 216 MHz. Code was run from DTCM, **clocked at 24 MHz** to eliminate flash wait states. Methodology: run in a loop 100×, repeat 100×, report **median** cycle counts. A QEMU Cortex-M7 (mps2-an500) target was added too, but only for functional CI tests, not for the reported cycle numbers.
- **M4 reference board:** STM32F407-DISCOVERY (STM32F407VG, Cortex-M4) with default pqm4 config, for the "Before M4" columns.
- **Toolchain:** Arm GNU toolchain **arm-none-eabi-gcc version 13.3.Rel1**. Specific -O flags not stated in text (inherited from pqm4 defaults). Baseline pqm4 commit 49ce5bea (as of Jan 16, 2025). Starting Keccak etc. from ref [4].
- **Parameter sets:** Component tables cover ML-DSA NTT at both q=8380417 and the smaller 257/769 arithmetic variants, and ML-KEM (n=256, q=3329, i.e. 769 arithmetic). **Full-scheme results are reported only for NIST security category 3: ML-DSA-65 and ML-KEM-768.** ML-KEM-512/1024 and ML-DSA-44/87 full numbers are stated to be in Appendix C (Tables 5 and 6) — NOT found in the pages I read; the main tables show only ML-DSA-65 and ML-KEM-768.

### Concrete numbers as stated (cycle counts = clock cycles, cc)
**Keccak-f1600 permutation (Table 1), After-M7:** best = **5149 cc @ 1.77 IPC** (from the M4 [4] code, 1.30× speed-up); the ref [17] variant gives 5376 cc @ 1.85 IPC (1.16×); the hand-opt M7 [4] gives 5322 cc @ 1.87 IPC (1.05×). So SLOTHY's best is ~5% faster than the prior hand-optimized M7 Keccak.

**ML-DSA components (Table 2), After-M7 (q=8380417 unless noted):**
- NTT: 8139 → **4141 cc** (1.87 IPC), 1.97×
- iNTT: 8207 → **4547 cc**, 1.80×
- Basemul: 2080 → 1063 cc, 1.96×; Basemul Acc.: 2166 → 1235 cc, 1.75×
- Pointmul 769: 1053 → 448 cc, **2.35×** (largest; low IPC of original explains >2× gain)
- Pointmul 257: 1311 → 612 cc, 2.14×
- Barrett reduction: 989 → 833 cc, 1.19×; caddq: 668 → 636 cc, 1.05×

**ML-KEM components (Table 3), After-M7:**
- NTT: 4011 → **2380 cc** (1.78 IPC), 1.69×
- iNTT: 4247 → **2586 cc**, 1.64×
- Barrett reduction: 1213 → 717 cc, 1.69×
- basemul 32 16: 2207 → 1059 cc, 2.08×

**Full-scheme, NIST category 3, Cortex-M7, "Ours" vs pqm4 (Table 4), mean of 5000 runs:**
- **ML-DSA-65 (speed/m4fspeed):** Ours KeyGen **1,446,421 cc** / Sign **3,412,174 cc** / Verify **1,380,098 cc**. pqm4 is ×1.25 / ×1.32 / ×1.27 slower respectively → so "Ours" is ~20–33% faster than pqm4-on-M7 for sign. (vs pqm4* which uses M7 Keccak: ×1.10 / ×1.19 / ×1.12.)
- **ML-KEM-768 (speed/m4fspeed):** Ours KeyGen **372,064 cc** / Encaps **385,371 cc** / Decaps **417,224 cc**. pqm4 is ×1.24 / ×1.22 / ×1.23 slower. (vs pqm4*: ×1.11 / ×1.09 / ×1.11.)
- Stack-optimized (m4fstack) variants are also reported, e.g. ML-KEM-768 Ours KeyGen 371,048 / Encaps 386,586 / Decaps 423,908 cc.

### Stack / memory usage
- **Stack usage in bytes: NOT FOUND as a table.** The paper distinguishes "speed-optimized" (m4fspeed) vs "stack-optimized" (m4fstack) implementations and gives cycle counts for each, but I did not find explicit stack-byte figures in the pages read. What IS reported is **code size in bytes** per routine (Tables 1–3), e.g. ML-DSA NTT code size grows 860 → 1704 bytes after optimization (code size roughly doubles due to software pipelining / 16→32-bit encodings). So: code size yes, RAM/stack bytes not found.

## Threat model
**N/A — this is a performance paper.** There is no cryptographic adversary or confidentiality asset. The only security discussion (Secs 3.5, 4.4) is **constant-time preservation**: SLOTHY relies on constant-time *input* and does not add branches or secret-dependent memory accesses, so timing side-channel resistance is preserved for its outputs. The authors explicitly note that **power/EM side-channels are out of scope** — microarchitectural scheduling/register choices could affect leakage, but modeling that is left as possible future SLOTHY work.

## Convenient assumptions (what the result quietly depends on)
- Cycle counts are taken at **24 MHz from DTCM** to avoid flash wait states — i.e. an idealized memory setting; real firmware running from flash at 216 MHz would see different absolute timing (wait states, caching). Numbers are comparative, not end-application latency.
- The Cortex-M7 microarchitectural model was **reverse-engineered** ("Arm does not provide any documentation on this matter, we base our model on our own experiments as well as on 3rd party reverse engineering" [18, 27]). Optimality claims are relative to this model's accuracy.
- Starts from **existing pqm4 hand-optimized assembly** as input; the whole premise assumes a good M4 baseline already exists.
- Full-scheme wins are measured **only for category-3 params** (ML-DSA-65, ML-KEM-768); other levels assumed to behave similarly (Appendix).
- Constant-time safety of custom splitting/fusion replacement patterns is asserted ("All replacement patterns provided in this work fulfill this property") rather than formally proven here.

## Stated limitations & future work
Two loop-handling limitations remain (address-offset-fixup needs manual annotation; the loop counter must be the first `cmp` argument). Side-channel coverage is limited to timing. One short quote: *"a possible solution could involve modeling a device's leakage characteristics as part of SLOTHY's microarchitectural model and incorporating this information in the construction of the optimization problem."*

## Load-bearing references (3–5)
- **[2]** Abdulrahman, Becker, Kannwischer, Klein, *Fast and Clean: Auditable high-performance assembly via constraint solving*, TCHES 2024 — the original **SLOTHY** tool this paper extends.
- **[4]** Alexandre Adomnicai, *An update on Keccak performance on ARMv7-M*, ePrint 2023/773 — source of the M4/M7 Keccak code they optimize and compare against.
- **[30]/[29]** **pqm4** library (and pqm4-fork "pqm7" at github.com/mupq/pqm7) — the benchmarking framework and baseline M4 assembly for all schemes.
- **[3]** Abdulrahman, Hwang, Kannwischer, Sprenkels, *Faster Kyber and Dilithium on the Cortex-M4*, ACNS 2022 — the state-of-the-art M4 NTT/basemul implementations migrated here.
- **[1]** pqmx benchmarking framework (github.com/slothy-optimizer/pqmx) — component-level benchmark harness used for Tables 1–3.

## Relevance to our project
KNOWN + MY INFERENCE (kept separate): This paper gives rigorous, real-hardware **Cortex-M7** cycle counts for ML-KEM-768 and ML-DSA-65, not Cortex-M33. Our target (EFR32xG24) is **Cortex-M33 / Armv8-M**, which this paper explicitly lists as a *different* microarchitecture (it notes M33/M35P implement Armv8-M and that running M4 code on them "may not perform well") — so these M7 numbers are **not directly reusable as M33 estimates**. INFERENCE: their value to us is (a) a clean, well-documented methodology template (24 MHz-from-DTCM, median of 100×100, GNU gcc 13.3, pqm4 harness) that we can mirror when we calibrate our own cost estimates; (b) solid order-of-magnitude anchors — ML-KEM-768 full ops are ~0.37–0.42 M cycles and ML-DSA-65 sign ~3.4 M cycles on a dual-issue M7, which bracket what a single-issue M33 would cost (likely slower, since M33 lacks the M7's dual-issue pipeline that drives most of these speed-ups). For a PQC-on-IoT direction, this is a strong *baseline/calibration reference and methodology source*, but we would still need actual M33 numbers (from pqm4-on-M33 or the EFR32 board) — the gap "no SLOTHY/clean-assembly M33 tuning for ML-KEM/ML-DSA" is a plausible seed but must be checked against literature before claiming it open.

## Possible seeds (direction-miner moves)
- **Port-the-platform:** redo this M4→M7 study for M4→**M33/Armv8-M** (our EFR32xG24); the paper itself flags M33 as untested.
- **Add-a-cost-axis:** they measure cycles + code size but *not* stack/RAM or energy; an IoT-framed note could add stack-byte and energy measurements on a constrained board.
- **Side-channel follow-up:** their explicit "model leakage inside SLOTHY" future-work line is exactly our phase-2 (side-channel analysis of our own implementation).
- **Calibration anchor:** reuse their 24 MHz/DTCM/median methodology to make our emulator→MCU estimates credible.

## Reading depth
**FULL-READ of the ePrint PDF** (author-hosted copy at kannwischer.eu, identical to ePrint 2025/366). Read pages 1–12 including all four results tables (Tables 1–4), intro, preliminaries, SLOTHY extensions, case study, results, conclusion, and references. NOT read: Appendix C (Tables 5–6) with the non-category-3 parameter sets and Appendix A (butterfly figures) — those pages were beyond what I fetched, so ML-KEM-512/1024 and ML-DSA-44/87 full numbers are marked "not found" rather than invented.

---
Sources:
- https://eprint.iacr.org/2025/366
- https://kannwischer.eu/papers/2025_slothy-m7.pdf (full PDF, read)
- https://dl.acm.org/doi/10.1145/3708821.3736210 (AsiaCCS 2025 venue/DOI)
