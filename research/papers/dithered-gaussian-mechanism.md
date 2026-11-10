# Dithered Gaussian Mechanism for Randomness-Efficient Differential Privacy

## Citation
- Title: Dithered Gaussian Mechanism for Randomness-Efficient Differential Privacy
- Authors: Nikita P. Kalinin (Institute of Science and Technology Austria), Rasmus Pagh (BARC, University of Copenhagen)
- Status (verified on arxiv.org/abs/2607.06320): **arXiv-only**, cs.CR + cs.LG. No journal-ref, no venue in the comments field.
- ID: arXiv:2607.06320. v1 7 Jul 2026; v2 28 Sep 2026; v3 1 Oct 2026 (current). v3 comment: "Improved Sampling Algorithm + Numerical Comparison with Baselines".
- **Version warning:** the caller's figures (~30% overhead vs. non-crypto PRNG, ~20% vs. CSPRNG) come from **v1**. v3 replaces them with 3%–7% overhead, measured on a different setup (see Numbers). Cite v3, and give the version number for every figure.

## Reading depth
- Full text of the arXiv HTML for v1 and v3, read through an extraction tool in chunks.
- Some v3 tables were truncated in extraction: the secure-noise timing tables, the standard-Gaussian timing rows, and the CIFAR-10 training-time table. I did not get their numbers.
- The PDF could not be rendered in this environment.
- Figure 4 (entropy vs. σ) was not read numerically. It is only described in the text.

## Core claim (one sentence)
Instead of discretizing the noise, round the Gaussian mechanism's noisy output onto a grid with a public random shift (a "dither"). The rounding is post-processing, so the result keeps the Gaussian mechanism's DP guarantee and its accounting (composition, subsampling). The output is discrete, so there is no floating-point leakage. It also needs only a constant number of private random bits per coordinate, independent of σ.

## Mechanism (KNOWN, from paper)
- Public dither: two uniforms (a, b). Per-coordinate grid shift γ_i = (a·i + b) mod 1, grid spacing ξ.
- Output: the integer grid index Z_i of f(X)_i + y_i, where y_i ~ N(0, σ²).
- Z_i is sampled directly from its explicit cell probabilities (Eq. 6), which are differences of the Gaussian CDF Φ. The continuous Gaussian y_i is never generated.
- Lemma 1: the output is distributed as f(X)_i + y_i + u_i with u_i ~ U(−ξ/2, ξ/2). Variance is σ² + ξ²/12.
- Prop. 2: the total variation distance to a pure Gaussian is ≤ 0.0202·(ξ/σ)² per coordinate.
- Lemma 3: private entropy H(Z | γ) ≤ (d/2)·log₂(2πe[(σ/ξ + 1/2)² + 1/12]). This is a small constant per coordinate when ξ = Θ(σ).
- Sampler (Appendix B, Algorithm 2):
  - It is an inverse-CDF sampler with adaptive bit consumption. It draws t-bit blocks per unresolved coordinate and narrows an interval [L, R].
  - A coordinate is output once its interval lies inside one CDF cell. A coordinate uses r_i × t bits in total.
  - t is chosen "so that most values are sampled on the first attempt". No numeric t is stated in the text I read.
  - v3 practical version: float64 throughout, via torch.special.ndtri/ndtr, with a "conservative margin ϵ = 10^-12".
  - Coordinates that float64 cannot certify fall back to mpmath arbitrary precision, with the float64 parameters converted to exact rationals. The fallback rate is not reported; the paper says only "a small number".
  - v3 says no truncation of the support. v1 described truncating to a window with tail mass ≤ δ′.
- Also included: a Dithered Laplace mechanism (Lemma 10, Theorem 9), with O(log(d/β)) public bits and o(1) or sublinear private randomness in some accuracy regimes.

