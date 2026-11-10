# Collision log: N3 "Is secure randomness the bottleneck for PQC, FHE and FL privacy on a Cortex-M?"

Date: 2026-11-10. Checker: collision-check subagent. Verdict: **PARTIAL OVERLAP (open as far as searched for the cross-primitive MCU budget; individual legs are partly covered).**

## Candidate (as given)
- Application: EFR32MG24 (Cortex-M33, Secure Vault TRNG, hardware AES) running ML-KEM/ML-DSA (incl. masked), BFV/CKKS client encryption (N=4096, ternary secret + error), DP-SGD noise.
- Asset: secrets / private data protected by randomness quality (keys, masks, FHE secret/error, DP noise).
- Adversary: not stated precisely by candidate (implicitly: anyone exploiting weak or under-provisioned randomness; for DP, the aggregator / gradient-inversion attacker).
- Measured: random bytes per operation; cycles for TRNG, DRBG (CTR_DRBG on HW AES, ChaCha20, SHAKE), sampler; share of total cost; DRBG ranking on MCU vs desktop.

## Queries run (all via WebSearch)
1. `TRNG throughput Cortex-M post-quantum` (standard)
2. `randomness cost masked ML-KEM Cortex-M` (standard)
3. `DRBG benchmark microcontroller` (standard)
4. `random number generation bottleneck homomorphic encryption client` (standard)
5. `entropy budget IoT cryptography` (standard)
6. `EFR32 TRNG throughput` (standard)
7. `masked Kyber Cortex-M4 random bytes TRNG cycles fraction of decapsulation eprint` (standard, ePrint-focused)
8. `SEAL-Embedded homomorphic encryption Cortex-M4 sampling PRNG cost breakdown` (standard)
9. `differential privacy Gaussian noise generation microcontroller cost federated learning on-device DP-SGD MCU` (standard, ML community)
10. `randomness generation overhead masked post-quantum implementation microcontroller 2025` (extended, date-restricted)
11. `Bos Gourjon Renes Schneider van Vredendaal Masking Kyber first- and higher-order implementations randomness TRNG cycles` (standard)
12. `"randomness" "bottleneck" masked lattice implementation Cortex-M4 TRNG slow random bytes Saber Kyber higher-order` (standard)
13. `eprint 2025 2026 lattice FHE encryption microcontroller sampling error ternary secret PRNG SHAKE dominant cost client` (extended, date-restricted)
14. `Gaussian noise sampling cost differential privacy microcontroller TinyML on-device training random number generator energy` (standard, ML/embedded)
15. `Silicon Labs EFR32xG24 Secure Engine TRNG entropy rate bits per second reference manual` (standard, embedded)
16. `dithered Gaussian mechanism randomness-efficient differential privacy random bits arXiv` (standard, 2026)
17. `secure aggregation federated learning microcontroller PRG mask expansion cost Cortex-M ChaCha AES` (standard, networking/FL)
18. `pqm4 randombytes cost ML-DSA signing randomness keccak share cycles Cortex-M4 hashing percentage` (standard)
19. `CKKS BFV client encryption microcontroller ARM Cortex-M profiling breakdown sampling NTT 2025 2026` (extended, date-restricted)
20. `random number generator cost comparison PQC FHE differential privacy embedded device 2026 randomness consumption` (standard, 2026)

