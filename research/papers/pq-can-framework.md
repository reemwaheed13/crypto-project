# PQ-CAN: A Framework for Simulating Post-Quantum Cryptography in Embedded Systems

Lane: A (application = vehicles, in-vehicle CAN)

## Citation
- Title: PQ-CAN: A Framework for Simulating Post-Quantum Cryptography in Embedded Systems
- Authors: Mauro Conti, Francesco Marchiori, Sebastiano Matarazzo, Marco Rubin (Dept. of Mathematics, University of Padova)
- ID: arXiv:2504.10730v1 [cs.CR], submitted 14 Apr 2025
- Status: arXiv preprint. The arXiv page names no peer-reviewed venue, and no published version was found in searches (as of Nov 2026).
- Code: github.com/spritz-group/PQ-CAN (as stated in the paper, repo not inspected)

## Reading depth
I read the full text through the arXiv HTML using a fetch-and-extract tool. The PDF binary could not be rendered here. The Table II numbers below were transcribed from the HTML extraction, so re-check them against the PDF before quoting them in a paper.

## Core claim
A Docker-based simulator with throttled CPU shares and a rate-limited virtual CAN bus can estimate the end-to-end overhead of PQC key exchange and signatures between ECUs. In that simulator, communication dominates the cost: Kyber512 is the cheapest KEM, and Falcon-512 gives the best balance of overhead and success rate among signatures.

## Platform (what was actually used)
- **No real ECU or MCU hardware, and no real CAN transceiver.** Everything ran on one PC: Intel Core i7-1065G7, 16 GB RAM, Arch Linux, kernel 6.12.10.
- Each ECU is a Docker container. Alice and Bob run the protocol, and Charlie injects background traffic.
- **Bus:** a virtual CAN link (vxcan), with bit rate shaped on the host by tc (iproute2). It is **classical CAN 2.0. CAN FD is not evaluated.**
- **Clock model:** each container's CPU share is set to target_freq / host_freq with Docker's `--cpus` flag. Two cores are pinned with taskset, and Hyper-Threading and Turbo Boost are disabled. This is a time-slice throttle of an x86 core, not a Cortex-M cycle model. It ignores differences in ISA, memory and cache, and the code uses AVX2.
- **ECU classes:** low (120 MHz, 125 kbps), mid (200 MHz, 500 kbps), and high (300 MHz, 1 Mbps). The clock values come from Microchip and Infineon MCU listings.
- **Background load:** 88% bus load, generated with canbusgen (can-utils).
- **Implementations:** the NIST C API, linked to an AVX2-optimized library. The paper does not name the library, so liboqs is not confirmed. Compiler flags are not reported in the extraction.
- **Schemes:**
  - KEMs: Kyber-512/768/1024, BIKE L1/L3/L5, HQC-128/192/256, and Classic McEliece. McEliece was dropped because of 8–49 s latency.
  - Signatures: Dilithium L2/L3/L5 (the round-3 names, not ML-DSA), Falcon-512/1024, and SPHINCS+-128f/192f in SHA2 and SHAKE variants. The "s" variants and Level 5 were excluded.
- **Flag:** all costs were measured on a PC. The MCU numbers are throttled-x86 estimates, and the paper gives no calibration against a real MCU.

## Threat model
- No formal threat model is given. The motivation is that CAN has no native authentication or encryption, and that quantum adversaries break classical public-key schemes.
- Charlie models congestion, not an attacker.
- The protected asset is implicitly session-key establishment (KEMs) and message authenticity (signatures) between two ECUs.
- Out of scope: active attacks on the bus, replay, real-time deadlines, side channels, and the cost of frame-level authentication of periodic traffic.

## What it does NOT consider (relevant to our candidate idea)
- **No amortised or batched authentication.** There are no Merkle roots over windows, hash chains, or signature aggregation. Every DSA run signs a single random 32-byte message.
- **No description of fragmentation or transport.** The extraction finds no mention of ISO-TP, segmentation, frames per signature, or key and signature sizes on the wire.
- **No CAN FD.** Only 8-byte classical CAN frames are modelled, inferred from "CAN 2.0" and the bit rates.
- **No deadline metric.** "Success rate" reflects receiver timeouts (2 s) and synchronization failures, not a control-loop deadline. It does not measure the bus-load impact on other traffic, and it does not measure loss tolerance.
- Each configuration was run 100 times, with outliers removed.

## Numbers that matter
Table I (KEMs, overhead from keygen to decaps):
- Kyber512: about 1.126 ms overhead (low config, success 0.59) and about 1.19 ms (high config, success 0.93).
- BIKE L5: about 1961.9 ms (low config, success 0.58).
- HQC-256: about 4921.8 ms (low config, success 0.72).
- Inconsistency: the RQ2 text says HQC-256 is "<250 ms", but Table I shows 598–4922 ms. This is probably a typo.

Table II (DSAs, ms, overhead = total minus nominal message time), transcribed from the HTML:

| Scheme | Config | Sign | Verify | Overhead | Success |
|---|---|---|---|---|---|
| Falcon-512 | high | 0.492 | 0.089 | 46.995 | 0.96 |
| Falcon-512 | low | 0.528 | 0.123 | 167.530 | 0.75 |
| Dilithium L2 | high | 0.144 | 0.117 | 61.275 | 0.28 |
| Dilithium L2 | low | 0.133 | 0.090 | 470.414 | 0.29 |
| Dilithium L3 | low | 0.241 | 0.143 | 821.569 | 0.50 |
| Dilithium L5 | low | 0.315 | 0.186 | 1131.983 | 0.43 |
| Falcon-1024 | low | 0.921 | 0.251 | 388.039 | 0.92 |
| SPHINCS+-128f SHA2 | low | 185.383 | 1.709 | 4078.748 | 0.26 |
| SPHINCS+-192f SHAKE | low | 230.640 | 2.125 | 9243.958 | 0.04 |

