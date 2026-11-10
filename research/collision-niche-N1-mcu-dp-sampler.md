# Collision log: N1 — Soundness and cost of on-device DP-SGD noise samplers on a Cortex-M33

Date: 2026-11-10. Checker: collision-check subagent. Search tool: web search (US index), arXiv/ePrint abstract pages, one full-text HTML read.

## Candidate (as given)
Application: smart-meter IoT clients run federated DP-SGD on an EFR32MG24 (Cortex-M33) and upload the noisy float32 model update.
Asset: each client's raw readings (membership of a record in the local batch).
Adversary: honest-but-curious FL server that sees each client's exact float32 update (no secure aggregation); phase 2 adds a physical power/timing observer.
Claim: (a) float32 Box-Muller (newlib logf/cosf, ChaCha20 CSPRNG) is open to Mironov/Jin-style floating-point distinguishing, probably more so than float64; (b) the libm calls take data-dependent time; (c) sound alternatives (discrete Gaussian with constant-time Bernoulli, snapping, dithered Gaussian 2607.06320, binomial/centred binomial) cost a measurable share of a DP-SGD step on the M33.
Our existing number: float32 Box-Muller + ChaCha20 is about 66% of a DP-SGD step (16,608-parameter FCN, batch 4), measured by QEMU instruction counting. This is an ESTIMATE, not calibrated against hardware cycles.

## Queries run
1. `floating-point attack differential privacy Gaussian sampler float32 single precision` (technical)
2. `differential privacy noise on microcontroller is it really private sampler` (plain)
3. `eprint.iacr.org constant-time discrete Gaussian sampler Cortex-M differential privacy` (ePrint)
4. `DP-SGD embedded TinyML microcontroller floating point noise generation cost` (embedded/TinyML)
5. `"tiny bits matter" precision-based attacks differential privacy float32` (Tumult)
6. `Opacus secure_mode floating point attack Gaussian noise float32 torch`
7. `differentially private federated learning microcontroller Cortex-M on-device noise sampler timing side channel 2025` (extended, date-targeted)
8. `emergentmind 2605.21378 Apple differential privacy framework audit floating-point`
9. `single-precision float32 Gaussian noise DP-SGD floating-point vulnerability GPU attack distinguish` (extended, ML community)
10. `secure differential privacy noise generation IoT device low-power hardware floating-point Laplace Gaussian 2026` (extended, embedded/hardware, date-targeted)
11. `"Evaluation of DP-SGD Impact on TinyML-Optimized Models"`
12. `float32 floating-point attack differential privacy 2025 2026 arXiv lower precision more vulnerable` (extended, date-targeted)
13. `on-device differentially private training microcontroller federated learning DP-SGD Cortex-M4 Cortex-M33 noise overhead` (extended, embedded)

## Papers opened
| ID | Title / authors | Verified | Reading depth | Overlap with N1 |
|---|---|---|---|---|
| arXiv 2112.05307 (IEEE S&P 2022) | Jin et al., "Are We There Yet? Timing and Floating-Point Attacks on DP Systems" | Yes (arXiv HTML v4) | Full text, first ~100k of 128k chars, read via fetch-model questions (appendix not read) | HIGH on (a) and part of (b). Threat model is the same: an FL server collecting per-client noisy gradients; attacks Opacus DP-SGD (up to 92.56% success) and PyTorch Box-Muller. Timing attack on discrete Laplace/Gaussian samplers. All on double precision, Intel Xeon; float32 never discussed; no embedded, no libm timing; mitigation cost only for time padding (+10.5%). Paper note handled separately in research/papers/jin-fp-timing-dp-attacks.md (not written here). |
| arXiv 2605.21378 (reportedly IEEE S&P 2026) | Chourasia, Bao, Javaid, Xiao, "Auditing Apple's DifferentialPrivacy.framework" | Yes (arXiv abstract) | Abstract only | MEDIUM. Shows deployed client-side (on-device) floating-point samplers fail DP (5 of 9 mechanisms). macOS, local DP, not DP-SGD, not MCU; abstract does not state precision, timing or cost. Strengthens "known problem" framing. |
| arXiv 2207.13793 + Tumult blog "Tiny bits matter" | Haney, Desfontaines et al., precision-based attacks and interval refining | Yes (search snippets + blog listing) | Snippets only | MEDIUM. Precision attacks even on MPFR libraries; fix by interval refining. Not float32-specific, not embedded, no MCU cost. |
| arXiv 2607.06320 | Kalinin, Pagh, "Dithered Gaussian Mechanism for Randomness-Efficient DP" | Yes (arXiv abstract) | Abstract only | LOW-MEDIUM. Sound drop-in for DP-SGD with "modest overhead"; hardware for overhead not stated in abstract; no MCU, no timing. It is one of our candidate fixes, not a collision. |
| arXiv 2107.10138 (ESORICS 2021) | Holohan, Braghin, "Secure Random Sampling in DP" | Yes (arXiv abstract) | Abstract + search snippet | LOW. Fix that the search snippet says "can be extended to" single/half precision; no cost, no embedded. |
| arXiv 2608.27766 | Fu, Wang, "Revisiting Continuous Noise Sampling for Multi-Party DP" | Yes (arXiv abstract) | Abstract only | LOW. MPC setting (Orchard, DP-BREM+); sampler cost compared, not MCU. |
| ISCA 2018 / IEEE 8416855 | "Guaranteeing Local Differential Privacy on Ultra-Low-Power Systems" (DP-Box) | Partly (title/venue via search; PDF text not extractable, IEEE page empty) | Search-snippet level | MEDIUM on device class. Low-resolution fixed-point noise on ultra-low-power devices breaks LDP; hardware fix (resampling, thresholding). Local DP on sensor values, not DP-SGD, not float32 software libm. Must be cited. |
| SBSeg 2025, DOI 10.5753/sbseg.2025.11481 | da Silva et al., "Evaluation of DP-SGD Impact on TinyML-Optimized Models" | Yes (SBC page via search) | Snippet only | LOW. DP-SGD utility for TinyML models on Raspberry Pi 4; no sampler soundness, no MCU. |
| ePrint 2026/1610, 2018/1234 (FACCT), 2019/267 | Constant-time Gaussian samplers (Falcon / lattice), Cortex-M3 QEMU for 2026/1610 | Yes (ePrint listings via search) | Snippets only | LOW for DP, but useful as cost baselines for constant-time discrete Gaussian on Cortex-M. None target DP. |

