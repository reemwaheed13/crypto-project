# Collision check: C7, per-client cost of constant-time integer DP noise in distributed DP as a function of N (Cortex-M33)

Date: 2026-11-11
Checker: collision-check agent (round 4)

## Candidate (as given)
D2 smart meters (EFR32MG24) run DP-SGD with secure aggregation over integers mod 2^k, so they need integer noise
(distributed discrete Gaussian, Kairouz-Liu-Steinke ICML 2021, or Skellam) in place of float32 Box-Muller. In distributed DP each
client adds noise with std sigma/sqrt(N). A constant-time (CT) CDT sampler's cost grows linearly with the per-client sigma
(our QEMU data: 194/698/2,714 M33 instructions per sample at sigma0 = 2/8/32, about 85 instructions per unit sigma). So the per-client
sound-sampler cost falls as 1/sqrt(N). Claim: there is a crossover N above which the CT integer sampler costs less than the
insecure float sampler, and this crossover, together with the dropout noise deficit, can be quantified on the MCU.
- Application: FL / DP-SGD on smart-meter sensor clients with SecAgg (D2/N1 setting).
- Asset: raw meter readings and per-client updates. DP protects the aggregate, SecAgg the individual update.
- Adversary: honest-but-curious server, possibly colluding with some clients. Dropouts are benign. A timing observer comes in phase 2.
- Measured: M33 instructions/cycles per noised coordinate against N; the crossover N; the epsilon inflation under dropout.

## Queries run (verbatim)
1. [technical] distributed discrete Gaussian client cost constant time sampler per-client noise sigma/sqrt(n)
2. [technical/plain] discrete Gaussian sampling cost scales with sigma constant-time CDT table size linear sigma
3. [ePrint-focused] eprint.iacr.org constant-time discrete Gaussian sampler large sigma convolution base sampler cost
4. [embedded/IoT] distributed differential privacy microcontroller noise generation cost number of clients secure aggregation IoT
5. [technical] Skellam mechanism sampler constant time Poisson sampling timing side channel differential privacy
6. [plain/FL] dropout clients missing noise distributed differential privacy secure aggregation noise deficit
7. [ML/embedded, 2025, extended] per-client noise cost decreases with number of clients distributed DP federated learning embedded device 2025
8. [DP/timing, 2026, extended] constant-time discrete Gaussian sampler for differential privacy timing attack floating point 2026
9. [ePrint/theory] Micciancio Walter Gaussian sampling over the integers convolution running time independent of sigma constant-time base sampler
10. [ePrint/theory] "Gaussian Sampling over the Integers" Micciancio Walter convolution number of base samples logarithmic in standard deviation

## Papers opened / verified
- arXiv 2102.06387, Kairouz, Liu, Steinke, "Distributed Discrete Gaussian Mechanism for FL with SecAgg" (ICML 2021). VERIFIED.
  Depth: FULL TEXT (arXiv HTML v4, both halves, about 198k chars, read through the fetch model). The sum noise variance is about
  n*sigma^2 and epsilon is about c/(sqrt(n)*sigma). So per-client noise of sigma_total/sqrt(n) is stated in the paper. Section 5.3
  ("Privacy Degradation from Client Dropouts", Fig. 9) quantifies the dropout deficit: privacy "degrades gracefully". Sampling uses
  the TensorFlow DGauss sampler, with no runtime, CT or device cost given. There is no MCU measurement.
- ePrint 2018/1234, Zhao, Steinfeld, Sakzad, "FACCT: Fast, Compact, and Constant-Time Discrete Gaussian Sampler over Integers"
  (IEEE TC). VERIFIED (abstract page). Depth: abstract. Quote: "The efficiency of our scheme is independent of the standard deviation."
  This is the KEY COUNTER-EVIDENCE to the candidate's premise that CT cost is linear in sigma.
