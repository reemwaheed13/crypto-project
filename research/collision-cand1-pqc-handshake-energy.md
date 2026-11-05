# Collision check — Candidate 1

**Date:** 2026-11-05
**Checker:** collision-check agent

## Candidate claim
On Cortex-M33-class IoT nodes (EFR32xG24) over IEEE 802.15.4 / Thread, the total energy of a
PQC (ML-KEM-based) EDHOC or DTLS 1.3 handshake is dominated by radio transmission and 6LoWPAN
fragmentation of large keys/ciphertexts rather than by lattice compute; and there is a crossover
where a compact-ciphertext KEM beats ML-KEM-512 in total (compute+radio) energy.

Novelty hinges on: **Cortex-M33 + 802.15.4/Thread** (not BLE, not LoRa, not NB-IoT)
**+ explicit compute-vs-radio ENERGY decomposition + KEM-size crossover.**

## Queries run (verbatim)
1. (technical) `post-quantum key establishment energy cost wireless low-power personal area networks Cortex-M33 802.15.4`
2. (technical) `ML-KEM EDHOC DTLS 1.3 handshake energy radio versus compute IoT Thread 6LoWPAN fragmentation`
3. (plain language) `how much battery does a post-quantum handshake use on a tiny wireless chip sending big keys`
4. (IACR ePrint) `post-quantum KEM energy EFR32xG24 Thread 802.15.4 EDHOC site:eprint.iacr.org`
5. (neighbouring: networking/6LoWPAN) `Thread 6LoWPAN fragmentation energy overhead large payload 802.15.4 radio transmission cost IoT measurement`
6. (2026 recency) `post-quantum handshake energy decomposition compute radio crossover compact ciphertext KEM Cortex-M33 2026`
7. (crossover-focused) `compact ciphertext KEM beats ML-KEM-512 total energy constrained IoT radio dominated handshake`
8. (neighbour: EDHOC) `Enabling Quantum-Resistant EDHOC nRF52840 Cortex-M4 BLE 6LoWPAN performance evaluation energy`
9. (recency, Thread) `EFR32xG24 Thread OpenThread post-quantum ML-KEM handshake energy measurement radio compute`
10. (IACR/arXiv only, 2026) `post-quantum IoT key exchange energy communication versus computation 802.15.4 Thread crossover KEM 2026` (allowed_domains: eprint.iacr.org, arxiv.org)

## Papers opened and verified