- Falcon keygen dominates its overhead: 139.9 ms (low) and 43.0 ms (high).
- Dilithium sign and verify take under 0.5 ms in this throttled model, yet its overhead is 61–1132 ms. This points to transmission time for the larger key and signature (my reading, CONJECTURE).
- Dilithium success rates are low (0.28–0.61). The paper does not fully explain why, and timeout/synchronization is the likely cause.
- Memory: not measured. Energy: not measured.

## Convenient assumptions
- CPU-share throttling of an AVX2 x86 core stands in for a 120–300 MHz MCU. This is optimistic for lattice schemes, and the ISA and memory gap is ignored.
- RAM and flash are unlimited, with no stack or code-size limits.
- The virtual bus has no real error frames, arbitration effects beyond tc shaping, or physical-layer loss.
- There is one sender and one receiver, and each run sends one message, with no periodic real-time traffic to authenticate.
- Key-generation cost is counted per run, so key reuse and pre-provisioned keys are not separated out.

## Stated limitations and future work (paraphrased)
- The authors want to validate on physical CAN networks, improve the underperforming implementations, and add more PQC schemes.
- Quote: "Future work should focus on real-world testing on physical CAN networks, improving underperforming implementations, and exploring other PQC schemes" (the extraction splits "CAN networks" and "PQC schemes" across lines).
- They note that communication overhead dominates and that success rates drop at lower clock speeds.

## Load-bearing references (as cited)
1. Lotto, Marchiori, Brighente, Conti (2024): survey and comparison of CAN authentication protocols, IEEE Communications Surveys & Tutorials.
2. Avatefipour and Malik (2018): survey of CAN security.
3. Liu, Ramachandran, Jurdak (2024): survey of PQC performance for IoT.
4. Fernández-Caramés (2019): survey of quantum-resistant IoT cryptosystems.
5. Kuznetsov et al. (2023): comparison of BIKE, HQC and Classic McEliece.

## Other 2025–2026 PQ-on-vehicle-bus papers seen (verified IDs only)
- **SAE 2025-01-0451**, "Post Quantum Cryptography on Intravehicular Networks." Smith, Owens, Kozan, NDIA GVSETS 2025. I saw the abstract only. It covers Kyber, Dilithium and Falcon, and the bus type (CAN or CAN FD) was not visible.
- **IACR ePrint 2026/2149** (Thales automotive PQC evaluation). I saw a search snippet only. It benchmarks ML-DSA, Falcon and others over TLS on a Raspberry Pi 4B. It is not a CAN or CAN FD benchmark.
- **Cultice and Thapliyal, "PUF-Based Post-Quantum CAN-FD Framework for Vehicular Security"** (MDPI Information 13(8):382, 2022). It is CAN FD but uses SIDH key exchange, which is now broken, and has no signatures. It predates the window.
- I found **no verified 2025–2026 paper that benchmarks ML-DSA, Falcon or SLH-DSA on CAN FD frame traffic**, as far as I searched.
  - Queries: "post-quantum signature CAN FD ML-DSA Falcon in-vehicle benchmark 2025"; "Post Quantum Cryptography on Intravehicular Networks SAE 2025-01-0451 CAN FD".
  - A Mitacs project page, "PQC integration in CAN FD/XL automotive networks", exists but is a project, not a paper.

## Possible seeds (direction-miner moves)
- **Change the bus:** move from CAN 2.0 to CAN FD with 64-byte payloads. PQ-CAN does not model CAN FD or frame counts per signature.
- **Change the unit of authentication:** replace per-message signatures with amortised authentication (a Merkle root or hash chain over a window of frames, or one signature per window). This is absent in PQ-CAN, so the trade-off between latency, bus load and loss tolerance is open as far as searched.
- **Fix the metric:** replace the timeout-based "success rate" with deadline-miss rate for periodic control frames, plus the bus-load impact on other IDs.
- **Fix the platform model:** replace CPU-share throttling with Cortex-M4 cycle counts (pqm4 numbers or an emulator) calibrated to a real core. That is the step to the EFR32xG24 in phase 2.
- **Separate one-time from steady-state costs:** keygen and certificate exchange at startup versus per-window cost. PQ-CAN folds Falcon keygen into every run.
- **Phase 2 side channel:** Falcon signing on the ECU, where floating-point sampling is a known leakage concern, versus hash-based window signing.

## Full-text check (2026-11-10, local PDF `2504.10730v1.pdf`, 6 pages)
Reading depth is now **full text (PDF)**. Table II (p. 5) was re-checked against the PDF, and every Sign, Verify, Overhead and Success value transcribed above matches. One addition: Table II has a **Key Generation** column, and for Falcon-512 it is 43.0 / 76.4 / 139.9 ms (high/mid/low). That accounts for almost all of Falcon-512's 47.0 / 90.8 / 167.5 ms "overhead", while signing is 0.49–0.61 ms. **Do not cite "Falcon-512 overhead 47–167.5 ms" as a signing or transport cost:** it is mostly key generation repeated in every run. CAN FD, ISO-TP, Merkle and amortisation do not appear anywhere in the PDF. The only "Merkle" mention describes SPHINCS+ itself (p. 2).
