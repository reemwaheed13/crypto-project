# Collision log, round 4, C3: constant-time Shamir dropout recovery on Cortex-M33

Date: 2026-11-11. Agent: collision-checker (subagent).

## Candidate
"Dropout recovery cost on the sensor: constant-time Shamir secret sharing of mask seeds over a small field
(GF(2^8) or a prime field) on a Cortex-M33, and how it scales with N and the dropout threshold t."
- Application: secure aggregation (Bonawitz CCS 2017 / SecAgg+ / MicroSecAgg) on smart meters (EFR32MG24).
- Asset: the client's mask seeds and the shares of peers' seeds that the client holds.
- Adversary: (a) honest-but-curious server plus colluding clients (protocol level); (b) physical attacker with
  power/timing access to one meter (phase 2).
- Measured: cycles, RAM, flash, energy estimate for share generation and reconstruction vs N, k, t; constant-time
  status (no secret-dependent branches or table lookups).

## Queries run (11)
1. `constant-time Shamir secret sharing microcontroller` (standard)
2. `Shamir secret sharing side channel attack power analysis reconstruction` (standard)
3. `secure aggregation dropout recovery cost IoT microcontroller Shamir` (standard)
4. `eprint.iacr.org Shamir secret sharing side-channel Lagrange interpolation leakage` (standard, ePrint-focused)
5. `secret sharing benchmark ARM Cortex-M cycles GF(2^8) embedded` (standard, embedded)
6. `MicroSecAgg secure aggregation microcontroller implementation evaluation Raspberry Pi Cortex` (standard)
7. `secure aggregation federated learning side-channel attack on client device masking seeds 2025` (standard, ML community, date-targeted)
8. `secure aggregation smart meter microcontroller implementation dropout Shamir energy measurement 2025 2026` (extended, date-targeted)
9. `SLIP-39 Shamir Trezor constant-time GF(256) bitsliced Sprenkels sss side channel` (standard)
10. `Coron Prouff Roche "On the Use of Shamir's Secret Sharing against Side-Channel Analysis" 8-bit implementation timings` (standard)
11. `Bonawitz secure aggregation embedded benchmark microcontroller client cost Shamir share generation measured` (extended)
12. `secure aggregation TinyML Cortex-M federated learning microcontroller pairwise masking Shamir implementation nRF52 ESP32` (standard, embedded/ML)
13. `timing attack Shamir secret sharing implementation non-constant-time GF(256) table lookup vulnerability` (standard)
14. `side-channel analysis Shamir secret sharing implementation microcontroller 2025 2026 power trace share recovery` (extended, date-targeted)

## Papers / artifacts opened or checked
| ID | What | Verified status | Reading depth |
|---|---|---|---|
| arXiv 2511.23252 | One-Shot Secure Aggregation for FL in IoT | Exists (arXiv abs page fetched) | Abstract page. Raspberry Pi 4 (Cortex-A72), no MCU; dropouts NOT handled in experiments, Shamir for seeds only proposed as future work; no constant-time discussion |
| arXiv 2607.20890 | Information-theoretically secure aggregation for lightweight FL, dropouts and adversaries (2026) | Exists (abs page fetched) | Abstract. Targets "smartphones and IoT nodes" but no MCU measurements; MDS-code decoding, no side-channel content |
| PoPETS 2024-0077 / ePrint 2022/714 | MicroSecAgg (Guo, Polychroniadou, Shi et al.) | Exists (PETS page in results) | PDF fetched but unreadable (binary); ePrint PDF 403. Search snippets: evaluated on laptops (e-SeaFL benchmarks it on M1 Pro). Not verified in full |
| arXiv 2410.09676 | Attacks on MicroSecAgg (Zhang, Behnia, Yavuz et al.) | Exists (search results) | Snippet only: protocol-level predictable-mask flaw, not physical side channel |
| CVE-2023-25000 | HashiCorp Vault Shamir cache-timing via GF(256) lookup tables | Exists (OSV GO-2023-1709, multiple DBs) | Advisory summary. Shows table-based GF(256) Shamir leaks; fixed by constant-time arithmetic (server-class, not MCU) |
| go-slip39 (shurlinet) | Claims bitsliced constant-time GF(256) ported from Trezor firmware | Exists (pkg.go.dev) | Docs snippet. Implies Trezor firmware (Cortex-M) already has constant-time bitsliced GF(256) Shamir for SLIP-39. Trezor source itself NOT opened |
| Rust crates shamir_share, wavefnx/shamirs | Constant-time table-free GF(2^8) Shamir | Exist (docs.rs / GitHub) | Docs snippet; desktop libraries |
| Coron, Prouff, Roche, CARDIS 2012 (LNCS 7771) | Shamir-based masking (Goubin-Martinelli CHES 2011) broken by first-order SCA | Exists (several copies) | Snippet only; full text not reachable. Different setting (Shamir as AES masking, not multi-party seed sharing) |
| ePrint 2017/918 / TCHES 2018(1) | Chabanne, Maghrebi, Prouff, Linear repairing codes and SCA on Shamir-masked data | Exists (ePrint page) | Snippet. Fewer than t leaked shares can suffice depending on evaluation points |
| MNPY23 (Purdue), Benhamouda et al. CRYPTO 2018, HMNY TCC 2024 | Local leakage resilience of Shamir | Exist | Snippets. Theory: single-bit leakage per share; evaluation points matter |

