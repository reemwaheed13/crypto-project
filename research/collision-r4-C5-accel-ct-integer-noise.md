# Collision check: C5, one hardware-AES randomness pass for SecAgg masks and constant-time integer DP noise (EFR32MG24)

Date: 2026-11-11
Checker: collision-check agent (round 4)

## Candidate (as given)
Generate both the secure-aggregation masks and the constant-time integer DP noise (distributed discrete Gaussian or
Skellam) from a single hardware-AES-CTR / DRBG stream on the EFR32MG24. Measure whether the accelerator removes the
8-17x cost of a constant-time discrete Gaussian sampler.
- Application: FL on IoT sensor clients (D2/N1 smart-meter setting) with secure aggregation and distributed DP.
- Asset: per-client updates and raw readings. DP covers the aggregate, masks hide the individual updates.
- Adversary: honest-but-curious server, possibly colluding with some clients. A timing/power observer comes in phase 2.
- Measured: instructions/cycles, random bytes and energy per noised-and-masked coordinate, with and without SE offload.

## Queries run (verbatim)
1. [technical] discrete Gaussian sampler hardware AES microcontroller randomness cost
2. [technical/plain] Skellam noise generation constant time embedded device distributed differential privacy
3. [plain/FL] secure aggregation masks and differential privacy noise generated from same PRG client
4. [ePrint-focused] eprint.iacr.org constant-time discrete Gaussian sampler AES-CTR randomness Cortex-M cycles per sample
5. [embedded] hardware AES accelerator PRNG lattice cryptography sampling speedup ARM Cortex-M4 SHAKE replaced AES
6. [ML/FL, 2025, extended] federated learning microcontroller differential privacy noise generation secure aggregation cost energy 2025
7. [embedded] Kyber 90s hardware AES accelerator microcontroller speedup sampling randomness share of cycles
8. [ML/TinyML, 2026, extended] on-device differential privacy noise sampling microcontroller TinyML secure aggregation discrete Gaussian 2026
9. [networking/IoT] secure aggregation mask expansion PRG AES hardware accelerator IoT device cost federated learning microcontroller

## Papers opened / verified
- arXiv 2604.19053, Hung Dang, "CHRONOS: A Hardware-Assisted Phase-Decoupled Framework for Secure FL in IoT". VERIFIED
  (abstract page). Depth: abstract. Masks come from one stream-cipher evaluation inside a TrustZone enclave. The testbed is
  Rock Pi 4 / Orange Pi 5, which are Cortex-A boards, not MCUs. The abstract has no DP noise and no sampler. A secondary
  summary says AES-128-CTR runs in hardware at over 1.5 GB/s (UNVERIFIED). This is the NEAREST NEIGHBOUR on the mask side only.
- arXiv 2604.07125, Wei, Nait-Abdesselam, Jammine, "Scalable and Private FL Using Distributed DP and Secure Aggregation"
  (v2, Jun 2026). VERIFIED (abstract page). Depth: abstract. Uses client Laplace noise and additive secret sharing. No PRG
  sharing, no MCU, no constant-time sampler.
- ePrint 2026/2287, Abou Haidar and Hoffmann, "Knuth-Yao masked (Gaussian) sampling". VERIFIED (abstract page). Depth:
  abstract. Masked Knuth-Yao on Cortex-M4 for FrodoKEM/HAWK/Falcon/HAETAE. Not about DP and no AES offload, but useful as a
  constant-time sampler baseline.
