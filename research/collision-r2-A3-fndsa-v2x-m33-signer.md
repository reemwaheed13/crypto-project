# Collision check, round 2, A3: FN-DSA signing at V2X message rate on a Cortex-M33-class signer

Date: 2026-11-05. Checker: collision subagent. Lane A (PQC).

## Candidate
"Can a Cortex-M33-class V2X unit sign with FN-DSA at message rate, in constant time?" The candidate compares three options on an M33-class signer: (a) emulated-double integer Falcon (Pornin), (b) fixed-point FN-DSA (ePrint 2026/2431), (c) triple-word single-precision FP (TWFalcon, TCHES 2026). It claims that the option sustaining at least 10 signatures/s within an OBU RAM budget differs from the M4 winner, and/or that no option meets the 10 Hz + RAM budget without precomputation.
Threat model: the asset is BSM authenticity and the signing key. The adversaries are an active network attacker now and a quantum forger later, plus a local timing observer. Power and EM are phase 2.

## Verdict: PARTIAL OVERLAP (open as far as searched for the combined question; parts are already settled or weak)

## Queries run (all 2026-11-05)
1. `Falcon signature Cortex-M33 constant-time signing performance` (standard, technical)
2. `post-quantum signatures V2X onboard unit microcontroller signing rate 10 Hz Falcon` (standard, plain-language/application)
3. `eprint.iacr.org Falcon FN-DSA Cortex-M33 implementation 2026` (standard, ePrint-focused, 2026)
4. `Falcon FN-DSA signing ARMv8-M single-precision FPU emulated double fixed-point comparison microcontroller` (extended, technical)
5. `post-quantum V2X embedded automotive ECU Falcon signing latency measurement AURIX Cortex-M 2025 2026` (standard, neighbouring community: automotive/embedded, date-restricted)
6. `Falcon signature STM32 Cortex-M33 benchmark cycles signing RAM TrustZone` (standard, embedded)
7. `roadside unit sensor broadcast post-quantum signature microcontroller signing throughput Falcon 2026` (extended, networking/IoT, date-restricted)
8. `"Falcon" "Cortex-M33" post-quantum signature 2025` (standard, exact-phrase, date-restricted)
9. `Falcon offline online signing precomputation V2X vehicle signing rate constrained Twardokus post-quantum V2V authentication` (standard, networking/V2X)
10. `FN-DSA ARMv8-M Cortex-M33 Cortex-M35P fixed-point triple-word Falcon signing evaluation 2026 ePrint` (extended, ePrint, 2026)

## Papers / items opened
| ID | Title (short) | Verified | Depth | Same claim? | Same device? | Same threat model? |
|---|---|---|---|---|---|---|
| ePrint 2026/2431 | Halmans, Kumar, Güneysu, "Flying, Not Floating" fixed-point FN-DSA | Landing page verified (prior note); PDF 403 again | Abstract + existing note research/papers/fixedpoint-fndsa-m4.md | Partly: fixed-point vs emulated double, 45% fewer cycles | No: M4 only, no M33 found | Partly: constant-time; no rate or application |
| ePrint 2026/1915 | Pornin, "A RAM-Efficient Implementation of Falcon" | Landing page verified | Abstract | No: RAM 31 to 11 kB, 13.45M cycles signing on M4 | No: M4 only | No application |
| ePrint 2026/2559 | Lee, Sim, Kim, Seo, "FN-DSA on Arm Cortex-M55" (native FP64 + Helium) | Landing page verified | Abstract | No: M55 with FP64, relative speedups only (sign 1.93x vs ref) | No: M55, not M33 | Constant-time claimed; no application rate |
| TCHES 2026 artifact a27 | Halmans, van Vredendaal, Schneider, Custers, Güneysu, TWFalcon | Artifact page verified | Artifact page | Partly: triple-word SP-FP, 1.84x vs emulated double | No: M4F (Nucleo-L4R5ZI) | Precision/security of sampler; no rate |
| arXiv 2608.05087 | Abrar et al., PQ signatures in C-V2X | Verified (prior note) | Full text (prior note research/papers/pq-v2x-falcon-cv2x.md) | Application match; explicitly treats signing cost as negligible, unmeasured | PC/liboqs only | Network-level only; no side channels |
| ePrint 2026/2149 | Kushwaha, Kaur (Thales), PQC for automotive (secure boot, OTA, V2X) | Landing page verified | Abstract | No: Raspberry Pi 4B / i5, TLS; no MCU, no rate vs 10 Hz | No | No |
| ePrint 2026/1610 | Houlès, Heckmann, Gaussian sampler fpr_expm_p63 optimisation | Landing page verified | Abstract | No: one sampler kernel, 1.28x on QEMU Cortex-M3 | M3 emulated, not M33 | dudect constant-time; no application |
| wolfSSL blog (Aug 2026, vendor) | wolfCrypt native Falcon | Page verified | Full page | Only M33 data point found: STM32H563 M33 @250 MHz, Falcon-512 "comparable to liboqs". No numbers | M33, yes | No rate, no constant-time evaluation |

Seen in snippets only, not opened: Howe and Westerbaan ePrint 2022/405 (M7, constant-time irregularities); Sinell et al. 2025 (AURIX TC399); Twardokus et al. NDSS 2024 (V2V PQ authentication, network level); ETRI Falcon-verify on M4 for UAV (verification only); a 2026 secure-boot PQ review on ResearchGate; SAE 2026-26-0622 (two-wheeler PQC).

## What is covered (KNOWN)
- On the M4, fixed-point beats emulated double by about 45% (2026/2431). Triple-word SP-FP costs about 1.84x emulated double (TWFalcon). Pornin's RAM-efficient signer runs at about 11 kB and 13.45M cycles (2026/1915). So the M4 "winner" ranking is already published.
- C-V2X frame fit for Falcon-512 is covered by arXiv 2608.05087, which leaves signing cost unmeasured.
- A search snippet from a 2026 ePrint (not opened) says NIST plans to specify FN-DSA signing in 64.64 fixed-point. If true, option (c) triple-word FP may become non-conformant, which weakens the three-way framing.

## What remains open (as far as searched)
- No paper found measures or estimates Falcon/FN-DSA signing on Cortex-M33 / ARMv8-M with any of the three arithmetic options. No paper found sets signing throughput, its rejection-loop tail latency, and RAM against a 10 Hz BSM / OBU budget on an MCU.
- No paper found combines Pornin's 11 kB layout with fixed-point precision and evaluates the result on M33.

## Weaknesses that could make A3 a minor variant (CONJECTURE)
- M33 and M4 share a 3-stage pipeline, the DSP extension (UMULL/UMLAL) and a single-precision-only FPU. The claim that the M33 winner "differs from the M4 winner" is unlikely to hold. If the ranking is the same, the result reduces to a port.
- The "none meets 10 Hz" half can be largely derived on paper. At the EFR32xG24's 78 MHz, 13.45M cycles is about 172 ms, or about 5.8 signatures/s, assuming M4-equal cycle counts. Reviewers may see this as back-of-envelope arithmetic unless it adds measured tail latency, RAM, energy and constant-time evidence.
- A3 is NOT a minor variant of the excluded items. ePrint 2026/635 covers verify-side, low-stack firmware/OTA verification with HAETAE/HAWK. The Thread item is about handshake energy. A3 is about signing at a sustained message rate, with a float-precision constraint.
