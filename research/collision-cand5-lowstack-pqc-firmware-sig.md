# Collision check — Candidate 5: Low-stack PQC signatures for firmware/OTA-update auth on memory-constrained MCUs

Date: 2026-11-05
Checker: collision-check agent

## Candidate claim (verbatim)
On memory-constrained MCUs (small usable stack, e.g. <=16-32 KB), a low-stack lattice signature
(HAETAE or HAWK) enables firmware/OTA-update signature verification (and/or signing) where ML-DSA
does not fit, and the time/energy/signature-size cost for OTA-update authentication is quantifiable
and acceptable.

## Queries run (verbatim)
1. (technical) `low-stack HAETAE HAWK lattice signature memory-constrained microcontroller Cortex-M4 firmware verification`
2. (plain-language) `post-quantum signature OTA firmware update authentication constrained IoT MCU stack usage`
3. (ePrint-only) `HAETAE low-stack microcontroller signing verification stack crossover ML-DSA` [allowed_domains=eprint.iacr.org]
4. (neighbouring: embedded security / secure boot / SUIT) `post-quantum secure boot firmware signature LMS XMSS SPHINCS+ microcontroller IETF SUIT`
5. (HAWK angle) `HAWK signature embedded microcontroller low memory stack IoT firmware authentication energy`
6. (2026-recency) `post-quantum signature energy measurement Cortex-M firmware update HAETAE HAWK 2026`
7. (ePrint-only, energy+OTA) `lattice post-quantum signature OTA firmware authentication energy cost measurement constrained device 2025 2026` [allowed_domains=eprint.iacr.org]
8. (hash-based reframe) `why not hash-based signatures stateful LMS firmware signing lattice alternative small signature IoT tradeoff`

## Papers opened / verified
- ePrint 2026/635 = arXiv 2604.15868 "Low-Stack HAETAE for Memory-Constrained Microcontrollers" — VERIFIED, abstract+metadata read.
  - Gives the stack crossover: HAETAE verify fits <8 KB at all levels; signing 5.8-6.0 KB; keygen 4.7-5.7 KB. Verify 2.34-3.34x faster than ML-DSA m4fstack. Platforms: Nucleo-L4R5ZI, RIOT-OS on Cortex-M4 and RISC-V, ATmega2560 (8,070 B SRAM full round-trip). Targets 8-16 KB SRAM.
  - Does NOT frame firmware/OTA auth as the application. Does NOT measure energy. Does NOT compare to hash-based sigs.
- ePrint 2026/2578 "HAETAE on a Diet: Less Stack, Less Time on Low-End Microcontrollers" — VERIFIED exists (PDF 403, abstract via search). Further stack+time reduction, Cortex-M focus. No firmware/OTA framing or energy evident from abstract.
- ePrint 2026/2066 "Optimizing HAETAE and SMAUG-T on Cortex-M4 for Resource-Constrained IoT Devices" — VERIFIED, abstract read.
  - DOES estimate energy, transmission-unit count, end-to-end latency using models anchored to prior IoT protocol measurements. Frames as "constrained wireless environments" (KEM+sig transmission), NOT firmware/OTA auth, NOT secure boot. No stack-crossover-vs-ML-DSA memory analysis.
- ePrint 2026/2149 "Identify PQC Algorithms for Automotive Security: ... Secure Boot, OTA, and V2X" — VERIFIED, abstract read.
  - Covers secure boot / firmware signing / OTA framing explicitly, but automotive domain, platform = Raspberry Pi 4B (NOT a <16 KB-stack MCU). Schemes: CROSS, MAYO, SPHINCS+, ML-DSA, FALCON. Does NOT include HAETAE or HAWK; does NOT do the low-stack MCU crossover.
- ePrint 2026/2557 "Energy Consumption of PQC on Constrained and General-Purpose Architectures" — VERIFIED exists. Benchmarks ML-KEM/ML-DSA/SLH-DSA energy on Raspberry Pi class, not <16 KB MCU, not HAETAE/HAWK, not OTA framing.
- ePrint 2022/1198 "To Be, or Not to Be Stateful: Post-Quantum Secure Boot using Hash-Based Signatures" — VERIFIED exists. Occupies the firmware/secure-boot niche with LMS/XMSS/SPHINCS+ (hash-based), not lattice low-stack.
- eprint 2021/781 / arXiv 2106.05577 "Quantum-Resistant Software Update Security on Low-power Networked Embedded Devices" — VERIFIED exists. SUIT-style software update, hash-based/lattice on low-power nodes.

## Analysis
The STACK CROSSOVER is already published (2026/635, and strengthened by 2026/2578): low-stack HAETAE
verify/sign fit where ML-DSA-87 signing does not, on Cortex-M4/RISC-V/AVR. HAWK already has published
low-memory (6-11 KiB) and acceleration results (2026/1078) and is routinely cited for firmware
update / secure boot / attestation as a use case. So the core "enables verify/sign where ML-DSA
doesn't fit" claim is NOT novel.

Remaining pieces of the candidate:
- Energy on-device: 2026/2066 already estimates energy + latency + transmission units for HAETAE
  (and SMAUG-T) on Cortex-M4, though framed as wireless KEM/sig transmission rather than OTA auth.
  2026/2557 covers PQC energy broadly. So the "energy is unmeasured" gap is largely closed for HAETAE;
  what is thin is energy specifically under a low-stack/streamed build and specifically a HAWK vs
  HAETAE vs hash-based comparison in the firmware-auth setting.
- OTA-auth framing: 2026/2149 already frames secure boot/OTA for PQC (but Pi-class, no HAETAE/HAWK,
  no stack crossover). No single paper found that (a) uses low-stack HAETAE/HAWK, (b) on a true
  <16 KB-stack MCU, (c) under an explicit firmware/OTA-update authentication threat model, AND
  (d) reports energy + a head-to-head vs hash-based (LMS/XMSS/SPHINCS+) firmware-signature baselines.

Hash-based reframe risk is real: LMS/XMSS (SP 800-208, CNSA 2.0) are the established PQC
firmware-signature choice, verify is cheap and low-stack, and 2022/1198 already occupies secure boot.
The candidate must justify lattice over hash-based (e.g. stateless signing without state management;
smaller signatures than SPHINCS+; non-stateful OTA from many signers) — this is an argument the
candidate has to win, not an open research gap per se.

## Verdict
**Partial overlap.**
The stack crossover (ML-DSA does-not-fit / low-stack HAETAE does) is CLOSED by ePrint 2026/635 and
2026/2578; HAETAE energy on Cortex-M4 is largely CLOSED by 2026/2066; generic PQC secure-boot/OTA
framing is CLOSED by 2026/2149 (Pi-class). What remains, as far as searched, and only thinly:
an explicit firmware/OTA-update threat model on a genuine <=16-32 KB-stack MCU that (1) uses the
low-stack HAETAE/HAWK builds, (2) reports energy under the streamed/low-stack build, and (3) does a
direct cost comparison against the incumbent hash-based firmware-signature choices (LMS/XMSS/SPHINCS+)
including state-management burden. This residual is narrow and heavily scaffolded by existing work,
i.e. incremental — fine as a measurement/systematization software project, weak as a novel-question DRP.
No single paper collides on all four axes (low-stack lattice + sub-16KB MCU + OTA-auth threat model +
energy with hash-based baseline), so it is not a full collision.
