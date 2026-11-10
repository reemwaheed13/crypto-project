# Revisiting Continuous Noise Sampling for Multi-Party Differential Privacy

## Citation
- Title: Revisiting Continuous Noise Sampling for Multi-Party Differential Privacy
- Authors: Yucheng Fu, Tianhao Wang
- ID: arXiv 2608.27766 (cs.CR), v1 submitted 27 Aug 2026. Only one version.
- Venue/status: **arXiv-only (verified 2026-11-10).** The arXiv page lists no comments and no venue. A web search found no published version.
- Same group's earlier work: Fu & Wang, "Benchmarking secure sampling protocols for DP", CCS 2024 (arXiv 2409.10667). Wei, ..., Wang, "Securely sampling discrete Gaussian noise for multi-party DP", CCS 2023.

## Reading depth
- **Full text via arXiv HTML** (about 140k characters, read in two passes).
- Both passes went through a summarising fetch tool, so the fine detail is second-hand. Quotes below come from that tool's extraction. Check them against the PDF before citing.
- The conclusion section was not clearly visible in either pass. "No stated open problems" therefore means "none found", not "confirmed absent".

## Core claim
In MPC-based distributed DP, the common "sample-and-scale" method has a flaw: it draws a standard Laplace or Gaussian value on a fixed-point grid, then multiplies by a public scale s. The result can only land on a sparse, publicly known set of grid points. An attacker who sees the released outputs can often tell which of two neighbouring datasets produced them. The authors replace it with a discrete Laplace / discrete Gaussian sampler built from parallel biased bits inside MPC, which is secure and also faster.

## Platform (KNOWN)
- **PC/server only.** Hardware: "16-core AMD Threadripper PRO CPU and 256 GB RAM" (Sec. 7.1).
- Network: emulated LAN and WAN, "up to 1 Gbps" bandwidth and "up to 100 ms" latency. The introduction says "up to 11 Gbps" for its motivating measurement, which does not match Sec. 7.1.
- Framework: MP-SPDZ. The sampler uses its low-level Boolean API. Security parameter λ = 64.
- Main experiments are semi-honest two-party. The appendix goes up to 16 parties.
- Fixed-point format attacked: n = 21 integer bits, l = 20 fractional bits (Sec. 5). The PrivaDA floating-point variant uses significand β = 32, l = 32.
- Efficiency sweep: precision l from 8 to 32, B = 1,024 samples, scale 10·2^l (Fig. 8).
- No MCU, no embedded target, no compiler flags reported.

## Setting and threat model (KNOWN)
- Setting: m users secret-share their data to N servers, and the servers run MPC to compute a noisy aggregate.
- Motivating uses are secure aggregation and DP federated learning. DP-BREM+ is an FL system, and Fig. 10 shows FL on MNIST with N = 16.
- **Clients only secret-share their data. All noise sampling runs between the servers.**
- Adversary: an honest-but-curious data analyst. They see the released output, know both neighbouring datasets and the implementation, but not the randomness (Sec. 3).
- MPC corruption: semi-honest, dishonest majority, "up to N−1 parties" (Thm 6.1).
- Malicious security is not evaluated. The authors argue a malicious backend changes only the preprocessing cost.
- Out of scope: side channels on the sampler itself (timing, power), client-side noise generation, malicious servers.

## Distributions and implementations analysed (KNOWN)
- Continuous Laplace and Gaussian via sample-and-scale on fixed-point. These are the vulnerable baselines, used by Orchard and DP-BREM+.
- PrivaDA-style floating-point sampler. This is also attacked.
- The proposed sampler draws discrete Laplace and discrete Gaussian noise at integer scale t = s·2^l, then maps it to the grid by multiplying by 2^-l.
  - Discrete Laplace: a truncated geometric variable is built from κ independent secure biased coins, then a sign/correction step makes it two-sided (Alg. 3).
  - Discrete Gaussian: uses a geometric proposal with rejection (App. E).
  - All coins are flipped in parallel, then one batched Boolean-to-arithmetic share conversion is applied. The number of rounds is nearly independent of the number of samples.
- **Skellam: only cited, not analysed. Binomial: not found in the text.** So distributed noise where each client adds its own share (as in DDGauss / Skellam / binomial secure-aggregation schemes) is NOT the subject. The paper is about server-side MPC sampling.

## Vulnerabilities (KNOWN)
- Main issue: scaling a grid-valued sample by s ≥ 2 can only reach a sparse subset of the grid. Neighbouring inputs can then produce disjoint output sets, so the outputs can be distinguished.
- Attack: Algorithm 2 checks, for each candidate dataset, whether the noise it would require is reachable on the scaled grid. If both candidates pass, it falls back to maximum likelihood.
- Results:
  - Orchard (DP k-means, Laplace): succeeds on essentially every query for ε ≤ 0.5 (Fig. 3) and approaches certainty by the 5th observation (Fig. 4).
  - PrivaDA variant: "stays above 99%" for ε from 0.1 to 20, and 100% vs a 54.3% baseline at ε = 0.1 (Fig. 5).
  - DP-BREM+ (FL, Gaussian): above 87% everywhere and 100% for every s ≥ 2 (Fig. 6). With the paper's clip values R ∈ {5, 10, 20, 50}, the resulting s = Rσ falls in the range that breaks the protocol.
  - Neighbouring-pair variants: 100% across all pair types (Fig. 7). On the same-distribution pair, the floating-point attack of Jin et al. drops to 50%.
