# Collision check — Candidate 3: Transciphering client cost on a Cortex-M33 IoT device

Date: 2026-11-05
Checked by: collision-check agent

## Candidate (precise claim)
On a Cortex-M33-class IoT client, transciphering (encrypt sensor data with a small HE-friendly
symmetric cipher, upload that) reduces client energy + RAM + uplink bytes versus direct CKKS/BFV
client-side HE encryption; and among HE-friendly ciphers (PASTA, FiLIP, Elisabeth, Transistor,
HERA/Rubato) one minimizes client-side cost on the MCU — with a per-message payload crossover
below which the HE path is not worth it.

Novelty hinges on: (a) an actual CLIENT-side software benchmark of HE-friendly symmetric
encryption on a real/emulated Cortex-M MCU, (b) comparing ciphers by client cost, and
(c) the direct-HE-vs-transciphering client crossover on-device.

## Queries run (verbatim)
1. (technical) `transciphering HE-friendly symmetric cipher client-side cost Cortex-M33 IoT microcontroller benchmark`
2. (technical) `comparing PASTA FiLIP Elisabeth HERA encryption cost on embedded microcontroller MCU client`
3. (plain-language) `which homomorphic-friendly cipher cheapest to run on small IoT device send encrypted sensor data to cloud`
4. (technical/neighbour) `hybrid homomorphic encryption client embedded device energy RAM uplink bytes versus CKKS BFV encryption`
5. (HHE Kombat scope) `HHE Kombat benchmarking hybrid homomorphic encryption schemes device platform`
6. (ePrint/2026-recency) `eprint 2026 transciphering client microcontroller Cortex-M33 symmetric cipher HHE benchmark energy`
7. (baseline verify) `SEAL-Embedded CKKS encode encrypt nRF52840 Cortex-M4 client homomorphic encryption RAM`
8. (PASTA on Edge verify) `PASTA on Edge cryptoprocessor hybrid homomorphic encryption FPGA ASIC client transciphering 2024`
9. (neighbour: embedded crypto / TCHES) `lightweight FHE-friendly stream cipher ARM Cortex-M software implementation energy cycles TCHES embedded sensor`
10. (PASTA 2021 platform verify) `Pasta case hybrid homomorphic encryption client benchmark Xeon server encryption cycles platform`
11. (plain-language 2026-recency) `cheapest way for tiny sensor chip to send encrypted data for cloud computation lightweight cipher vs full homomorphic battery 2026`

## Papers opened / verified
- **SoK: FHE-Friendly Symmetric Ciphers and Transciphering** — ePrint 2025/669 / TCHES 2025.
  Reading depth: abstract + search-corroborated; PDF fetch 403 (direct), confirmed via multiple
  result summaries. Benchmarks are SERVER-SIDE (homomorphic eval + client cost on x86/Xeon).
  Explicitly notes Rubato has poor *client-side* performance but does NOT measure on an MCU.
  No Cortex-M, no direct-HE-vs-transcipher on-device crossover. STATUS: not a collision.
- **Zeppelin: Client-Side BFV Encryption and Decryption for Helium-Powered Microcontrollers** —
  arXiv 2610.08301. Reading depth: full HTML fetch. Client = STM32N657 Cortex-M55 (Helium/MVE),
  800 MHz. DIRECT BFV only (no CKKS, no symmetric/transciphering). Reports ~340 uJ, 408.5 KB RAM,
  65,572 B seeded ciphertext, 4.76 ms enc. NO transciphering comparison, NO crossover.
  This is the strongest *direct-HE-on-MCU baseline* (newer than SEAL-Embedded, M55 not M4/M33).
  STATUS: partial — provides one leg (direct-HE client cost on an MCU) but not the symmetric
  ciphers nor the crossover.
- **SEAL-Embedded** — TCHES 2021. Reading depth: abstract + search summaries. First HE lib for
  embedded; CKKS encode+encrypt on nRF52840 Cortex-M4 (737 ms, ~136 KB RAM) and Cortex-A7 (77 ms).
  DIRECT CKKS client only; no symmetric cipher, no transciphering, no crossover. This is the
  classic direct-HE client baseline to cite. STATUS: partial — supplies the direct-CKKS leg only.
