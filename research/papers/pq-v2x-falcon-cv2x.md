# Deployment Feasibility Analysis of Post-Quantum Digital Signatures in Safety-Critical C-V2X Communication for Urban Mobility Scenario

Lane: A

## Citation
- Title: Deployment Feasibility Analysis of Post-Quantum Digital Signatures in Safety-Critical C-V2X Communication for Urban Mobility Scenario
- Authors (arXiv abstract page): Akid Abrar, Sagar Dasgupta, Abdullah Al Mamun, Minhaj Uddin Ahmad, Mizanur Rahman, Mashrur Chowdhury, Ahmad Alsharif
  - Note: a search-engine snippet listed only the first five names. The arXiv abstract page lists seven, and that is the list used here.
- ID: arXiv:2608.05087v1, submitted 5 Aug 2026, category cs.PF
- Status: arXiv-only. The abstract page has no comments or journal-ref field, and no venue was found.
- Links: https://arxiv.org/abs/2608.05087 , https://arxiv.org/html/2608.05087

## Reading depth
Full text (arXiv HTML v1), read through tool-assisted extraction across the whole body and the tail of the reference list. Reference entries [1]–[35] were seen only as in-text citations, not as full entries.

## Core claim
Of the NIST signatures, only Falcon-512 fits an SAE J3161 / IEEE 1609.2 C-V2X transport block. Even Falcon meets the 90% packet delivery ratio (PDR) only under light traffic with line of sight, so spectrum, not crypto compute time, is the binding constraint.

## Platform (what was actually used)
- **Everything ran in simulation on a PC or server. No embedded hardware was used.** The paper does not state the host CPU, OS, or compiler flags.
- Network: OpenCV2X ([26], a SimuLTE/OMNeT++ extension that models LTE PHY, MAC with semi-persistent scheduling (SPS), RLC and PDCP), coupled with Veins and SUMO (via TraCI). ns-3 was not used.
- Crypto: liboqs for the PQC schemes and OpenSSL for ECDSA P-256. Each vehicle and the roadside unit (RSU) generates a key pair at initialization, and every Basic Safety Message (BSM) is really signed and verified inside the simulation, using the liboqs reference library (Sec. 5.4, 6.2).
- Parameter sets: ECDSA P-256 and Falcon-512 in the simulation. ML-DSA-44 (Dilithium-2) and SLH-DSA-SHA2-128s (SPHINCS+) were size-checked only.
- Section 2 states that no commercial onboard unit (OBU) or RSU currently supports PQC primitives in production firmware.

## Signing and verification cost (what the caller asked about)
- **The paper reports no per-message signing or verification times, measured or taken from the literature.**
- Sec. 5.7 excludes crypto time from latency by design: "Signature generation at the sender and signature verification at the receiver occur outside the measured interval".
- Sec. 6.2 says Falcon-512's sign/verify overhead via liboqs "contributes negligibly to end-to-end delay". No supporting number is given.
- **Signing Falcon on a constrained Cortex-M-class signer at 10 Hz BSM rates is not costed.** Sign and verify are treated as free.
- Falcon floating point is not discussed as a risk. The topic appears only in related work: [18] Howe and Westerbaan benchmarked Dilithium and Falcon on Cortex-M7 and found Falcon signing 6–8x faster, which the paper attributes to the native 64-bit FPU. The paper does not discuss FPU-less cores such as Cortex-M0, M3, or M4 without double precision.
- Related work [38] Sinell et al. ran Falcon on the Infineon AURIX TC399: verification met real-time V2X needs, and key generation showed high latency variance.
- Side-channel and constant-time: the paper does not mention either.

## Threat model
- Not formally defined. The motivation is signature forgery by a future quantum adversary (Shor) and harvest-now-decrypt-later attacks (Sec. 2). The paper also mentions the privacy of location, speed, and trajectory.
- Protected asset: authenticity and integrity of BSMs (IEEE 1609.2 secured protocol data units, SPDUs).
- Out of scope: adversary modeling in the simulation, side channels, implementation attacks, and the security of certificate management.

## Numbers that matter
- Table 1, raw sizes in bytes (signature / public key):
  - ECDSA P-256: 64 / 65
  - Falcon-512: 666 / 897
  - ML-DSA-44: 2,420 / 1,312
  - SLH-DSA-SHA2-128s: 7,856 / 32
- Table 2, SPDU size with a full certificate (bytes):
  - ECDSA: 301
  - Falcon-512: 1,735
  - Dilithium-2: 3,904
  - SPHINCS+: 8,060
- SPDU size in digest mode (bytes):
  - ECDSA: 139
  - Falcon-512: 741
  - Dilithium-2: 2,495
