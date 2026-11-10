# Niche direction mining: randomness and sampling on microcontrollers (2026-11-10)

**Bottom line.**
- **Four candidates; only N1 is viable.** All four sit at the randomness and sampling corner of PQC, FHE and FL privacy
  on a Cortex-M. N1 (is on-device DP-SGD noise actually DP, and what does a sound sampler cost on the chip?) is the only
  one that survives a harsh review.
- **The preliminary run on our QEMU M33 harness found one strong effect and one weak one.**
  - **Strong: a floating-point pair-inversion attack.** It runs on the M33 build itself, with the same newlib float32
    code. In the worst case it decides membership from a single released Box-Muller pair with 96% accuracy at both ε
    settings, where an ideal Gaussian allows 73–87%. The released pair is never reproducible under the wrong
    hypothesis in 93% of canary-present trials, which makes the effective δ about 0.9.
  - **Weak: the 5.77σ tail cut.** Its exact effect is modest. It raises δ from 10⁻⁵ to 4.8×10⁻⁴ at ε = 8 and adds
    about 0.1 to ε at ε = 1, and DP-Box (ISCA 2018) already published the concept.
- **Pessimistic scores: N1 ties D2 at 26/30.** Its most damning objection, "Jin et al. (S&P 2022) ported to a new
  device", stands: the pair attack is theirs in float64. The new parts are float32 on a real MCU code path, the exact
  accounting, the instruction-path leak and the on-chip cost of fixing it.
- **N1 is the missing half of D2, not a rival.** D2 already measured that the noise sampler is 66% of an on-chip DP-SGD
  step. N1 shows that the same sampler is unsound, and that the sound version costs about 8–17× more. That is the
  "cost" story D2 lacked, plus a genuine crypto-engineering side-channel phase 2.
- **Recommendation:** stay with D2, and fold N1 in as D2's noise-sampler work package.

Labels: **KNOWN** (cited, reading depth given), **CONJECTURE** (ours), **ASSUMPTION** (modelling choice), **MEASURED**
(QEMU instruction counts: uncalibrated estimates, not cycles). "Open" always means *open as far as searched*.

## 1. Mining table

| Paper | Status | Reading depth | Seed | Move | Candidate |
|---|---|---|---|---|---|
| Jin et al., "Are We There Yet? Timing and Floating-Point Attacks on DP Systems", arXiv 2112.05307 | IEEE S&P 2022 | Full text (HTML v4 + ar5iv appendices); figures not seen | Float attacks on Box-Muller/Opacus DP-SGD, timing on discrete samplers; all float64, Xeon | Move to device; change precision | **N1** |
| Holohan & Braghin, "Secure Random Sampling in DP", arXiv 2107.10138 | ESORICS 2021 | Full text (HTML v2) | Box-Muller inversion attack; fix sums several samples; Python on PC; no constant-time | Move to device | N1 |
| Kalinin & Pagh, "Dithered Gaussian Mechanism", arXiv 2607.06320 | arXiv only (v3, Oct 2026) | Full HTML v1 + v3, some tables cut | Discrete output defeats FP attacks; 3–7% overhead on GPU; float64; not additive (no SecAgg) | Move to device | N1 (rival fix) |
| Fu & Wang, "Revisiting Continuous Noise Sampling for Multi-Party DP", arXiv 2608.27766 | arXiv only (Aug 2026) | Full HTML via summarising fetch | Sample-then-scale fixed-point noise has sparse support, ~100% distinguishing; MPC servers only | Move to the client | N1, N2 |
| Silva, dos Santos, Bezerra, DP-SGD on TinyML models, SBSeg 2025 (DOI 10.5753/sbseg.2025.11481) | Published short paper | Abstract + references (body streams undecoded) | DP-SGD trained off-device; Raspberry Pi 4 inference only | none (not a collision as far as read) | — |
| Aydin et al., RevEAL, ePrint 2022/204 | DATE 2022 | Full text | SEAL v3.2 Gaussian sampler branch leak, PicoRV32 on FPGA | Move to device | N4 |

Notes are in `research/papers/`: `jin-fp-timing-dp-attacks.md`, `secure-random-sampling-dp.md`,
`dithered-gaussian-mechanism.md`, `multiparty-dp-noise-sampling.md`, `dpsgd-tinyml-sbseg.md`,
`reveal-seal-sampler-sca.md`.

## 2. Candidates, collision verdicts and pessimistic scores

Each candidate gets one collision log, `research/collision-niche-N*.md`, which lists every query. Scores are
pessimistic: novelty / threat model / falsifiability / feasibility / DRP / group split, each out of 5.