Not found: any paper running a floating-point distinguishing attack on float32 Gaussian noise; any paper measuring newlib/libm logf/cosf timing variation as a DP side channel; any paper costing sound DP samplers on a Cortex-M MCU relative to an on-device DP-SGD step.

## Verdict: PARTIAL OVERLAP
Killed or mostly covered: the attack idea and the threat model. Jin et al. (arXiv 2112.05307) already attack per-client DP-SGD gradients seen by an FL server, including PyTorch Box-Muller, and already show timing leaks in discrete samplers. The Apple audit (2605.21378) shows on-device float samplers fail in deployment. DP-Box (ISCA 2018) already raised low-precision noise on ultra-low-power devices.
Open as far as searched:
1. A measured float32 version of the attack (how many distinct outputs float32 Box-Muller from a 23/24-bit uniform can produce, and the attack success rate against an M33 update), compared with float64.
2. Whether newlib-nano/Arm libm logf/cosf/sqrtf on the M33 FPU have input-dependent cycle counts that reveal the noise magnitude (phase 2 physical channel).
3. Cycle, memory and energy cost on the M33 of the sound options (constant-time discrete Gaussian, snapping, dithered Gaussian, centred binomial) as a fraction of one DP-SGD step, and the resulting utility at equal epsilon.

## KNOWN / CONJECTURE / ASSUMPTION
KNOWN (cited): float64 Gaussian samplers in PyTorch/Opacus are open to floating-point attacks against per-client DP-SGD updates (Jin et al. 2112.05307); discrete samplers leak through timing (same); deployed on-device float samplers break DP (2605.21378); precision attacks survive MPFR (2207.13793); low-resolution noise on ultra-low-power hardware breaks LDP (DP-Box, ISCA 2018); sound replacements exist (discrete Gaussian, snapping, interval refining, dithered Gaussian 2607.06320).
CONJECTURE (ours): float32 makes the attack cheaper and more accurate because far fewer outputs are reachable; newlib float libm has data-dependent timing on the M33; sound samplers cost a large share of a DP-SGD step on the M33.
ASSUMPTION: the server sees the exact float32 update with no secure aggregation and no added quantisation; clients use a float32 Box-Muller on a ChaCha20 stream; the QEMU instruction-count ratio (about 66%) tracks real M33 cycles (not calibrated yet, and QEMU does not model FPU or flash wait-state timing).

## Most damning reviewer objection
"This is a port of Jin et al. (S&P 2022) to a new platform: the same per-client FL threat model, the same attack, and fixes that are already known. The only new content is engineering numbers, and the headline 66% comes from QEMU instruction counts. QEMU cannot show the timing leak in claim (b) and cannot give real cycle or energy costs. Unless the float32 attack gives a clearly stronger, quantified break, and the M33 costs are measured on hardware, the contribution is incremental."

## Addendum (coordinator, 2026-11-11): DP-Box full text and the preliminary attack
- DP-Box (Choi, Tomei, Sanchez Vicarte, Hanumolu, Kumar, ISCA 2018), full text extracted from the PDF. Low-resolution
  fixed-point RNGs bound the Laplace noise range, giving infinite privacy loss in local DP. Fixed in hardware by resampling
  or thresholding. Collision for the *tail-cut concept* only; not float32, not Box-Muller/Gaussian, not DP-SGD/FL.
- The preliminary pair-inversion attack on the M33 build (see `direction-mining-niche-2026-11-10.md` §4) reaches 96%
  single-release accuracy. This is the float32 analogue of Jin et al.'s float64 Box-Muller attack, so the
  "port of Jin et al." objection stands. The new parts are the MCU code path, the exact accounting and the on-chip
  fix cost.