- Seen in results, not opened: arXiv 2112.06872 (Stevens et al., USENIX'22, already read in full in the N2 log); arXiv
  1903.04570 (MIT lattice processor: SHAKE 3x more energy-efficient than AES in their silicon, sampling about 70% of the
  software overhead); arXiv 2503.10207 (Kyber on ESP32 with the AES/SHA coprocessor, 1.7-1.8x overall); arXiv 2511.23252
  (one-shot IoT SecAgg, ChaCha20 masks); ePrint 2018/1234 (FACCT); DATE 2015 Knuth-Yao on Cortex-M4F; arXiv 2110.04995
  (Skellam); 2102.06387 (DDGauss); Falkor (AES-CTR SecAgg on server GPUs).

## Quantitative check of the main objection
Objection: "the bottleneck is the sampler's table scan, not the randomness, so an accelerator cannot help."
- KNOWN (our N1 §4 method): the CT-CDT counts 194/698/2,714 were measured with the randomness pre-generated, so they are
  the transform only. The accelerator can only shrink the separate randomness term.
- ASSUMPTION: a CT-CDT sample uses 64 bits (2 words), or 128 bits (4 words) at higher table precision.
  With ChaCha20 at 76.6 instructions per word, randomness costs 153 (or 306) instructions per sample.
- Share of randomness per sample: sigma0 = 2: 44% (61%); sigma0 = 8: 18% (30%); sigma0 = 32: 5% (10%).
  The 46-98 M/step estimate (sigma_int about 461) is built from wide base samples, so it falls in the 5-10% regime.
- SE Manager AES-128-CTR: 474 us per kB at 78 MHz (ASSUMPTION: the datasheet clock) is about 37,000 cycles per kB, or
  about 144 cycles per 32-bit word. Software ChaCha20 at CPI 1-2 costs 77-153 cycles per word. So the SE is not faster per
  word. At best it runs in parallel and hides the randomness term.
- Upper bound even with free randomness: 2,714/2,867 is about 0.95, so 8-17x becomes about 7.6-16x.
  The objection HOLDS: the accelerator cannot remove the 8-17x. CONJECTURE: the only possible gain is energy, if the CPU
  sleeps while the SE works, and the CPU cannot sleep because it is busy scanning the table.

## Second objection (design, not cost)
- KNOWN (Bonawitz et al. CCS 2017, protocol knowledge, not opened in this check): pairwise masks are expanded from
  pairwise-shared seeds, so they cannot come from one client-local stream. The self-mask seed b_u is revealed to the
  server for surviving clients during unmasking.
- If the DP noise came from the same seed or stream as any mask, the server could regenerate the noise and remove it,
  and DP would be void.
- So "one stream" is unsafe. Only "one engine with domain-separated keys" is sound, and that is ordinary engineering.

## KNOWN / CONJECTURE / ASSUMPTION
- KNOWN: hardware AES as a PRG for SecAgg masks (CHRONOS on Cortex-A; Falkor on GPU). AES/SHA coprocessors speed up
  Kyber-90s on ESP32 (2503.10207). Lattice-sampler randomness cost is a recognised issue (1903.04570). Our CT-CDT counts
  exclude randomness.
- CONJECTURE: the randomness is at most 5-10% of the CT-DGauss cost at the sigma that DP needs, so SE offload saves at most
  that much. A constant-time Skellam (Poisson) sampler would be even more scan- or arithmetic-bound.
- ASSUMPTION: 64-128 random bits per CDT sample; 474 us/kB measured at 78 MHz including SE mailbox overhead; ChaCha20 CPI 1-2.

## Verdict
PARTIAL OVERLAP in the literature. No opened or seen paper generates both SecAgg masks and constant-time integer DP noise
from one hardware-AES stream on an MCU, so that combination is open as far as searched (9 queries). However, the
candidate's central hypothesis fails our own numbers. The table scan dominates (about 95% at sigma0 = 32), the SE AES is
no faster per word than software ChaCha20, and sharing one stream with the masks breaks DP under Bonawitz unmasking.
What remains open is narrow: SE-offloaded randomness as an energy line item and a domain-separation note. This fits as a
sub-row of D2/N1's cost table, not as a standalone direction.

## Most damning reviewer objection
"Your own measurements show the constant-time sampler's cost is the table scan, not the random bits. Your accelerator is
no faster than ChaCha20 per word. And deriving the noise from the mask stream lets the server strip it. What is left?"