Not found in any search: a paper measuring Shamir share generation / reconstruction cycles on any Cortex-M, nor any
SecAgg client implementation on an MCU (Cortex-M, nRF52, ESP32) with dropout handling.

## Verdict: PARTIAL OVERLAP (weak novelty), open as far as searched only for a narrow slice
- KNOWN (cited): table-based GF(256) Shamir leaks timing (CVE-2023-25000); constant-time bitsliced GF(256) Shamir
  exists in production code (Trezor SLIP-39 per go-slip39 docs; Rust crates). Shamir-structured data leaks under
  SCA and evaluation points matter (CARDIS 2012, TCHES 2018, MNPY23). IoT SecAgg papers stop at Raspberry Pi / laptop
  and either skip dropouts (2511.23252) or use MDS codes (2607.20890).
- What remains open as far as searched: (1) a measured, phase-by-phase cost of a SecAgg/SecAgg+ client on a Cortex-M33
  including Shamir share generation vs N, k, t; (2) a physical (power) SCA of the seed-sharing step of a SecAgg client
  on an MCU (phase 2).

## Most damning objections
1. Protocol misread: in Bonawitz/SecAgg+ the client never reconstructs. It generates shares once and, after dropouts,
   just forwards the shares it holds; the SERVER does Lagrange interpolation. "Reconstruction on the sensor" only
   exists in serverless/decentralised variants (e.g. arXiv 2404.17984, not opened).
2. Not the bottleneck. CONJECTURE (back-of-envelope, not measured): a 32-byte seed, SecAgg+ with k = 20 neighbours,
   t = 10 needs about 20 x 32 x 10 = 6,400 GF(2^8) multiplies; at an ASSUMED 30-60 cycles per constant-time
   multiply that is about 0.2-0.4 M cycles, i.e. 3-5 ms at 78 MHz. Full Bonawitz with N = 100, t = 50 is about
   160k multiplies, 5-10 M cycles, roughly 60-120 ms. Both are likely smaller than the k (or N-1) X25519 key agreements
   (ASSUMPTION: several hundred k cycles each on Cortex-M, from literature on M4, not checked here) and the PRG mask
   expansion. A reviewer will say "Shamir is milliseconds and dominated by DH; constant-time GF(256) is solved".
3. Constant-time over a prime field on M33 is near-free (single-cycle UMULL, ASSUMPTION), so the "constant-time"
   angle reduces to "do not use log/exp tables", which the Vault CVE fix already documents.

## Salvage suggestion (CONJECTURE)
Reframe as either (a) full SecAgg+ client cost breakdown on Cortex-M33 (DH, PRG, Shamir, comms/energy) vs N, k, t,
where Shamir is one row, or (b) phase-2 single-trace/horizontal power attack on the seed-sharing polynomial
evaluation (the same seed coefficient is reused across k evaluations, a horizontal-attack setting), which no
search here covered.