- Implementation gaps noted: Orchard's prototype `laplace_fx` adds a single random bit, and DP-BREM+ uses `torch.normal` in plaintext simulation (Sec. 5, footnotes).
- Timing: only in related work (Jin et al. S&P 2022 timing attacks on discrete and floating-point Gaussian). **The new sampler is not analysed for timing leakage. "Constant-time" is not discussed. Only "constant online rounds" is.**

## Numbers that matter (KNOWN, PC/server, emulated network)
- Motivating cost: sample-and-scale takes about 19 s (Laplace) and about 61 s (Gaussian) for 2-party WAN at s = 10, l = 16 (Sec. 1).
- The speedup claims are stated inconsistently across sections:
  - Abstract: 4×–612× over existing secure discrete samplers.
  - Intro: 2×–706× over directly adapted secure samplers, and 190×–1,317× over insecure sample-and-scale under WAN.
  - Fig. 8: up to 1,300× (Laplace) and 550× (Gaussian) over sample-and-scale; 2×–10× over garbled circuits; 217×–706× over binary secret sharing [Fu & Wang CCS'24].
- Rounds: 476×–1,733× fewer than the adapted secret-sharing sampler. The Gaussian sampler "still needs 689 online rounds" even after optimisation. The extraction showed "689689", which is presumably a duplicated rendering; verify in the PDF.
- Communication: 4.5×–6.1× lower than garbled circuits (2-party).
- Table 3 (App. D) compares against the LUT sampler of Franzese et al. for 2/4/8/16 parties:

  | Metric | Method | 2 | 4 | 8 | 16 |
  |---|---|---|---|---|---|
  | LAN (s) | Ours | 0.0017 | 0.0017 | 0.0093 | 0.04 |
  | LAN (s) | LUT | 0.09 | 0.07 | 0.06 | 0.06 |
  | WAN (s) | Ours | 0.10 | 0.11 | 0.11 | 0.24 |
  | WAN (s) | LUT | 0.10 | 0.08 | 0.08 | 0.25 |
  | Comm (MB) | Ours | 0.13 | 0.76 | 3.54 | 15.10 |
  | Comm (MB) | LUT | 0.72 | 1.91 | 3.30 | 11.59 |

- Utility: KS test D_n about 0.012–0.014, p about 0.50–0.62 at l ∈ {8, 16, 32} (Table 2). FL on MNIST with N = 16 matches the centralised mechanism (Fig. 10).

## Convenient assumptions
- Honest-but-curious analyst and semi-honest servers. No malicious evaluation.
- Server-class machines with 256 GB RAM. Clients do no noise work, so client compute and radio cost are absent.
- MNIST only for FL utility, with i.i.d.-style partitioning presumably. The split was not verified.
- Tail truncation at 2^κ is accepted, leaving a small statistical distance.
- The analytic-Gaussian refinements are skipped "for simplicity".

## Stated limitations / future work (paraphrased)
- Fixing sample-and-scale by rounding the query costs utility. Adding guard bits widens every circuit value and slows the sampler.
- No MPC protocol rounds transcendental functions exactly, and approximations may themselves break DP.
- Truncation leaves a residual statistical distance.
- Malicious security is not evaluated.
- The LUT sampler's table must cover the whole support. Ours sends more data than LUT at 16 parties.
- No explicit open-problems list was found.

## Load-bearing references
1. Wei, Yu, Fan, Chen, Wang. "Securely sampling discrete Gaussian noise for multi-party DP." CCS 2023.
2. Fu, Wang. "Benchmarking secure sampling protocols for DP." CCS 2024 (arXiv 2409.10667).
3. Jin, McMurtry, Rubinstein, Ohrimenko. "Are we there yet? Timing and floating-point attacks on DP systems." IEEE S&P 2022.
4. Mironov. "On significance of the least significant bits for DP." CCS 2012.
5. Franzese et al. "Secure noise sampling for differentially private collaborative learning." ePrint 2025/1025.
- Also relevant: Keller et al., "Secure Noise Sampling for DP in MPC with Finite Precision", ePrint 2023/1594. Canonne et al., discrete Gaussian.

## Relevance to our candidates (INFERENCE, ours)
- (a) Constant-time DP sampling on MCUs: the paper is only indirectly relevant. It shows that fixed-point continuous sampling plus scaling is broken, and that the fix is to sample a discrete distribution at the integer scale. It says nothing about timing, constant-time code or embedded targets.
- (b) Client-side distributed DP for FL with secure aggregation (DDGauss / Skellam / binomial): the paper does not cover this. Its noise is generated by servers inside MPC, not by clients. Its sparse-support argument may apply to clients that generate noise in fixed point and then scale. That is CONJECTURE, not tested here.

## Possible seeds
- Transfer setting: apply the sparse-support (sample-then-scale) attack to client-side noise in FL secure aggregation on fixed-point MCU code. Do clients scale a unit discrete Gaussian or Skellam sample? CONJECTURE.
- Constrained-client cost: bring the "biased-coin geometric" construction to a single MCU in plaintext and measure cycles, RNG bits per sample and timing variance on Cortex-M33. The paper has only server costs.
- Side-channel follow-up: the rejection step in the discrete Gaussian sampler (App. E) is a natural timing/SPA target in a non-MPC embedded port. This fits phase 2 on the EFR32xG24.
- Re-check the vulnerable-system list: scan open-source FL+DP client libraries (e.g. TFF DDGauss, Skellam implementations) for scale-after-sample patterns.
- Not yet searched for collisions: "constant-time discrete Gaussian sampler microcontroller DP", "Skellam client side embedded federated learning". Open as far as searched: none of these queries run yet.
