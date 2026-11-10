# Collision check (round 4): per-round SecAgg + DP-SGD client cost on a Cortex-M sensor, vs number of clients

Date: 2026-11-11
Checker: collision-check agent
Prior notes read first: research/collision-cand4-pq-secagg-mcu-cost.md, research/collision-cand6-secagg-smallN-leakage.md,
research/papers/microsecagg.md (full-read note: MicroSecAgg measured on AWS r5.xlarge only, no MCU).

## Claim under test
The per-round cost on a Cortex-M class sensor of single-server secure aggregation (pairwise key agreement, PRG
masking, Shamir sharing for dropouts) together with DP-SGD, for FL on sensor data, and how that cost scales with
the number of clients N. Context: D2 (smart meters, EFR32MG24 Cortex-M33, 802.15.4, 16,608-parameter forecaster,
DP-SGD measured at 5.63 M instructions per step on the M33).

## Queries run (verbatim)
1. [technical, extended] secure aggregation federated learning microcontroller Cortex-M client cost pairwise masking Shamir measured
2. [plain] how expensive is secure aggregation on tiny IoT sensors federated learning energy per round number of clients
3. [ePrint only, domain eprint.iacr.org] secure aggregation constrained devices lightweight IoT federated learning implementation
4. [embedded, extended] secure aggregation Raspberry Pi ESP32 nRF52 federated learning benchmark key agreement Shamir dropout embedded
5. [recency 2025-2026, extended] secure aggregation distributed differential privacy DP-SGD resource-constrained IoT clients on-device cost microcontroller 2025 2026
6. [primitive cost] X25519 Curve25519 scalar multiplication Cortex-M4 cycles Haase Fujii Aranha
7. [ML/TinyML community] TinyML federated learning secure aggregation microcontroller STM32 nRF52840 masking measured cycles 2025
8. [networking community] wireless sensor network secure aggregation federated learning 802.15.4 Zigbee Thread smart meter pairwise masking overhead
9. [smart-grid community] privacy-preserving smart meter aggregation masking Diffie-Hellman implementation microcontroller ARM measured execution time Kursawe
10. [exact phrase, extended, domains arxiv/eprint/ieee/acm/mdpi/popets/usenix] "secure aggregation" "Cortex-M" federated learning
11. Fujii Aranha "Curve25519 for the Cortex-M4 and beyond" cycles X25519 907
12. X25519 Cortex-M33 cycle count benchmark scalar multiplication
13. LightSecAgg per-user computation communication complexity table SecAgg SecAgg+ O(N^2) O(N log N) offline online
14. Bonawitz practical secure aggregation client computation O(n^2 + mn) communication O(n+m) table cost
15. [application, recency, extended] smart meter federated learning load forecasting secure aggregation differential privacy on-device cost embedded 2025 2026
16. [recency 2026, extended, scoped domains] secure aggregation microcontroller implementation federated learning Shamir secret sharing ECDH energy measurement IoT node 2026
17. "Privacy-Preserving Federated Learning with Low-Cost Secure Aggregation for IoT Devices"

## Papers / sources opened (ID, verified status, reading depth)
- MicroSecAgg, Guo et al., PoPETs 2024(3), popets-2024-0077 / ePrint 2022/714. VERIFIED (prior full-read note).
  AWS r5.xlarge, Python/ABIDES, up to 1024 users. No MCU. No DP. Not a collision.
- LightSecAgg, So et al., MLSys 2022, arXiv 2109.14236. VERIFIED. Depth: ar5iv HTML, first ~100k of 136k chars
  (Table 1, setup, Table 4). Users = AWS EC2 m3.medium ("comparable to smartphones"); mentions ARM PyTorch for
  Raspberry Pi but reports NO embedded results. N <= 200. No DP (says it does not rely on DP). No MCU.
- Flamingo, Ma et al., S&P 2023, arXiv 2308.09883. VERIFIED. Depth: ar5iv HTML, first ~100k of 200k chars.
  Clients simulated in ABIDES on a 40-core Xeon; 1K clients in microbenchmarks; per-client CPU e.g. regular report
  0.22 s, BBG+20 report 0.02 s (single-threaded, server-class). States clients are weaker devices; reports none. No DP.
- Hyb-Agg, Emmaka & Phuong, "One-Shot Secure Aggregation ... for Private FL in IoT", arXiv 2511.23252. VERIFIED.
  Depth: HTML full text via fetch (setup, Table II, scaling). CLOSEST HIT. Raspberry Pi 4 (Cortex-A72, 1.5 GHz,
  8 GB) as the "constrained" client: N=50, d=8192, average client time 431.8 ms (keygen + CKKS encrypt + ECDH
  masking), uplink 787 KB/round (~12x plaintext). Client compute O(N*d) from pairwise ECDH masks, setup O(N),
  uplink O(d). No DP. No MCU. Curve not named; no separate ECDH timing.
- MDPI Future Internet 18(6):306, "Secure Federated Intrusion Detection for Resource-Constrained IoT Devices Using
  Lightweight Cryptography: A Hardware-Validated Study". VERIFIED as existing (search snippets; full-page fetch
  failed). Depth: snippets only. 3 ESP32 clients + RPi4 server, SPECK + ECDH-HKDF session channel; authors state
  explicitly it is NOT secure aggregation. Measures latency/RAM/ROM/energy of channel protection. Not a collision.
- Kabir, Megias, Cabaj, arXiv 2501.06161 (Computing Conf. 2025), RIOT smart metering on Nucleo boards. VERIFIED
  (abstract). Watermarking + AES, not masking SecAgg, not FL. Not a collision.