| | Candidate | Collision verdict | Most damning objection (first) | Score /30 |
|---|---|---|---|---|
| **N1** | On-device DP-SGD noise on a Cortex-M33: is it still DP, and what does a sound, constant-time sampler cost? | **Partial overlap.** Jin et al. hold the threat model and the float64 Box-Muller attack. DP-Box (ISCA 2018, full text) holds the "finite-resolution RNG bounds the noise, so infinite loss" concept for fixed-point local DP. Open: float32 MCU code path and its exact accounting, libm instruction-path leak, on-chip cost of sound samplers | "Jin et al. ported to a new device; the tail cut is DP-Box's point; QEMU instruction counts are not timing; the break is shown for a single-step (FedSGD) release, not D2's FedAvg." | 3 / 4 / 5 / 5 / 5 / 4 = **26** (scored on the pair attack; the cap alone would make novelty 2, total 25) |
| N2 | Reuse ML-KEM's centred binomial / Falcon's CT sampler as the DP noise for secure aggregation | Partial overlap: binomial DP for SecAgg is standard (cpSGD; ePrint 2017/643; PBM arXiv 2207.09916); Stevens et al. USENIX'22 reuse DP noise as LWE error | PQC samplers have σ ≈ 1–1.8, DP needs σ in the tens to hundreds, so "reuse" saves only Keccak, which every sampler calls anyway | 2 / 3 / 4 / 4 / 3 / 3 = **19** |
| N3 | Secure randomness as the bottleneck for PQC/FHE/DP on Cortex-M | Partial overlap: masked-Kyber random-byte tables (arXiv 2311.08040, Tables 7–8); FHE seeds a fast PRG | "A benchmark note with no threat model; the TRNG only seeds a DRBG by design" | 2 / 1 / 3 / 5 / 2 / 3 = **16** |
| N4 | Side-channel leakage of the embedded FHE client's sampler (SEAL-Embedded, Zeppelin) | Partial overlap, close to collision: SEAL-Embedded's samplers are branch-free; single-trace CBD/NTT attacks on Cortex-M4 exist (ePrint 2023/1128; TCHES 2025 ×2; ePrint 2026/2137) | "The samplers are already branch-free, the known leaks are published, and an emulator leakage test is not an attack" | 2 / 4 / 4 / 2 / 5 / 4 = **21** |
| **D2** (current) | On-device cost of defeating gradient inversion on smart-meter FL | Partial overlap (round 3) | "The cost half may be thin" (now answered: the DP step is 4.85× the plain step on the M33, MEASURED) | 3 / 5 / 5 / 4 / 4 / 5 = **26** |

D2's score is from 2026-11-10. Its kill experiment is still running, and its re-score is in `decision-summary.md`.

## 3. N1 direction card

**Application.** Smart-meter or air-quality sensors (EFR32MG24, Cortex-M33, single-precision FPU) train a shared
forecaster with DP-SGD on the device. They upload noisy updates, exactly the D2 setting.

**Threat model.**
- Asset: membership of a reading window in a client's batch, i.e. the DP guarantee itself.
- Adversary 1: an honest-but-curious FL server that sees each client's exact float32 update. This is the
  floating-point / support attack, the same model as Jin et al.
- Adversary 2, phase 2: a physical observer of the sensor's power or timing, who sees the sampler's execution path.
- Out of scope: malicious servers and secure aggregation (N1 is about the noise, not the masking).

**Claim (falsifiable).** The sampler typically used on MCUs, float32 Box-Muller on newlib `logf`/`cosf`/`sinf` fed by a
CSPRNG, has three problems:
- (a) Released pairs can be checked against the sampler's finite image. A Mironov / Holohan–Braghin pair inversion
  separates neighbouring datasets almost surely when each step's noise is released. The hard 5.77σ tail cut adds a
  smaller, exactly computable δ inflation.
- (b) An execution path that depends on the noise.
- (c) Every sound replacement costs a multiple of the current DP-SGD step on the M33. The constant-time discrete
  Gaussian is the most expensive.

**Plain-language explanation.**
- A computer cannot draw a truly continuous random number. It draws a fixed number of random bits and turns them into
  a value, so only certain values can ever come out.
- On a microcontroller, numbers are single precision (24 bits), so far fewer values are possible than on a PC. One
  consequence is easy to see: the biggest noise value the sampler can ever produce is about 5.8 standard deviations.