- ePrint 2017/259, Micciancio and Walter, "Gaussian Sampling over the Integers: Efficient, Generic, Constant-Time" (CRYPTO 2017).
  VERIFIED (abstract page). The PDF returned 403 and the slides (cse.iitm.ac.in) refused the connection. Depth: abstract plus
  secondary snippets. A small-sigma CT base sampler is extended to large sigma by convolution, "without substantial slow-down".
  UNVERIFIED in this check: the exact base-sample count against sigma. My recollection is that it grows at most logarithmically,
  and it is NOT linear.
- arXiv 2209.12528, Jiang, Wang, Chen, "Dordis: Efficient FL with Dropout-Resilient DP" (EuroSys 2024). VERIFIED (search snippets
  plus the abstract-level text in results). Depth: snippet. Quantifies the dropout noise deficit (target epsilon 6 rises to 8.6)
  and fixes it with add-then-remove noise. So the dropout caveat is covered.
- Seen in results, not opened: arXiv 2112.05307 (Jin et al., S&P 2022, timing and floating-point attacks on DP samplers);
  FORC 2023 (Ben Dov et al., time-oblivious DP samplers); arXiv 2409.05623 (Ratliff-Vadhan, timing privacy); arXiv 2212.04371
  (Skellam mixture, sampled as a difference of Poissons, not CT); arXiv 2607.06320 (dithered Gaussian mechanism, randomness-
  efficient and float-attack-resistant, date unverified); Cybersecurity 2022 s42400-022-00113-0 ("Generic, efficient and
  isochronous Gaussian sampling over the integers"); ePrint 2025/1025 (MPC noise sampling); DAC 2019 Karmakar et al. (CT Falcon
  base sampler); PrivAgE edge toolchain (dl.gi.de); arXiv 2110.04995 (Skellam, not re-opened, seen in the N2 log).

## KNOWN / CONJECTURE / ASSUMPTION
- KNOWN: per-client std sigma/sqrt(n) and the dropout degradation are both stated in DDG (2102.06387, Sec. 5.3). Dropout deficits are
  quantified and repaired in Dordis (2209.12528). Discrete DP samplers leak through timing (2112.05307).
- KNOWN: CT samplers whose cost is independent of sigma exist (FACCT 2018/1234). Convolution reduces large-sigma CT sampling to a
  few small-sigma base samples (Micciancio-Walter 2017/259). The full-table CT-CDT scan is linear in sigma only because the table
  length is about tau*sigma. That is the textbook reason CDT is replaced at large sigma.
- CONJECTURE: with a sigma-independent CT sampler (FACCT-style or convolution), per-client cost is roughly flat in N. The "1/sqrt(N)"
  saving and the crossover then disappear, or shrink to the last table-scan term. The crossover exists only for the naive
  full-scan CDT, and N* = (85*sigma_total/c_float)^2 is closed-form arithmetic from our own numbers.
- CONJECTURE: under t colluding clients, honest clients must add sigma/sqrt(N-t) or more, which shifts N* upward. This is still arithmetic.
- ASSUMPTION: our 85-instructions-per-sigma slope (QEMU, randomness excluded) holds for the per-client sigma range. The float
  Box-Muller baseline cost is constant per sample. Every client is sampled every round (no subsampling).

## Verdict
PARTIAL OVERLAP, bordering on collision. No opened paper reports CT integer-noise cost against N on an MCU or states this
crossover, so that exact framing is open as far as searched (10 queries). But each ingredient is published: sigma/sqrt(n) and
dropout degradation in DDG 2102.06387, the dropout deficit in Dordis 2209.12528, and sigma-independent CT sampling in FACCT
2018/1234 and Micciancio-Walter 2017/259. The latter undermines the central premise that sound CT cost scales with sigma. What
remains is an M33 cost table (CT-CDT vs FACCT/convolution vs float, against N) as a sub-row of D2/N1. It is not a standalone direction.

## Most damning reviewer objection
"sigma/sqrt(N) per client is in the DDG paper, and so is the dropout degradation. Linear-in-sigma cost is a property of the
naive full-scan CDT, which FACCT and Micciancio-Walter already replace with sigma-independent CT samplers. Your crossover is the
artefact of picking the worst CT sampler, and with a proper one there is no N-dependence to report."
