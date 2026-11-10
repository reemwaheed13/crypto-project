# Collision check, round 4, C1: PRG choice for SecAgg mask expansion on EFR32MG24

Date: 2026-11-11. Checker: collision-check subagent.

## Candidate
"Which PRG should expand secure-aggregation masks on a smart-meter MCU?" Device: EFR32MG24 (Cortex-M33 plus Secure Engine).
One mask per peer per round, 16,608 parameters (about 66 kB at 32 bits). Options: software ChaCha20 (our QEMU count: 1,226 M33
instructions per 64-B block), AES-128-CTR on the SE (datasheet Table 4.53: 474 us per 1 kB via the SE Manager API), software AES,
Ascon-XOF / SHAKE in software. Claim: the cheapest option on the device differs from desktop advice, the accelerator may not collapse
the cost, and mask expansion dominates SecAgg client cost.

## Queries run (all web, US index)
1. secure aggregation PRG mask expansion microcontroller AES ChaCha cost (technical)
2. mask generation cost secure aggregation IoT devices hardware AES accelerator federated learning (plain)
3. eprint.iacr.org secure aggregation constrained devices smart meter pseudorandom generator masking (ePrint-focused)
4. ChaCha20 vs AES-CTR Cortex-M energy benchmark hardware accelerator (embedded)
5. MicroSecAgg PoPETs 2024 secure aggregation small inputs PRG cost
6. EFR32MG24 Secure Engine AES throughput SE Manager performance application note
7. federated learning microcontroller secure aggregation on-device benchmark 2025 Cortex-M mask PRG energy (extended, 2025 variant)
8. ChaCha20 cycles per byte Cortex-M33 Cortex-M4 optimized implementation
9. secure aggregation implementation ESP32 OR nRF52840 OR STM32 pairwise masks PRG time energy (extended)
10. Silicon Labs Series 2 crypto performance mbedTLS benchmark AES-CTR SE CRYPTOACC throughput EFR32
11. hardware crypto accelerator slower than software small data overhead microcontroller AES benchmark energy ChaCha (embedded/networking)
12. eprint 2025 2026 secure aggregation lightweight clients PRG choice AES ChaCha Ascon benchmark (2025-2026 variant)
13. EFR32MG24 datasheet "AES-128" "CTR" "SE Manager" crypto performance table us
14. CHRONOS hardware-assisted phase-decoupled secure federated learning IoT TrustZone mask generation (2026)
15. silabs community Series 2 xG24 AES throughput SE mailbox overhead slow CRYPTOACC benchmark psa_cipher
16. benchmark AES ChaCha20 Ascon Keccak SHAKE Cortex-M33 cycles per byte lightweight cryptography 2024 2025
17. oryx-embedded crypto benchmark EFR32MG24 OR EFR32MG21 OR EFR32BG22 AES-CTR ChaCha20