- Worse, only certain *pairs* of values can come out, so a guess about the data can be checked against that list (the main attack, §4).
- Differential privacy also assumes the noise *can* be larger. Suppose a server sees a value that would need more than
  5.8σ of noise under one guess about the data. It then knows that guess is wrong, with certainty.
- Separately, the maths routines take different paths for different inputs. Someone watching how long the routine
  runs, or its power trace, learns something about the noise.
- The fixes are known in principle: integer ("discrete") noise drawn in constant time. Nobody has priced them on a
  chip where noise is already most of the work.

**Status of evidence.**
- KNOWN: float64 attacks on Box-Muller and Opacus DP-SGD (Jin et al.); Box-Muller invertibility (Holohan–Braghin);
  sample-then-scale sparse support (Fu–Wang); timing leaks in discrete samplers on a Xeon (Jin et al.).
- MEASURED (this round): see §4.
- CONJECTURE: the float32 floating-point-hole attack is stronger than the float64 one; the angle-dependent path is
  visible in single power traces on the EFR32.
- ASSUMPTION: instruction-path variation is the dominant timing source on the in-order M33. FP divide/sqrt latencies
  and flash wait states are not modelled.

> **Kill test run 2026-11-11 (`n1-kill-2026-11-11.md`): the headline does NOT transfer to FedAvg.** After summing 5
> local steps (noise-only sum, and the real 5-step D2 update), the best attack only matches the ideal-Gaussian bound
> (LR test on the Gaussian-optimal line; the low-bit floating-point test is at chance). The pair-inversion break holds
> for single-step / per-step releases only (96%, reproduced). N1 re-scored **24** (novelty 2). It is kept inside D2 as
> hardening plus a phase-2 side-channel target (instruction-path leak), not as a break.

**Kill experiment (week 1).** The single-step pair attack is already done (§4). Week 1 tests whether it survives D2's
real upload: the update w0 − w5 after 5 local steps, with the lr/B scaling and float32 roundings, and the canary in only
some steps.
- If the attacker's accuracy falls to the ideal-Gaussian bound, (a) shrinks to "FedSGD-only".
- If the full-exponent + discrete-output fix costs less than 2× the current noise, (c) is dead.

**Measurements.**
- Distinguishing success: float32 vs float64, FedSGD vs FedAvg.
- Per-sample instruction paths, and mutual information with the noise.
- Instructions, RAM, flash and random bytes per sample for each sampler: Box-Muller, full-exponent Box-Muller,
  snapping, dithered Gaussian, and constant-time CDT/convolution discrete Gaussian.
- Energy per DP-SGD step and per FL round, including the radio.

**Semester plan.**
1. Float32 attack replication plus tail-cut accounting.
2. Sampler zoo on the harness.
3. Cost-vs-privacy table at equal ε, and the accuracy check on D2's forecaster.
4. Write-up.

**DRP phase 2.** Port the samplers to the EFR32MG24 kit. Measure real cycles and energy. Run a power/timing
side-channel attack on Box-Muller's angle path, and test the constant-time CDT's resistance. This is the instructor's
"side channels on our own implementation" hint, applied to a crypto-engineering object.

**Group split.** Attack and DP accounting / sampler implementations / MCU cost model and harness / evaluation and
writing.

## 4. Preliminary results (N1, run 2026-11-10, about 1 hour)

Code: `experiments/n1-mcu-dp-sampler/` (`sampler.c` sampler costs and paths, `attack.c` pair-inversion attack, `pld_trunc.py`
exact accounting of the cap, `build.sh`, `analyze.py`, `winsn_many.c`, a variant of the D3 plugin that records one window per sample). Setup: QEMU 8.2.2 `mps2-an505`, `arm-none-eabi-gcc 13.2.1 -O3`, newlib libm. The
randomness is pre-generated so that each window times only the transform. ChaCha20 is timed separately.