- Lenngren, ePrint 2025/523, "Assembly optimised Curve25519 and Curve448 implementations for ARM Cortex-M4 and
  Cortex-M33". VERIFIED (abstract page). Primitive cost source (see below). Board for M33 not named in abstract.
- github.com/Emill/X25519-Cortex-M4 README. VERIFIED (fetched). 548,873 cycles M4; 476,275 cycles M4F (FPU variant).
- Fujii & Aranha, LATINCRYPT 2017 / LNCS 2019, DOI 10.1007/978-3-030-25283-0_6. VERIFIED as existing; PDF fetch
  failed (expired cert). Cycle count NOT verified by me; do not cite a number from it.
- Bonawitz et al. CCS 2017 (ePrint 2017/281) and Bell et al. CCS 2020 (ePrint 2019/1407): VERIFIED as existing;
  ePrint PDFs returned 403. Depth: abstract/search only. Cost formulas taken via LightSecAgg Table 1 (read).
- Seen in results, NOT opened (do not treat as checked): IEEE Xplore doc 11442145 (title not confirmed; snippet says
  ECDH + multilayer weight masking for constrained IoT, no hardware details seen; 418 on fetch); DDP-SA arXiv
  2604.07125; Chen et al. ICML 2022 "Fundamental Price of Secure Aggregation in DP FL"; Kairouz et al. DDGauss
  arXiv 2102.06387; MDPI AI 7(9):358 (2026 co-design trilemma, snippet gives +68.5 KB SRAM for client-level DP);
  ePrint 2024/1655 secure stateful aggregation (DP-FTRL, RLWE, "weak ephemeral clients"); TinyFedTL arXiv 2110.01107.

## What is already published on MCU-class or near-MCU devices
- KNOWN: No opened paper measures a masking-based SecAgg client (key agreement + PRG masks + Shamir) on a Cortex-M or
  any MCU. Closest device class: Raspberry Pi 4 / Cortex-A72 (Hyb-Agg, 431.8 ms/client at N=50, d=8192, no DP;
  also arXiv 2601.01053 from round 1, PQ variant, RPi4).
- KNOWN: ESP32 work exists (MDPI fi18060306) but protects the channel only, not secure aggregation.
- KNOWN: MicroSecAgg, LightSecAgg, Flamingo all measure on server-class x86 and flag weaker clients as untested.
- KNOWN: The combination SecAgg + distributed DP is well studied in theory/communication (DDGauss, Skellam, Chen et
  al. ICML 2022) but on server-class simulation; no MCU cost found.

## Reusable cost formulas (KNOWN, from LightSecAgg Table 1, read; d = model size, N = users, s = seed length)
- SecAgg (Bonawitz 2017): offline comm per user O(sN); offline compute per user O(dN + sN^2); online comm per user
  O(d + sN); online compute per user O(d); server mask reconstruction O(dN^2).
- SecAgg+ (Bell 2020): offline comm O(s log N); offline compute O(d log N + s log^2 N); online comm O(d + s log N).
- LightSecAgg: offline comm O(d); offline compute O(d log N); online comm O(d); server recon O(d log N).
- Concrete per-client operation count (CONJECTURE, derived from the Bonawitz protocol structure, not from a table we
  read): 2 key pairs generated, 2(N-1) X25519 agreements (one for share-encryption keys, one for mask seeds),
  (N-1)+1 PRG expansions of length d, 2 Shamir t-of-N sharings, 2(N-1) AEAD encryptions of shares. In SecAgg+ replace
  N-1 by neighbourhood size k = O(log N).

## Verifiable X25519 cycle counts
- KNOWN: Cortex-M4 441,116 cycles; Cortex-M33 (with DSP) 411,061 cycles. Source: Lenngren, ePrint 2025/523, abstract.
- KNOWN: Cortex-M4 548,873 cycles; Cortex-M4F 476,275 cycles. Source: Emill/X25519-Cortex-M4 README (not a paper).
- KNOWN (search snippet only, not opened): Cortex-M0 3,589,850 cycles, Duell et al. 2015 (Des. Codes Cryptogr.,
  s10623-015-0087-1).
- ASSUMPTION: the 411,061 figure transfers to the EFR32MG24 M33 (it has DSP ext.; flash wait states may add cycles).

## Back-of-envelope (CONJECTURE, estimate only, not measured)
N = 8, Bonawitz-style: 14 X25519 x 411,061 = ~5.8 M cycles of key agreement per round, i.e. the same order as one
DP-SGD step (5.63 M instructions; ASSUMPTION instructions ~ cycles, unverified). PRG cost: 8 x 16,608 words of
mask expansion per round, not yet costed. Key agreement grows linearly in N per client (quadratic overall).

## Verdict
PARTIAL OVERLAP. The cost structure is known (Bonawitz 2017 / Bell 2020 / LightSecAgg asymptotics) and an IoT
client measurement exists on Raspberry Pi 4 (Hyb-Agg, arXiv 2511.23252; no DP). Open as far as searched: a
measured or calibrated per-round client cost of masking SecAgg + DP-SGD on a Cortex-M class MCU (cycles, RAM, radio
bytes over 802.15.4) and its scaling at small N. Risk: reviewers may see it as arithmetic over known formulas unless
it yields a non-obvious result (e.g. a crossover N where key agreement or radio bytes exceed DP-SGD compute, or a
RAM wall for Shamir state on 256 KB SRAM).