## Papers / sources opened
| ID | What | Verified exists | Reading depth | Same claim? Same device? Same threat model? |
|---|---|---|---|---|
| arXiv 2604.19053 (CHRONOS, Apr 2026) | TrustZone SecAgg for IoT | yes (abs + HTML v3) | HTML, first ~100k of 130k chars | Uses AES-128-CTR with ARMv8-CE through mbedTLS, about 1.26 GB/s, masking under 6 ms for a 50k-param CNN. Cortex-A (RK3399/RK3588S), not Cortex-M. Does not compare PRGs. Says a Cortex-M port is future work. No collision. |
| PoPETs 2024-0077 / ePrint 2022/714 (MicroSecAgg) | single-server SecAgg | yes (landing page) | metadata only; PDF did not extract as text | DL-based small-input protocol. No sign of MCU PRG benchmarking. Not checked in full. |
| DATE 2017 (De Santis et al.), ChaCha20-Poly1305 on Cortex-M4 | embedded ChaCha | yes (PDF fetched) | could not extract text; snippet only | Motivates ChaCha on Cortex-M that lacks AES hardware. This is the known "ChaCha on MCU" argument. Not SecAgg. |
| gimli.cr.yp.to/perf.html | cross-primitive cpb table | search snippet only (fetch failed) | snippet | Cortex-M3/M4: AES-128 about 34 cpb, ChaCha20 about 13 cpb, Ascon about 196 cpb (snippet; need to check before citing). |
| PrivAgE arXiv 2309.12483 | SecAgg on edge | snippet | snippet | Pixel 5 phones, separates PRG time. Not an MCU. |
| Hyb-Agg arXiv 2511.23252 | one-shot SecAgg for IoT | snippet | snippet | Raspberry Pi 4. Not an MCU, not about PRG choice. |
| docs.silabs.com PSA driver pages | SE / CRYPTOACC architecture | yes | snippet | Say CRYPTOACC is on Virtual-SE devices, SE drivers go through the SE Manager mailbox, no throughput numbers. |
| Oryx EFM32GG11 benchmark | Series 1 AES-CTR HW vs SW | snippet | snippet | Different chip (Series 1 CRYPTO block). Not usable for MG24. |

## Verdict: Partial overlap (open as far as searched, but the parts are already known)
- No paper found that benchmarks PRG choice for SecAgg mask expansion on a Cortex-M MCU with a secure-element AES, so the exact
  question looks unanswered as far as searched.
- The parts are already KNOWN: ChaCha is about 2.5x faster than software AES on Cortex-M3/M4 (cpb tables). Hardware crypto
  accelerators lose on small calls because of setup and driver overhead (USENIX'03 Keromytis, Nordic devzone). CHRONOS already
  picks AES-CTR for SecAgg masks on Arm IoT, but on Cortex-A.
- Still open: measured energy (not just time) per masked parameter on the MG24 for SE AES-CTR vs software ChaCha20 vs Keccak/Ascon,
  including SE mailbox overhead versus call size, and where PRG cost falls relative to radio cost as the number of peers grows.

## Most damning reviewer objection
Radio, not the PRG, dominates. One mask expands in about 31 ms on the SE (66 kB x 474 us/kB) and an estimated 11-20 ms in software
ChaCha (13-19 cycles/B at 78 MHz). Sending the 66 kB masked update over 802.15.4 at 250 kb/s takes at least 2.1 s of radio time.
So "mask expansion dominates" only holds for many peers (roughly 70-200 or more). With logarithmic-degree graphs (Bell et al. CCS'20)
there are only tens of peers. The PRG choice then changes total client energy by a few percent at most, and a reviewer will call it
an engineering benchmark.

## KNOWN / CONJECTURE / ASSUMPTION
- KNOWN: ChaCha20 about 13 cpb and AES-128 about 34 cpb on Cortex-M3/M4 (gimli perf snippet, to be checked). CHRONOS uses HW AES-CTR
  on Cortex-A. Silabs docs: CRYPTOACC is on Virtual-SE parts, SE drivers use the SE Manager.
- CONJECTURE: software ChaCha20 beats SE AES-CTR in time on the MG24 for 66 kB buffers. Energy ranking unknown, because the SE may
  draw a different current. The datasheet has a separate "average current for SE Manager API" table that has not been checked.
- ASSUMPTION: 78 MHz core clock. Instruction count is close to cycle count (QEMU does not model cycles). The 474 us/kB figure covers
  a single 1 kB call, including mailbox overhead. Larger calls may amortise it. The MG24 is a Hardware-SE (Secure Vault) part, so
  "CRYPTOACC" may be the wrong name for its AES engine. Check this in the reference manual.

## EFR32MG24 SE AES throughput sources beyond the datasheet
None reliable found. Silabs docs give architecture only. Oryx/wolfSSL figures are for Series 1 parts. The only primary source is the
datasheet table and its companion SE Manager average-current table. The dependable route is the mbedTLS/PSA benchmark on the dev kit
(phase 2).