| Quantity | Result | Label |
|---|---|---|
| **Pair-inversion attack** (`attack.c`, on the M33 build). Worst-case DP adversary, canary (+C) on one coordinate of a Box-Muller pair, single-step release y = fl(G + fl(σC·a)), 2,000 trials per setting | Membership accuracy from **one** released pair: **96.6% (σ = 0.451, ε = 8 setting)** and **96.0% (σ = 0.798, ε = 1 setting)**. An ideal Gaussian at the same σ allows at most 86.6% / 73.4%. The true hypothesis reproduces y exactly in 2,000/2,000 trials. The wrong one reproduces it in only 6.7% / 8.0% | MEASURED (QEMU, bit-exact softfloat + newlib) |
| What that means for (ε, δ) | In about 93% of canary-present releases, y never occurs without the canary (0 of 2,000), an infinite-loss event. Over 750 steps at q = 4/1209, effective δ ≈ 1 − (1 − 0.93q)⁷⁵⁰ ≈ **0.9** at both settings (claimed 10⁻⁵) | Arithmetic on MEASURED rates. The "never" is empirical, not proven, because the search windows are heuristic |
| Same attack on the full-exponent sampler | 94.8% / 95.0%: **no help** | MEASURED |
| Attack cost | About 45–140 k M33 instructions per trial (two hypothesis tests) | MEASURED |
| Largest noise the baseline sampler can emit (forced all-zero random words) | **5.768σ** (float64 Box-Muller: 8.57σ) | MEASURED; matches √(48 ln 2) |
| **Exact effect of that cap alone** (`pld_trunc.py`): privacy-loss distribution of the truncated, Poisson-subsampled Gaussian, worst-case 1-D neighbour, T = 750, bracketed by rounding losses up and down | **ε = 8 setting: δ(8) = 4.8×10⁻⁴** (48× the claimed 10⁻⁵), and no finite ε reaches δ = 10⁻⁵, because the infinite-loss atom alone is 4.8×10⁻⁴. **ε = 1 setting: δ(1) = 1.1–2.0×10⁻⁵** (vs 0.5–1.5×10⁻⁵ untruncated), and ε at δ = 10⁻⁵ is 1.05–1.2 (vs 0.95–1.1). Sanity check: untruncated reproduces opacus PRV (ε 7.95–8.10 and 0.95–1.10) | Computed. **Plainly: the cap alone is modest at ε = 8 and nearly negligible at ε = 1.** The noise's own tail mass beyond 5.77σ is only ~8×10⁻⁹. What matters is the neighbour's shifted mass q·P(N > 5.77 − 1/σ). My first estimate (0.14, "14,000×") left out the subsampling rate q and was wrong |
| Same with 5 local steps summed (D2's FedAvg) | Both effects are untested. The pair test needs per-step pairs, and the cap region needs all five noises near the cap | CONJECTURE: much weaker, not computed |
| Box-Muller execution paths | 34 distinct instruction counts per pair (181–331); correlation with the pair's angle +0.52, with its magnitude +0.06 | MEASURED, 2,000 pairs |
| Information leaked by the instruction count | **1.22 of 2 bits** of the two noise signs per pair. Both signs guessed right **64.6%** of the time (chance 25%). The full-exponent fix does not change it (65.0%) | MEASURED (in-sample estimate, 34 bins, slightly optimistic) |
| Constant-time CDT discrete Gaussian | **Constant** (1 distinct count over 1,000 samples): 194 / 698 / 2,714 instructions per sample at σ₀ = 2 / 8 / 32; variances check out (3.7 / 69 / 1,043 vs 4 / 64 / 1,024) | MEASURED |
| ChaCha20 CSPRNG | 1,226 instructions per 64-B block = 76.6 per 32-bit word | MEASURED |
| Noise cost per DP-SGD step (16,608 coordinates) | Baseline Box-Muller: **3.70 M** instructions (agrees with D2's directly measured 3.72 M). Full-exponent fix: 5.65 M (+53%). CT-CDT with convolution to σ_int ≈ 461 (grid C·2⁻¹⁰, ε = 8): **≈ 46–98 M**, which is 8–17× D2's whole DP step and 40–85× the plain training step | Baseline/fix MEASURED; CDT per-step figure an estimate (convolution parameters not checked against smoothing bounds) |
| Energy per FL round (5 steps), CT-CDT noise | ≈ 23–68 mJ compute (CPI 1–2 × 0.100–0.147 nJ/cycle), against about 43 mJ to radio the 66 kB update at 0 dBm | Uncalibrated estimate |

**Reading the numbers (plain words).**
- **The real break is the floating-point one.** The sensor builds each pair of noise values from two 24-bit random
  numbers, so only about 2⁴⁸ pairs can ever come out, out of about 2⁶⁰ possible float32 pairs.
  - A server that knows everything except whether one record was used can subtract its guess and ask: could the chip
    have produced exactly this pair? For the right guess the answer is always yes. For the wrong guess it is almost
    always no, so the guess is settled.
  - This works in the setting where each step's noisy gradient leaves the device (FedSGD, the setting both Jin et al.
    and TS-Inverse use). Summing several local steps first (D2's FedAvg) is untested and probably much harder.
- **The tail cut is real but small.** The noise can never exceed 5.77σ, but outcomes that only the larger noise could
  explain are rare. The exact accounting shows δ growing 48× at ε = 8 and barely changing at ε = 1. DP-Box made the
  same point for fixed-point local DP in 2018.
- **Box-Muller's execution path depends on the noise.** It reveals the sign pattern of the noise most of the time.
  Remote servers cannot see that, but a power or timing probe could, which makes it the natural phase-2 target.
- **Constant-time integer noise works but is expensive.** The cost scales with σ₀, so a sound sampler would make noise
  generation roughly an order of magnitude more expensive than training itself. That is exactly the "privacy costs the
  sensor" result D2 is looking for.

## 5. Dropped candidates and what killed them

- **N2:** binomial noise for secure aggregation is standard (cpSGD; Dahl–Pastro ePrint 2017/643; Poisson Binomial
  Mechanism arXiv 2207.09916). The variance mismatch makes PQC-sampler reuse pointless. At most it is a line item in D4.
- **N3:** masked-PQC randomness volumes are tabulated (Kundu–Karmakar–Verbauwhede arXiv 2311.08040). FHE and DP seed fast
  PRGs, so it has no threat model.
- **N4:** SEAL-Embedded's CBD and ternary samplers are branch-free (source `device/lib/sample.c`). Single-trace attacks
  on Cortex-M4 noise samplers are already published (Aydin–Aysu ePrint 2023/1128; Peng–Kuhn TCHES 2025; Fouque et al.
  TCHES 2025; ePrint 2026/2137). It would need real hardware this semester.

## 6. Verification log

- **Queries:** the 12 harvest queries are listed here, and each collision log lists its own (N1: 13, N2: 15, N3: 20, N4: 14).
  Harvest queries: "floating-point attack differential privacy Gaussian noise sampler implementation 2025 2026"; "timing
  side channel discrete Gaussian sampler differential privacy constant-time"; "distributed discrete Gaussian secure
  aggregation federated learning implementation cost microcontroller"; "differential privacy noise generation
  microcontroller Cortex-M energy random number generator federated learning"; "eprint 2025 2026 randomness cost ML-KEM
  ML-DSA Cortex-M TRNG SHAKE bottleneck"; "FHE client encryption error sampling discrete Gaussian embedded side channel
  SEAL-Embedded sampler attack"; "floating-point attack DP-SGD gradient noise single precision float32 differential
  privacy vulnerability"; "SEAL-Embedded side-channel attack sampler microcontroller eprint 2022/1563 2025/867"; "TinyML
  on-device differential privacy noise sampler embedded secure implementation 2026"; "discrete Gaussian sampler Cortex-M
  cycles constant-time Falcon FN-DSA sampler benchmark differential privacy reuse"; "secure aggregation federated learning
  modular integer noise skellam mechanism client cost embedded"; "newlib logf sinf timing data-dependent Cortex-M
  constant time math library side channel".
- **Unchecked:**
  - **Checked:** DP-Box. Choi, Tomei, Sanchez Vicarte, Hanumolu, Kumar, "Guaranteeing Local Differential Privacy on
    Ultra-low-power Systems", ISCA 2018 (full text, extracted from the PDF with a stdlib script). It shows that low-
    resolution fixed-point RNGs bound the Laplace noise range, which gives infinite privacy loss for local DP, and fixes
    this in hardware with resampling or thresholding. It does not cover float32, Box-Muller/Gaussian, DP-SGD, FL or the
    pair attack. Verdict: a **collision for the tail-cut concept only**, which is why N1 is scored on the pair attack.
  - The Apple DifferentialPrivacy.framework audit (arXiv 2605.21378) was read at abstract level.
  - The SBSeg 2025 body text was not decoded.
  - The "92.56%" figure appears only in Jin et al.'s abstract. Do not cite it as a body result.
- **Not modelled:**
  - Real M33 cycle timing (flash wait states, FP divide/sqrt latency), so the leak is an instruction-path proxy.
  - The lr/B scaling and w0 subtraction before upload. These are extra roundings the attacker would also have to
    invert.
  - D2's FedAvg multi-step case.
  - A proof that the attack's search is complete. Its "impossible" verdicts are empirical (0 false negatives in
    8,000 true-hypothesis tests).

## 7. Recommendation

**Stay with D2, and absorb N1 as its noise-sampler work package.** N1 ties D2 on the harsh rubric and depends on D2's
setting. What it adds is a crypto-engineering core: the noise D2 pays 66% of its step for is breakable by a float32 pair
inversion when updates are released per step, and the sound constant-time replacement costs about 8–17× more on the
chip. It also gives D2 a real side-channel phase 2 (the Box-Muller instruction path). The tail cut alone would not
justify any of this.
