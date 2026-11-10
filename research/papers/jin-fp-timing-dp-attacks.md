# Jin, McMurtry, Rubinstein, Ohrimenko — Timing and Floating-Point Attacks on DP Systems

## Citation
- Title: "Are We There Yet? Timing and Floating-Point Attacks on Differential Privacy Systems"
- Authors: Jiankai Jin, Eleanor McMurtry, Benjamin I. P. Rubinstein, Olga Ohrimenko
- Venue: IEEE Symposium on Security and Privacy (S&P) 2022, as stated in the arXiv comments field. Published.
- ID: arXiv 2112.05307. Versions v1 10 Dec 2021, v2 8 Apr 2022, v3 15 Jun 2022, v4 11 Sep 2024 (current).

## Reading depth
Full text, read through tool-assisted extraction rather than visual page reading:
- arXiv HTML v4 (https://arxiv.org/html/2112.05307v4): body through the Conclusion. The appendices were cut off in that rendering.
- ar5iv HTML (an earlier, undated version): full text, including Appendices A–D and Table II.

The PDF itself could not be rendered locally because poppler is not installed. Numbers below come from the extracted text. Read Fig. 3 and Fig. 5 as plots before quoting any per-epsilon values; I did not see the plotted values.

## Core claim
Floating-point Gaussian samplers in NumPy, PyTorch and Go can produce only certain output values, so an observer can tell neighbouring datasets apart (DP-SGD in Opacus included). The discrete Laplace and Gaussian samplers proposed as the fix leak the noise magnitude through running time.

## Platform
- **FP attack.** PC software only: Python and Go. The analysis assumes IEEE 754 double precision. Section IV-B says random "doubles" are sampled on modern computers.
- **Resolution R.** The uniform resolution R = 2^p is called system-dependent. The only concrete value is R = 2^10, for the authors' toy polar sampler in Fig. 1 (sigma = 114). No R is given for NumPy or PyTorch.
- **Precision in the DP-SGD experiment.** Never stated. No dtype is given and float32 is never mentioned (KNOWN, from searching the text). PyTorch's default tensor dtype is float32, so the Opacus run may well have used float32 noise. That is OUR INFERENCE and the paper does not confirm it.
- **Timing attack hardware.** One core of an Intel Xeon Platinum 8180M, Ubuntu 16.04.1 64-bit, kernel 4.15.0-142, with no other processes on that core. Timers were Python `time.process_time_ns()` and Java `System.nanoTime()` (Section VII-A).
- **Compiler flags.** None reported.
- **Embedded targets.** No microcontroller, embedded, GPU or float32 platform is discussed anywhere. All costs and attacks were measured on PC/server only.

## Threat model (Section III)
- **Adversary knowledge.** Knows every record except the target, knows the neighbouring datasets D and D', knows f, and knows the implementation. It does not know the sampler's random bits.
- **Scenarios.**
  - (S) public release of statistics.
  - (DB) an analyst querying a DP database.
  - (FL) a central server receiving clients' noisy gradients. This is the framing used for DP-SGD.
- **FP attack needs.** Several outputs from repeated queries, or one d-dimensional output such as a histogram or per-parameter gradient. The polar and Box-Muller attacks assume the attacker knows the second (cached) sample s2 (Section IV-C2). Ziggurat needs only one sample.
- **Timing attack needs.** Black-box access to measure running time, with a profiling baseline first. One timing observation is enough during the attack.
- **Asset.** Membership of one record (DP-SGD canary) or a sensitive attribute value (private count or sum).
- **Out of scope.** The RNG and CSPRNG themselves (the random bits are assumed secret and ideal). Final-model observation. Power and EM side channels. Multi-step composition of the FP attack.

## What was attacked
**Floating-point attack (Sections IV–V)**

| Sampler | Method | Library |
|---|---|---|
| Gauss_polar | Marsaglia polar | Authors' own implementation |
| Gauss_numpy | Polar | NumPy |
| Gauss_pytorch | Box-Muller | PyTorch |
| Gauss_go | Ziggurat (`NormFloat64`, n = 128) | Go `math/rand` |

- **DP-SGD.** Run on Opacus, which uses the PyTorch sampler.
- **Mentioned but not tested.** GNU C++ `std::normal_distribution` (polar), Java `nextGaussian` (polar), and older Diffprivlib (Box-Muller).
- **Mechanism.**
  - Polar: the attacker checks whether exp(-(s^2+s2^2)/2)·R^2 is an integer for a candidate s.
  - Box-Muller: the same check, done "similarly, using trigonometric functions."
  - Ziggurat: the attacker checks whether s/(sigma·w[i]) is an integer for some table entry.
  - All cases search 50 neighbouring float values in each direction to absorb rounding, which causes false positives and false negatives.
  - Ziggurat tail samples (under 0.1% for n = 128) are not attacked.

**Timing attack (Sections VI–VII)**
- **Implementation I.** Canonne–Kamath–Steinke discrete Gaussian and discrete Laplace, Python (IBM repository). The geometric subroutine draws Bernoulli trials until the first success, so running time grows with the noise magnitude. The authors argue this subroutine, not rejection sampling, is what leaks.
- **Implementation II.** Google DP discrete Laplace, Java. The geometric sample comes from a binary search, and less likely (larger) values take longer to find.
- **Not vulnerable.** Google's discrete Gaussian (binomial plus rejection).
- **Less vulnerable.** Implementation I's discrete Gaussian, compared with its discrete Laplace.

## Numbers that matter
- **Abstract headline figures.** "up to 92.56%" for the FP attack on DP-SGD and "up to 99.65%" for the end-to-end timing attack on a private sum with discrete Laplace. Neither number appears in the extracted body text. Treat both as abstract-only and check them against the plots before citing.
- **DP-SGD (Section V-C, Fig. 3, Appendix B).**
  - Setup: MNIST, the Opacus MNIST example model (d = 26,010 parameters), lr 0.1, 8 epochs, batch 64, 100 batches per epoch, clip L = 1, delta = 1e-5, sigma in [1, 250] giving epsilon from 0.1 to 0.68. Sensitivity 2L/S = 1/32.
  - The adversary sees one noisy batch gradient (26,010 floats) and decides whether a canary is in the batch.
  - DiffLabelCanary: success is "always better than a random guess" for epsilon < 0.68, and rises with sigma. Results are plotted only, with no table.
  - SimLabelCanary: no distinguishing.
  - Multi-step accumulation, final-model attacks and a real multi-client FL deployment were not tested. Each test is a single-update distinguishing game.
- **Private count, FP attack (Section V-B1, Table II, Fig. 7).**
  - NumPy and PyTorch: attack rates of 1.7% to 92.8%, with accuracy of at least 89%.
  - Go Ziggurat: attack rate at least 76%, accuracy at least 99%. The text says "above 99.99%" while the caption says 99.9%, which is internally inconsistent.
  - Settings: epsilon from 1 to 20, delta = 1e-5, sensitivity 1 or 10.
- **Timing, per-sample magnitude guess (Table I).** Random baselines are 10% exact and 33% within ±1.

  | Sampler | Parameter | Exact | ±1 |
  |---|---|---|---|
  | Gaussian I | sigma = 2 | 24.4% | 56.2% |
  | Gaussian I | sigma = 4 | 13.6% | 39.9% |
  | Laplace I | lambda = 1 | 42.1% | 84.2% |
  | Laplace I | lambda = 3 | 24.7% | 61.5% |
  | Laplace II | lambda = 1/ln3 | 42.0% | 89.0% |
  | Laplace II | lambda = 3/ln3 | 17.0% | 44.0% |
  | Laplace II | lambda = 1/ln2 | 32.0% | 71.0% |

- **Timing, private sum (Section VII-C, Fig. 5).** German Credit credit attribute, sensitivity 5000, Laplace II, epsilon from 1 to 10, one million trials per epsilon. The text states 69.15% at epsilon = 1, rising with epsilon.
- **Padding mitigation (Section VIII-B, Fig. 8).** Pad to 41 µs, which covers 99.5% of noise magnitudes (in [0, 2677]), at epsilon = 10 and sensitivity 5000. Mean runtime 37.1 µs, max 40.3 µs. Total execution time grows by 10.5%. Attack success after padding is not reported.

## Mitigations and their costs (Section VIII)
- **Snapping (Mironov).** Cited as reducing privacy and utility. Not measured here.
- **Drop the cached second sample** in polar and Box-Muller. Not evaluated. It would defeat the s2-known assumption but costs double the sampling work (our reading).
- **Average several noise samples.** The authors say this "may still be susceptible to attacks." Not evaluated.
- **Discrete Gaussian in DP-SGD** (Appendix C): clip, randomly round onto a gamma-grid, add discrete Gaussian noise, rescale. At gamma = 1e-3 accuracy matches continuous DP-SGD (Fig. 6). Only accuracy was measured. Runtime and attack resistance were not.
- **Timing defences.**
  - Constant-time sampling or offline batched generation with shuffling. Neither was measured.
  - Padding, at the 10.5% cost above.
  - Lattice-crypto constant-time samplers are noted, with a caveat: they are only statistically close to the target distribution, so standard composition analysis does not apply directly.
- **Disclosure.** Opacus proposed a fix after the authors notified Facebook (pytorch/opacus PR #260, per a footnote). The paper does not describe its contents.

## Convenient assumptions
- The attacker knows the cached second sample s2 (polar and Box-Muller), and knows f(D) and f(D') exactly.
- Double precision and PC libraries only, with no constrained or embedded arithmetic. R is unspecified for the real libraries.
- Single-update distinguishing game for DP-SGD, MNIST image data only, one fixed model.
- The timing attacker gets clean measurements on an isolated server core, with sampling time dominating everything else.
- The RNG is an ideal source of secret bits.

## Stated limitations
- The FP check has false positives and negatives because of the bounded neighbour search and multiple (s1, s2) fits. Ziggurat tails are not attacked.
- Box-Muller without a cached sample would need brute force.
- The timing attack works for one-dimensional outputs: "the attack will not be as successful if the attacker were to observe the time it takes to draw multiple samples." That makes it weak against DP-SGD.
- There is no explicit future-work section. The conclusion stresses careful implementation of DP mechanisms.

## Load-bearing references
- Mironov, "On significance of the least significant bits for differential privacy," CCS 2012. The original FP attack on Laplace. The paper itself calls it "2011."
- Canonne, Kamath, Steinke, "The discrete Gaussian for differential privacy" (NeurIPS 2020). The discrete samplers that were attacked.
- Google differential-privacy library. Implementation II.
- Opacus (PyTorch DP-SGD). The DP-SGD target.
- Kairouz et al., distributed discrete Gaussian for FL (cited around [39] for the rounding in Appendix C). I did not verify the reference number.

## KNOWN vs OUR INFERENCE, relevant to on-device float32 DP-SGD on Cortex-M
- **KNOWN.**
  - No float32, single-precision, FPU, embedded or MCU experiment exists in this paper.
  - All FP analysis assumes doubles. The DP-SGD precision is unstated.
  - The FP attack exploits the arithmetic structure of the sampler (polar, Box-Muller, Ziggurat) given known s2 and the uniform resolution R.
  - The timing attack is shown only on discrete samplers on a Xeon.
- **OUR INFERENCE.**
  - float32 Box-Muller (logf, cosf) has a far coarser output lattice (24-bit significand) and probably a small R if uniforms are built from 23–24 bits of ChaCha20 output. Gaps between achievable outputs could then be larger relative to the noise scale, so the attack may get stronger, not weaker. Untested.
  - newlib logf and cosf are not correctly rounded, which changes the feasible set the attacker must model. They may also be data-dependent in time (argument reduction branches in cosf), opening a timing channel on the continuous sampler too. Untested; neither is in this paper.
  - A single-cycle-accurate MCU with no OS noise is a cleaner timing target than the Xeon used here.
  - The "one-dimensional only" limitation may not hold for an MCU when per-coordinate timing is observable via power or EM. That is phase-2 territory.

## Possible seeds
- **Port to a constrained platform.** Redo the FP feasibility test for float32 Box-Muller with newlib logf and cosf and a ChaCha20-derived R on Cortex-M33 (QEMU first). Measure the fraction of infeasible outputs and the distinguishing rate.
- **Change precision.** Quantify how the float32 output lattice versus the sigma·C noise scale drives attack success, as a function of the clip norm and sigma used on-device.
- **Relax the assumption.** Box-Muller without a cached s2, with the attacker seeing both outputs of the pair as consecutive gradient coordinates. That is natural in a gradient vector, and the paper notes it only in passing.
- **Multi-step and FL.** The paper tests single updates only. Accumulate FP evidence across rounds of a federated client to see how the distinguishing advantage composes.
- **Mitigation cost on MCU.** Cycle and flash cost of a constant-time discrete Gaussian (Appendix C pipeline) versus float32 Box-Muller with snapping on Cortex-M, paired with the residual attack rate. Neither cost was measured in the paper.
- **Timing channel on the continuous sampler.** Check whether newlib logf and cosf runtime depends on the input on Cortex-M. Open as far as searched in this paper only; a literature search is still needed.