## Papers / sources opened
| ID | Title | Verified | Depth | Relevance |
|---|---|---|---|---|
| arXiv 2311.08040 | Kundu, Karmakar, Verbauwhede, "On the Masking-Friendly Designs for Post-Quantum Cryptography" | Yes (arXiv abs + HTML) | Full-text tables (Tables 7, 8, TRNG paragraph) via HTML | Counts random bytes for masked Kyber/Saber/Scabbard at orders 1-3 (e.g. Kyber 2nd/3rd order ~0.9 MB / ~2.4 MB per decaps per their Table 8 citing [9]); STM32F407 TRNG 32-bit word per ~20 system cycles. Does NOT give randomness share of cycles. |
| ePrint 2022/058 | Heinz et al., "First-Order Masked Kyber on ARM Cortex-M4" | Yes (ePrint landing via search) | Abstract/snippet only (PDF 403) | Cycle counts include RNG cost; no breakdown seen. |
| ePrint 2021/483 / TCHES 2021(4) | Bos et al., "Masking Kyber: First- and Higher-Order Implementations" | Yes (dblp) | Abstract/snippet only (TCHES page bot-blocked) | Higher-order masked Kyber random-byte counts (cited in 2311.08040 Table 8). |
| arXiv 2607.06320 | Kalinin, Pagh, "Dithered Gaussian Mechanism for Randomness-Efficient Differential Privacy" (Jul 2026) | Yes (arXiv abs) | Abstract only | Directly targets the cost of cryptographically secure randomness for DP-SGD noise; claims random-bit count independent of noise level. No MCU/IoT/TRNG mention in abstract. Threatens the DP leg. |
| arXiv 2603.19340 | ML-KEM/ML-DSA on RP2040 (Cortex-M0+) benchmark | Yes (arXiv HTML) | First ~100k chars | Times RNG at 32/64 B, argues RNG does not affect timing; no RNG share figure. |
| arXiv 1910.07557 | Banerjee et al., Sapphire crypto-processor | Yes (search snippet; PDF not parseable) | Snippet only | States sampling is a major portion of software lattice cost; compares SHAKE/ChaCha20/AES-CTR PRNG energy (hardware). |
| Seen in snippets only (not opened): arXiv 2507.00367 Presto (RNG decoupling in HHE client hardware); arXiv 2602.19550 (FHE accelerators: seed-expanded uniform polys, sampling <3% client overhead in HW); arXiv 2009.03777 Garfinkel & Leclerc "Randomness Concerns When Deploying DP" (90 TB random bytes for 2020 Census); TCHES SEAL-Embedded (Natarajan & Dai 2021, total encrypt time only in snippet); COSADE 2022 Beckers slides (misconfigured TRNG polling on STM32F4 in masked AES benchmarks). |

## KNOWN (cited)
- Masked lattice KEMs consume tens of KB to MB of randomness per decapsulation, rising steeply with order (2311.08040 Table 7-8; Bos et al. 2021).
- STM32F407 TRNG: one 32-bit word per ~40 TRNG clocks (~20 core cycles at 24 MHz) (2311.08040).
- Published masked-Kyber cycle counts include RNG cost (2022/058), and TRNG polling misconfiguration has distorted masked benchmarks (COSADE 2022 slides, snippet).
- Randomness cost of DP noise is an active 2026 topic for DP-SGD (2607.06320), and DP deployments need large randomness volumes (2009.03777).
- FHE libraries/accelerators expand randomness from seeds with a PRG (BLAKE2, SHAKE etc.); HW accelerators report sampling overhead small (2602.19550, snippet).

## CONJECTURE (ours, untested)
- On an M33 with HW AES, CTR_DRBG on the AES peripheral beats SHAKE/ChaCha20 per byte, reversing desktop advice (ChaCha20 often preferred on CPUs without AES-NI; on desktops with AES-NI, AES-CTR already wins, so the "reversal" may be weak).
- For BFV/CKKS client encryption at N=4096 and DP-SGD noise, randomness generation + sampling is a large share of client cost on a Cortex-M.

## ASSUMPTION
- EFR32MG24 SE TRNG throughput is "slow" -- NOT verified; no public figure found (query 6, 15). NIST CMVP entropy certificate E287 (Silicon Labs "SixG301x") surfaced but unread.
- Search-snippet summaries for unopened papers are accurate.

## What remains open (as far as searched)
- No paper found that puts PQC (masked/unmasked), FHE client encryption, and DP-SGD noise on one Cortex-M33 and attributes cycles/energy to TRNG vs DRBG vs sampler under a single randomness budget.
- No MCU DRBG ranking (HW-AES CTR_DRBG vs ChaCha20 vs SHAKE vs TRNG-direct) tied to these workloads found.

## Most damning reviewer objection
"This is a benchmark note, not research." Unmasked: Keccak dominance in ML-KEM/ML-DSA is already reported in pqm4 profiling; FHE and DP both seed a fast PRG, so the TRNG is used only for a 32-byte seed and is never the bottleneck by design; the remaining cost is just symmetric-primitive throughput on the AES/Keccak path, which is datasheet-predictable. Masked randomness volume is already tabulated (2311.08040). DP randomness efficiency is being solved algorithmically (2607.06320). Secondary objection: no adversary/threat model -- what attack does a cheaper DRBG enable or prevent?