- The transport block size (TBS) ceiling is 2,481 B (19,848 bits) at MCS 11 with 10 subchannels and 98 PSSCH PRBs. Dilithium-2 exceeds it even in digest mode.
  - Allowed low-speed MCS under J3161: 5, 6, 7, 11.
  - BSM core: 39 B. IEEE 1609.2 header: 28 B.
- Table 3, subchannels needed at MCS 11 (certificate / digest):
  - ECDSA: 2 / 1
  - Falcon: 7 / 4
  - Dilithium-2: N/A
  - Internal inconsistency: the Falcon certificate SPDU is 1,735 B in Table 2 but 1,739 B in Table 3.
- WSM 1400 B limit: the paper does not mention it. The limit it uses is the LTE transport block, not the WAVE Short Message (WSM) MTU.
- Setup (Tables 4–5):
  - 10 Hz BSMs (RRI 100 ms), full certificate every 5th BSM, HARQ disabled
  - 5.9 GHz, 20 MHz, 23 dBm
  - Densities of 10–100 veh/km across traffic levels of service (LOS) A–F
  - Line-of-sight (dual-slope) and non-line-of-sight (ITU UMi) channels
  - 24 scenarios of 300 s each, at a single 4-leg intersection
- PDR (Sec. 7.1, Fig. 3):
  - ECDSA under line of sight: 94.89% (A), 95.40% (B), 95.02% (C)
  - Falcon-512 under line of sight: 92.76% (A), 79.86% (B), and about 73% near the source at C, never reaching 90%
  - Non-line-of-sight: neither scheme reaches 90% at any distance.
- Latency (Table 6, excludes crypto):
  - Mean: 49.76–52.66 ms
  - P95: 96–98 ms
  - Max: up to 101 ms for ECDSA and up to 199 ms for Falcon. The Falcon tail is "double-cycle queuing" and affects fewer than 1% of packets.

## Convenient assumptions
- Crypto compute time is zero, or "negligible", and is unmeasured. The host is a PC, not an OBU, RSU, or MCU.
- No energy or memory cost is reported for signing.
- HARQ is disabled. There is a single intersection, and only the RSU observes (V2V receptions are not logged).
- The fixed certificate policy (1 in 5) is not optimized.
- OpenCV2X lacks SPS+One-Shot and the REQ-J3161-SPS_004 subchannel constraint. Under strict compliance, Falcon's 7-subchannel point would round up to 10 (Sec. 8).

## Stated limitations and future work (paraphrased)
- Limitations: one geometry, an unoptimized crypto library (they state liboqs "is not optimized for embedded automotive hardware"), and simulator gaps against J3161.
- Future work:
  - NR-V2X (Rel. 16/17)
  - Hybrid ECDSA/Falcon switching driven by congestion
  - Higher-order MCS / QAM
  - Multi-intersection corridors
  - Hardware profiling on automotive-grade platforms
  - Fragmentation across subframes, as in [44]

## Load-bearing references
- [26] McCarthy and O'Driscoll 2019, OpenCV2X (the simulator)
- [18] Howe and Westerbaan, Falcon/Dilithium on Cortex-M7 (the FPU-dependent signing speed)
- [38] Sinell, Tschirley, Jungklass 2025, "Evaluation of post-quantum cryptography signature scheme on an automotive multicore microcontroller for real-time application", FICC, pp. 454–466 (AURIX TC399)
- [43] Twardokus, Bindel, Rahbari, McCarthy, NDSS 2024, "When cryptography needs a hand: practical post-quantum authentication for V2V communications"
- [44] Twardokus and Rahbari 2025, cross-layer quantum-resistant C-V2X authentication (TechRxiv preprint)
- Standards: SAE J3161/1, IEEE 1609.2, SAE J2945/1

## Possible seeds (CONJECTURE, ours)
- **Fill the cost hole:** cost Falcon-512 signing at 10 Hz (plus verifying N neighbours' BSMs) on an FPU-less or single-precision Cortex-M33 (EFR32xG24 class), using emulated integer floating point. This paper assumes the cost is negligible without measuring it.
- **Swap the platform:** move the signer from a PC or OBU to a roadside sensor MCU, and report cycles, stack and RAM, and energy per signed BSM against the duty-cycle budget.
- **Add the missing threat:** side-channel and constant-time behaviour of the Falcon signing Gaussian sampler on M33. This is phase 2 of the DRP. The paper does not mention side channels at all.
- **Combine:** feed measured signing and verification latency, including tail variance (cf. the key-generation variance in [38]), into the 100 ms deadline model. Does the crypto tail plus the 199 ms queuing tail break J2945/1?
- **Check verification load:** the receiver must verify many 666 B signatures per 100 ms at high density. Its budget is unquantified here.
- Caveat: check [38], [18], and Sinell-like automotive MCU studies before claiming novelty. The gap is open only as far as this paper's related work shows.