## Floating-point security claim
- KNOWN: the paper claims the output avoids "vulnerabilities caused by finite-precision floating-point outputs". It motivates this with Mironov 2012 (Laplace) and Jin et al. 2022 (Gaussian).
- KNOWN: the argument is structural. The released value is an integer given the public dither, and the privacy proof is post-processing of the idealized continuous Gaussian.
- KNOWN: there are no attack experiments.
- INFERENCE (ours):
  - Soundness of the implemented sampler rests on the float64 certification margin plus the mpmath fallback. If certification works as described, sampling is exact; I found no formal error analysis.
  - The design assumes float64 is available. Nothing is said about float32.
  - On a Cortex-M33, float64 is software-emulated (the M33 FPU is single-precision only). That makes the cost of ndtri/ndtr there an open question, not something the paper answers.

## Random bits per sample
- KNOWN (v3, Appendix C): "[2]2 bytes of randomness per coordinate on average", versus "40[40] bytes" for the jax_privacy discrete Gaussian, "20[20] times more".
- The HTML extraction doubles digits ("22", "4040", "2020"). The internal ratio 40/2 = 20 supports **about 2 bytes (16 bits) per coordinate**.
- INFERENCE: treat this as 2 B/coordinate until checked against the PDF.
- For comparison (ours, ASSUMPTION): float32 Box-Muller consumes 2×32 bits per 2 samples, about 4 B/sample. So the randomness saving vs. our baseline is roughly 2×, not 20×.

## Platform
- **PC/server only.** No embedded hardware, no emulator, no compiler flags reported.
- v1: a single NVIDIA A100 with 16 CPU cores; 10 epochs; averaged over 3 runs.
- v3: CPU is an AMD EPYC 9654 (16 cores); GPU is an A100 with 16 CPU cores; sampling microbenchmarks averaged over 100 runs.
- Implementation: Python/PyTorch, float64.
- Secure noise comes from the Python `secrets` module. Non-secure noise comes from the PyTorch generator.

## Threat model (as far as stated)
- Standard central/local DP adversary who sees the released (noised, discretized) gradient or query output, including the public dither.
- Protected: individual records, at the DP guarantee of the Gaussian mechanism.
- Explicitly out of scope:
  - Timing attacks: "A separate vulnerability, shared by all of the methods considered here, is exposure to timing attacks."
  - Distributed/federated settings.
  - Physical side channels: not mentioned.

## Numbers that matter
**v1 DP-SGD training time (CIFAR-10 ConvNet, batch size 512, A100)**
- About 30% over standard Opacus (non-secure noise).
- About 20% over Opacus "secure mode". The paper treats secure mode as "a heuristic rather than ... a formal guarantee".
- Egan (2024) is cited for 74% to more than 400% CSPRNG-noise overhead at batch size 32.

**v3 DP-SGD training time**
- "only a 3%−7% runtime overhead relative to standard Gaussian DP-SGD for batch size 512", with cryptographically secure noise.
- The discrete Gaussian is about 45% slower on CIFAR-10 and about 3.2× slower on IMDb.

**v3 Table 1 (CPU, non-secure noise, ms)**

| d | Dithered (ξ = 0.5σ) | Discrete (h = 0.5/√d) |
|---|---|---|
| 10⁶ | 13.35 | 79.2 |
| 10⁷ | 105 | 1868 |

**v3 Table 2 (GPU, ms, d = 10⁷)**

| Mechanism | Time |
|---|---|
| Dithered (ξ = σ) | 3.14 |
| Discrete | 1635 |

**v3 Table 5 (CPU, d = 10⁶, ms)**

| Mechanism | Time |
|---|---|
| Dithered | 12.56 |
| jax_privacy discrete Gaussian | 175.9 |
| OpenDP discrete Gaussian | 17230 |

The paper summarizes the speedup over jax_privacy as "between 2× and 500× faster".

**v3 Table 8 (CIFAR-10 accuracy %)**

| Mechanism | ε = 0.5 | ε = 1 | ε = 2 | ε = 4 | ε = 8 |
|---|---|---|---|---|---|
| Gaussian | 43.7 | 47.1 | 51.1 | 53.1 | 56.2 |
| Dithered (ξ = σ) | 42.6 | 46.4 | 50.5 | 54.2 | 56.7 |

