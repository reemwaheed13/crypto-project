# Project context

- Course: CS 5803 Applied Cryptography, LUMS, Fall 2026. Software team (no hardware required).
- Groups finalised 20 Oct 2026. Final deliverable plus a possible 5–10 minute demo in vacant slots.
- Instructor encourages LLM-assisted gap finding and novel, publishable work.
- Allowed primitive families: post-quantum crypto, FHE, federated learning privacy.
- Target device for cost estimates: a "typical IoT MCU". Lab kit available later for DRP phase 2:
  Silicon Labs EFR32xG24 dev kit (Arm Cortex-M33 class, wireless SoC). VERIFY clock, flash, RAM,
  active current per MHz and radio TX current from the official datasheet before using them.
- Phase-2 directions the instructor already hinted at: side-channel attacks on our own implementation.

## Current state
- Round 1 done 2026-11-05: `research/direction-mining-2026-11-05.md`. 14 papers read (`research/papers/`),
  6 candidates collision-checked (`research/collision-cand*.md`). All 6 came back Partial overlap.
- Top 3 by score, no direction committed yet:
  - **D3 (recommended)** — HE-friendly cipher *client* cost + transciphering-vs-direct-HE crossover on
    Cortex-M33. Collision: Open as far as searched; cleanest + most feasible; strong SCA DRP.
  - **D2** — on-device cost of defeating gradient-*inversion* reconstruction on low-dim sensor FL.
    Partial overlap (distinct from membership-inference MDPI 10.3390/make8100320).
  - **D1** — compute-vs-radio energy split of PQ EDHOC/DTLS on M33/Thread + compact-KEM crossover.
    Partial overlap; "radio dominates" already published (arXiv 2602.18708, 2610.13026) so novelty capped.
- Dropped: D5 firmware signatures (stack crossover already done, ePrint 2026/635). Deprioritised: D4 PQ
  secure-agg MCU cost (feasibility), D6 small-N SA leakage (pre-empted by Elkordy PoPETs 2023).
- Round 2 done 2026-11-05 (novelty-focused, two lanes): `research/direction-mining-round2-2026-11-05.md`.
  10 more papers read; 8 candidates collision-checked (`research/collision-r2-*.md`), all Partial overlap or worse.
  - Lane A survivors: **A4** amortised PQ source authentication of CAN FD frame streams (total 26, ranks 3rd overall
    behind D3/D2); **A3** FN-DSA signing at 10 Hz on an M33-class V2X unit (24, best DRP fit).
  - Lane B survivors (NEED INSTRUCTOR APPROVAL): **B3** MLS/TreeKEM post-compromise recovery vs duty cycle (23);
    **B4** BBS credentials on M33 with secure-element ECDSA binding (23).
  - Dropped: A1 CKKS actuator oracle (killed by ePrint 2026/2490), A2 federated-unlearning leakage (minor variant
    of D2), B1 leveled Ascon (ePrint 2024/755), B2 FROST nonce state (minor variant of LMS state work).
- Carry-forward before publishing: read the 403-blocked full texts listed in the report's §5.3; VERIFY all
  EFR32xG24 datasheet electrical numbers before any energy estimate.