- **PASTA on Edge: Cryptoprocessor for HHE** — ePrint 2024/1919 (DATE 2025). Reading depth:
  search summaries (PDF 403). Client = dedicated FPGA/ASIC cryptoprocessor on a RISC-V SoC
  (130nm/28nm/7nm), NOT a software Cortex-M MCU benchmark. Single cipher (PASTA only).
  No cipher comparison, no direct-HE-vs-transcipher crossover. STATUS: not a collision (HW accel).
- **HHEML** — arXiv 2510.20243. Reading depth: full HTML fetch. Client = Xilinx PYNQ-Z2 FPGA SoC
  (Cortex-A9 + PL), NOT an MCU. Single cipher (PASTA). No multi-cipher client comparison,
  no crossover. STATUS: not a collision.
- **Pasta: A Case for HHE** — ePrint 2021/731 (TCHES 2023). Reading depth: search summaries
  (PDF 403). Client PASTA encryption timings measured on Intel Xeon E5-2699 v4 (17.0M cycles
  PASTA-3). Motivates embedded clients but does NOT benchmark on a Cortex-M MCU. STATUS: not a
  collision; confirms the "no MCU benchmark" premise.
- **Towards Privacy-Preserving FL using HHE** — arXiv 2603.26417. Reading depth: full PDF fetch.
  Desktop/server implementation (gRPC, pybind11, cryptography.io). No MCU/Cortex-M client, no
  symmetric-on-device cost, no crossover. STATUS: not a collision.
- **HHE Kombat: Benchmarking HHE Schemes** — ePrint 2026/1314. Reading depth: search summaries +
  GitHub/Springer listing. 19 frameworks / 218 cipher benchmarks in a client-server setting across
  HE libs/languages; edge coverage is FPGA-oriented. No evidence of a software Cortex-M client
  benchmark with a direct-HE-vs-transcipher crossover. STATUS: must-cite comprehensive baseline;
  not a collision as far as searched (recommend reading full PDF before publishing to confirm no
  Cortex-M client-cost table / crossover).
- **PETCHA: Post-quantum Efficient Transciphering with ChaCha** — ePrint 2026/112. Reading depth:
  abstract fetch. Transciphering throughput results (single-core), platform undisclosed in
  abstract; no MCU client benchmark or crossover mentioned. STATUS: not a collision as far as
  searched (verify platform in full text).
- **From PQC to HHE: SCA-Protected PASTA** — ePrint 2026/1485. Reading depth: search summary.
  HW/SW co-design of PASTA on a RISC-V PQC platform (RISQrypt) with accelerators; not a pure
  Cortex-M software client-cost comparison across ciphers, no direct-HE crossover. STATUS: not a
  collision (adjacent; cite as HW-accel point of comparison).

## Assessment of the three novelty legs
- Leg (a) direct-HE client cost on an MCU: ALREADY DONE (SEAL-Embedded = CKKS on M4; Zeppelin =
  BFV on M55). Candidate must treat these as baselines, not contributions.
- Leg (b) multiple HE-friendly symmetric ciphers compared by CLIENT cost on a Cortex-M MCU:
  NOT found. All client-cost cipher comparisons are server/x86 (SoK 2025/669) or HW-accel
  (PASTA on Edge, HHEML, 2026/1485). No software Cortex-M multi-cipher client benchmark located.
- Leg (c) on-device direct-HE-vs-transciphering client crossover (payload threshold):
  NOT found in any opened paper.

## Verdict
**Partial overlap.** The direct-HE-client-on-MCU leg is already covered: SEAL-Embedded (CKKS,
nRF52840 Cortex-M4, TCHES 2021) and Zeppelin (BFV, STM32N657 Cortex-M55, arXiv 2610.08301) are
the baselines to cite/beat, so that part is NOT open. What remains open as far as searched:
(1) a client-side software benchmark of HE-friendly SYMMETRIC ciphers (PASTA/FiLIP/Elisabeth/
Transistor/HERA-Rubato) on a Cortex-M33-class MCU comparing them by client cost, and (2) the
on-device transciphering-vs-direct-HE payload crossover. No opened paper does either of those on
an MCU. Before publishing, read full texts of SoK 2025/669, HHE Kombat 2026/1314, and PETCHA
2026/112 to confirm none hides a Cortex-M client-cost/crossover table.