**v3 Table 9 (BERT-Base on IMDb, accuracy %)**

| Mechanism | ε = 0.5 | ε = 1 | ε = 2 | ε = 4 | ε = 8 |
|---|---|---|---|---|---|
| Gaussian | 85.6 | 86.9 | 88.0 | 88.9 | 89.4 |
| Dithered (ξ = σ) | 85.1 | 87.0 | 88.3 | 88.8 | 89.0 |

**v3 Table 10 (BERT, seconds over 10 epochs, ε = 0.5)**

| Mechanism | Time |
|---|---|
| Gaussian | 3896 |
| Dithered | 4152 |
| Discrete | 12680 |

v1 also reports that accuracy is essentially unchanged for ξ ∈ {σ/2, σ} and degrades at ξ = 2σ.

## Convenient assumptions
- float64 and erf/ndtri are cheap. Large-batch GPU training means noise is a small share of step time.
- That share is why the overhead is small. On an MCU running DP-SGD with a tiny model, noise dominates; that is our 66% emulation figure.
- A centralized trainer adds the noise, so there is no client decomposition.
- The adversary is not timing-capable. Python `secrets` is taken as the CSPRNG.
- Image and text benchmarks only (CIFAR-10, IMDb). No sensor or time-series data.

## Stated limitations / future work (paraphrased)
- The mechanism is not additive, so noise cannot be split across clients. Integrating it with secure aggregation in FL is left as future work.
- Timing attacks remain open for all methods compared.
- It would benefit from dedicated hardware support for discrete sampling and representation, avoiding conversion to floating point.
- The sampler is approximate in v1. v3 makes it exact through float64 certification plus an mpmath fallback.
- Limited datasets and models.

## Embedded / FL / secure aggregation coverage
- Embedded, IoT, microcontroller: **not discussed** ("embedded" does not appear in the text I read).
- FL and secure aggregation: discussed only as a limitation and future work, because the mechanism is not additive.

## Load-bearing references
1. Canonne, Kamath, Steinke 2020: the discrete Gaussian, the baseline it is compared against.
2. Mironov 2012 and Jin et al. 2022: floating-point attacks on Laplace and Gaussian samplers, the motivation.
3. Kairouz, Liu, Steinke 2021: the distributed discrete Gaussian for FL with SecAgg, which the dithered mechanism cannot replace.
4. Canonne, Su, Vadhan 2025: randomness complexity of DP.
5. Egan 2024: CSPRNG overhead in ML, and RNG co-processors.

## Possible seeds (direction-miner moves)
- **Move to the constrained platform.**
  - Cost the dithered sampler on a Cortex-M33: float64 soft-float ndtr/ndtri versus a fixed-point or float32 certification, against our float32 Box-Muller-from-ChaCha20 baseline (66% of the step).
  - Open as far as searched. Searched only this paper.
- **Swap the precision assumption.**
  - Can the certified inverse-CDF be done in float32 or fixed point with a wider margin and a rarer exact fallback?
  - Measure fallback rate vs. margin, and cycles. The paper reports no fallback rate.
- **Attack the "out of scope".**
  - Timing is explicitly excluded, and the adaptive bit consumption (r_i blocks) plus the mpmath fallback create data-dependent timing.
  - This is a natural phase-2 side-channel target on the EFR32xG24.
- **Close the stated limitation.**
  - The mechanism is non-additive, so it does not fit FL with secure aggregation. A local-DP or single-client on-device setting is where it does apply.
  - A seed must state its setting clearly to avoid inheriting the SecAgg gap.
- **Randomness budget.** About 2 B/coordinate vs. about 4 B for float32 Box-Muller. That is a ChaCha20 cycle saving, roughly 2× (INFERENCE), but it only pays off if the CDF evaluation is cheaper than the saved keystream.