| ID / URL | Verified | Depth | What it does | What it does NOT cover vs claim |
|---|---|---|---|---|
| arXiv 2602.18708v1 "On the Energy Cost of PQ Key Establishment in Wireless Low-Power PANs" (Liu, Ramachandran, Jurdak, 21 Feb 2026) | YES, full HTML read | Deep | **nRF52840 + BLE**, Power Profiler Kit II real HW. Decomposes energy compute vs comms; comms = 57-63% of total (the closest match to the "radio-bound" half of the claim). ML-KEM-512/768/1024. | **BLE not 802.15.4/Thread. Cortex-M4-class, not M33. No EDHOC/DTLS. No compact-KEM crossover** (lists HQC sizes only for reference). |
| arXiv 2610.13026 "PQ Session Establishment for Matter over Thread" (Oct 2026) | YES, full HTML read | Deep | **Thread / 802.15.4**, ML-KEM-512/768, "communication dominates, computation secondary." Message sizes, frame counts, radio time (85 ms / 318 ms / 1.15 s). | **Models/simulation only, no HW, no mJ energy, no compute-vs-radio energy split** ("All results are from models... not yet measured on hardware"; energy only "scales roughly with time"). **Matter CASE only, not EDHOC/DTLS. No compact-KEM crossover.** |
| arXiv 2609.21273 "Transcript-Bound Combiners..." (Gupta, Rana) | YES, abstract + PDF | Medium (PDF stream partly undecodable; abstract clear) | Combiner **security proof**; cost from a **calibrated Cortex-M4 cost model** (published measurements), reports transcript-binding = "1.5% of radio-inclusive energy." | **Not a measurement study; Cortex-M4 model not M33; no link tech named; no 802.15.4/Thread; energy is incidental; no crossover.** The "86% radio" figure in search snippets is a conflation, not this paper's contribution. |
| arXiv 2607.23007 "Practical PQC for Bandwidth/Power Constrained Devices" (Eichen et al., 25 Jul 2026) — subject of the "Survive the Radio" blog | YES, via blog summary | Medium | **Cortex-M33 @ 133 MHz**, radio = large share of energy; shared-secret vs certificate (10,688 vs 3,584 mJ). | **NB-IoT link, not 802.15.4/Thread. No explicit compute-vs-radio % split. No compact-KEM crossover. No EDHOC/DTLS/6LoWPAN.** |
| eprint 2026/2557 "Energy Consumption of PQC on Constrained & General-Purpose Architectures" | YES, abstract (PDF 403) | Shallow | PQC compute energy on **Raspberry Pi + laptops**. | **Compute only, no radio, no 802.15.4/Thread, no handshake decomposition, no crossover.** |
| IEEE Access 2025 "Enabling Quantum-Resistant EDHOC" (10937704) | YES, via RG/IEEE summaries | Medium | **nRF52840 Cortex-M4 + 6LoWPAN over BLE**, PQ-EDHOC energy per cipher suite (ML-KEM-512/HAWK1 best). | **BLE not 802.15.4/Thread; Cortex-M4 not M33; no explicit compute-vs-radio energy decomposition; no compact-KEM total-energy crossover.** |
| Future Internet 2026 18(6):316 "Evaluating PQC in IoT Networks" | partial (MDPI 403; via search snippets) | Shallow | Communication/fragmentation/reliability of PQC; LoRa link. | **PC/liboqs + LoRa, no MCU, no energy decomposition, no Cortex-M33/Thread, no crossover** (consistent with candidate's own note). |

## Analysis
No single paper holds the full combination (Cortex-M33 + 802.15.4/Thread + measured mJ compute-vs-radio
decomposition + compact-KEM-vs-ML-KEM-512 energy crossover). But the claim is now heavily surrounded:

- The **"radio-bound, not compute-bound"** finding is already established on BLE with real hardware
  (2602.18708) and asserted (unmeasured) on Thread (2610.13026). That headline is no longer novel in spirit.
- **Cortex-M33 + radio-dominated energy** exists (2607.23007) but over NB-IoT.
- **Thread/802.15.4 + ML-KEM + "comms dominates"** exists (2610.13026) but in simulation with no mJ and no crossover.
- The **compact-ciphertext-KEM total-energy crossover** appears in NONE of them — this is the genuinely
  unclaimed piece, as is a *measured/emulated* compute-vs-radio **energy** split specifically on
  **Cortex-M33 over 802.15.4/Thread with 6LoWPAN fragmentation** and for **EDHOC/DTLS 1.3** (2610.13026 is Matter CASE).

## Verdict
**Partial overlap.** The "radio dominates compute" headline is already shown on BLE (arXiv 2602.18708,
real HW, energy-decomposed) and asserted on Thread in simulation (arXiv 2610.13026). Still genuinely open
as far as searched: (a) a *measured or carefully-emulated mJ* compute-vs-radio energy split on
**Cortex-M33 / EFR32xG24 over 802.15.4/Thread** (existing M33 radio-energy work is NB-IoT; existing Thread
work is sim-only with no mJ), (b) for **EDHOC / DTLS 1.3** specifically (neighbours do BLE-EDHOC or Matter
CASE), and (c) the **compact-ciphertext-KEM vs ML-KEM-512 total-energy crossover**, which no opened paper
performs. Recommend narrowing the contribution to (b)+(c) with 6LoWPAN fragmentation effects, and citing
2602.18708 / 2610.13026 as the baselines whose headline you are *quantifying and extending*, not discovering.
