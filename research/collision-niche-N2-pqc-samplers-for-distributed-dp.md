# Collision check: N2, PQC constant-time samplers reused as distributed-DP noise source on an MCU

Date: 2026-11-10
Checker: collision-check agent

## Candidate (as given)
IoT FL clients on a Cortex-M33 run ML-KEM (secure-aggregation key agreement) and must add integer DP noise
before secure aggregation (DDGauss, Kairouz-Liu-Steinke ICML 2021; Skellam, Agarwal-Kairouz-Liu NeurIPS 2021;
binomial/cpSGD). Claim: the ML-KEM centred binomial sampler (SHAKE-based, constant-time, already in flash), or a
Falcon/FN-DSA-style constant-time discrete Gaussian sampler, can produce the client DP noise at a fraction of the cost
of a dedicated DGauss/Skellam sampler. The privacy/accuracy penalty of binomial versus Gaussian/Skellam can be
quantified, and both costs can be measured on the M33.
- Application: FL on IoT sensor clients with secure aggregation.
- Asset: raw sensor readings and per-client updates (DP on the aggregate); constant-time sampling protects the noise from timing leaks.
- Adversary: honest-but-curious server plus collusion, which DP on the sum covers. A local timing/SCA observer is phase 2.
- Measured: cycles, flash and RAM per noised coordinate on the M33; the epsilon/accuracy penalty of binomial versus DGauss/Skellam at equal variance.

## Queries run (verbatim)
1. [ML/FL] binomial mechanism secure aggregation federated learning
2. [technical] centered binomial distribution differential privacy lattice cryptography sampler reuse
3. [technical] Kyber centered binomial sampler used as differential privacy noise federated learning
4. [ePrint-only] discrete Gaussian differential privacy sampler constant-time lattice sampler reuse
5. [embedded] discrete Gaussian secure aggregation microcontroller client noise sampling cost
6. [embedded/IoT] Skellam mechanism embedded device implementation cost distributed differential privacy IoT
7. [crypto/ML] differentially private secure aggregation learning with errors noise doubles as DP noise Stevens USENIX
8. [plain/IoT] distributed differential privacy client cost IoT microcontroller noise generation energy federated learning
9. [recency, extended] lattice encryption error reused as differential privacy noise federated learning 2025
10. [ML] Poisson binomial mechanism client computation cost on-device implementation
11. [embedded/TinyML] differential privacy noise sampler Cortex-M microcontroller TinyML constant-time discrete Gaussian
12. [ePrint-only] binomial noise distributed differential privacy secure aggregation sampler
13. [technical] Falcon FN-DSA discrete Gaussian sampler used for differential privacy mechanism
14. [recency 2026, extended] post-quantum cryptography and differential privacy shared noise sampler embedded device 2026
15. [theory] Rényi differential privacy of symmetric binomial noise mechanism tight bound vs Gaussian

## Papers opened / verified
- arXiv 2112.06872, Stevens, Skalka, Vincent, Ring, Clark, Near, "Efficient DP Secure Aggregation for FL via Hardness of LWE"
  (USENIX Sec 2022). VERIFIED. Depth: FULL TEXT (ar5iv HTML, about 100k of 100.2k chars read). The DP noise doubles as the LWE
  error (discretised Gaussian). Clients do not use a Kyber/CBD sampler, and no sampling algorithm is given. Client cost was
  measured in single-threaded Python on an AWS Xeon. There is no MCU or IoT measurement and no binomial-vs-Gaussian comparison.
  This is the NEAREST CONCEPTUAL NEIGHBOUR: one noise source does double duty for lattice crypto and DP. It runs in the
  opposite direction to N2 (DP noise used as LWE error, not a PQC sampler used as the DP source).
- ePrint 2024/1771, Bangalore, Cheu, Venkitasubramaniam, "PRIME: DP Distributed Mean Estimation with Malicious Security".
  VERIFIED (abstract page). Depth: abstract plus search snippet. The PDF returned 403. Each client samples BINOMIAL noise for
  secure aggregation, and a lemma relates binomial to Gaussian. The abstract mentions no hardware, lattice or IoT.
- arXiv 2207.09916, Chen, Özgür, Kairouz, "Poisson Binomial Mechanism" (ICML 2022). VERIFIED (search snippets, several mirrors).
  Depth: abstract-level. Binomial-type discrete DP for secure aggregation with RDP matching the Gaussian. No device cost reported.
- arXiv 2601.09460, "SoK: Enhancing Cryptographic Collaborative Learning with DP". VERIFIED (abstract). Evaluates noise sampling in
  MPC over WAN/LAN. No constrained devices and no lattice-sampler reuse.
- arXiv 2608.27766, "Revisiting Continuous Noise Sampling for Multi-Party DP". VERIFIED (abstract). A biased-bit sampler inside
  MPC. No client MCU work and no PQC reuse.
- Seen in search results, not opened: ePrint 2025/1025 (secure noise sampling in MPC); 2023/1594 (Keller, symmetric binomial in
  MPC); 2017/643 (Dahl-Pastro, binomial noise); arXiv 2110.04995 (Skellam); 2102.06387 (DDGauss); 1805.10559 (cpSGD); USPTO
  12526126 (binomial noise in MPC via Hamming weight of random bits, status unverified); eprint 2017/259, 2018/1234, 2019/511
  (constant-time lattice Gaussian samplers).

## KNOWN / CONJECTURE / ASSUMPTION
- KNOWN: binomial noise is already a standard distributed-DP and secure-aggregation noise (cpSGD 2018; Dahl-Pastro 2017; PRIME
  2024/1771; Dwork et al. 2006). The privacy accounting of binomial versus Gaussian already exists (PRIME lemma; PBM RDP for the
  Poisson-binomial variant). Coupling lattice noise with DP noise was published by Stevens et al., USENIX 2022.
- KNOWN: ML-KEM's CBD has eta in {2,3} (variance 1 or 1.5). Falcon's SamplerZ targets sigma of about 1.2-1.8 around arbitrary centres.
- CONJECTURE: DDGauss/Skellam per-client noise in integer grid units is large (std of tens to hundreds after scaling and
  quantisation). A binomial noise of variance s^2 then needs about 4*s^2 random bits per coordinate. Only the SHAKE/Keccak
  core is reusable, and a dedicated sampler would use that anyway. "Fraction of the cost" is therefore likely FALSE, or
  amounts to a flash saving of a few KB.
- CONJECTURE: the code saving is real but small, and the timing behaviour (constant-time) is the only distinctive property.
- ASSUMPTION: the M33 client already links ML-KEM (D4 setting), and DP is enforced client-side before masking.

## Verdict
PARTIAL OVERLAP. No opened paper reuses an ML-KEM CBD or Falcon sampler as the DP noise source, and none measures DP-noise
sampling cost on a Cortex-M class MCU. On those two points N2 is open as far as searched (15 queries). The binomial-vs-Gaussian
privacy penalty is already covered by known accounting (cpSGD, PRIME 2024/1771, PBM 2207.09916), so it is not a contribution.
Sharing noise between lattice crypto and DP is pre-empted conceptually by Stevens et al. (USENIX 2022, arXiv 2112.06872).
What remains is a narrow engineering measurement: the cycles, flash and constant-time cost of a DP sampler on the M33 when
Keccak is already present.

## Relation to D4 (collision-cand4-pq-secagg-mcu-cost.md)
Not the same question. D4 measures key-agreement and masking cost, while N2 measures noise sampling. Both share the device,
the setting and the ML-KEM dependency, so N2 fits naturally as a sub-measurement or section of D4 (the DP-noise line item
in the client cost budget). It is not strong enough to stand alone.

## Most damning reviewer objection
"The ML-KEM CBD samples variance-1 noise. DP noise needs variance in the thousands, so you are summing thousands of CBD
draws per coordinate, and the only thing reused is SHAKE, which any sampler would call anyway. The privacy analysis of
binomial noise is textbook (cpSGD, PRIME). What is new beyond a cycle count?"
