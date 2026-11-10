# Collision check — Round 2, A4: Amortised PQ authentication of high-rate CAN FD sensor frames

Date: 2026-11-05. Searcher: collision-check subagent.

## Candidate (as received)
Application: Cortex-M ECUs broadcasting periodic sensor/control frames on CAN FD (64 B payload, 1–100 ms periods).
Asset: frame authenticity and origin. Adversary: compromised ECU that holds SecOC shared keys (so MACs give no
source authentication), plus a quantum forger against long-lived keys. Out of scope: DoS, side channels.
Claim (CONJECTURE): for each of ML-DSA-44, Falcon-512 and SLH-DSA there is a window size N* (Merkle-root or
TESLA-anchor signed once per N frames) that meets a 10 ms authentication deadline at under X% extra bus load,
and Falcon's size advantage disappears once signatures are amortised.

## Queries run
1. `post-quantum signature CAN FD authentication amortized Merkle tree batch frames` (technical)
2. `quantum-safe in-vehicle network message authentication ECU CAN bus Dilithium Falcon overhead` (plain language)
3. `eprint.iacr.org post-quantum CAN bus authentication automotive signature aggregation` (ePrint): FAILED (rate limit), redone as #11
4. `TESLA delayed key disclosure CAN FD post-quantum hash-chain broadcast authentication 2025`: FAILED (rate limit), redone as #5
5. `TESLA CAN FD authentication post-quantum`
6. `broadcast source authentication CAN bus hash chain delayed key disclosure ECU compromise (LiBrA-CAN OR TESLA OR LASAN)` (classical neighbours)
7. `2026 post-quantum signature amortization in-vehicle network SecOC hash-based Merkle stream authentication` (extended, 2026)
8. `quantum-resistant CAN FD message authentication protocol hash-based one-time signature ECU 2025 2026` (extended, 2025–26)
9. `automotive Ethernet TSN stream authentication post-quantum signature amortized per-packet latency` (networking neighbour)
10. `post-quantum CAN bus signature batch verification window frames latency bus load arXiv 2025 2026` (extended, 2025–26)
11. `CAN in-vehicle authentication batch signature Merkle tree ECU` (limited to eprint.iacr.org and arxiv.org)
12. `"Authentication protocol for secure automotive systems" benchmarking post-quantum cryptography`
13. `"Design-Space Mapping of Post-Quantum Cryptographic Artifact Transport on CAN-FD"`
14. `SAE 2026-26-0622 post-quantum two-wheeler CAN`

## Papers / items opened or checked
| ID | Item | Verified | Depth | Relevance |
|---|---|---|---|---|
| doi:10.3390/app16083705 | Lee, Sim, Eum, Song, Seo, "Design-Space Mapping of PQC Artifact Transport on CAN-FD: A Discrete-Event Simulation Study", Applied Sciences 16(8):3705, Apr 2026 | Yes (Crossref metadata) | Abstract only (MDPI full text returned 403) | CLOSEST. Discrete-event simulation of ISO-TP-segmented PQC artifact transport on CAN-FD under multi-ECU load. 29 parameter sets, 534 scenarios. Finds FN-DSA best for signatures and SLH-DSA nearly always infeasible. Covers one-off artifact transport (handshake/startup), with no sign in the abstract of per-frame stream authentication, Merkle/TESLA amortisation, a window-size optimum, or Cortex-M cycle costs. Must read the full text before claiming the amortised regime is uncovered. |
| arXiv:2504.10730 | Conti, Marchiori, Matarazzo, Rubin, "PQ-CAN: A Framework for Simulating PQC in Embedded Systems", QSNS 2025 | Yes | Abstract | Per-algorithm KEM/signature overhead on CAN at several bit rates. Falcon-512 is the best signature. No amortisation, batching or TESLA in the abstract. |
| SAE 2025-01-0451 | Smith, Owens, Kozan (SwRI), "PQC on Intravehicular Networks" | Yes | Abstract | Kyber/Dilithium/Falcon on 32-bit MCUs. Fragments keys across CAN frames. No amortisation. |
| SAE 2026-26-0622 | "Feasibility and Performance of PQC on Embedded Systems for Two-Wheeler Security" | Yes (listing) | Search snippet only | Cortex-M benchmarks. Search results do not show CAN authentication or amortisation. |
| ePrint 2023/492 | Aguilar-Melchor et al., "Batch Signatures, Revisited" | Yes | Search snippet | Generic Merkle batch signing. Falcon-512 adds about 82 B per extra message. This is the primitive A4 would use (KNOWN), with no CAN application. |
| draft-harvey-cfrg-mtl-mode-09 | IETF MTL mode signatures (ML-DSA/SLH-DSA), Mar 2026 | Yes | Search snippet | Generic amortised PQ signing for streams. States it suits subset verification over evolving streams and is a poor fit when each message needs its own signature. |
| Groza et al. | LiBrA-CAN (CANS 2012) and related TESLA-on-CAN work | Yes (PDF listed) | Search snippet | Classical (pre-quantum) broadcast authentication on CAN with delayed disclosure and key splitting. No PQ anchor signatures. |
| Mitacs project | "PQC integration in CAN FDx automotive networks" | Yes (project page) | Page read | Planned evaluation of Kyber/Dilithium/Ascon/Ring-LWE on CAN FDx. No mention of amortisation. Possible future competitor. |
| ISCAS 2020 | Ravi et al., "Authentication Protocol for Secure Automotive Systems: Benchmarking PQC" | Yes (metadata) | Metadata only (full text fetch refused) | PQ authentication protocol and bandwidth for ECUs. Appears to be a handshake, not frame-stream amortisation. Not confirmed. |
| NDSS 2024 | Twardokus et al., "When Cryptography Needs a Hand" (PQ V2V) | Yes | Search snippet | Neighbouring field (V2V radio, partially hybrid). Not CAN. |
| arXiv:2608.22123 | Lightweight PQ framework for IEC 61869-9 Sampled Values | Yes | Abstract | Neighbour (substation process bus). ML-KEM plus Chaskey MAC. Not a collision. |
| Springer s13635-026-00228-8 | PQ signature aggregation for low-latency distributed networks (2026) | Listed only | Not opened | Generic batch verification. Not CAN. |

## Verdict: PARTIAL OVERLAP
- Taken (KNOWN): per-message or one-off PQ signature transport cost on CAN/CAN-FD, including "Falcon/FN-DSA best,
  SLH-DSA infeasible" under bus contention (Lee et al. 2026, app16083705; PQ-CAN 2025). The generic Merkle batch-signature
  primitive is also taken (ePrint 2023/492; IETF MTL).
- Open as far as searched: amortised source authentication of periodic CAN FD frame streams (a PQ-signed Merkle
  root or TESLA anchor per window of N frames). This covers the deadline-vs-bus-load optimum N* per scheme, the
  question of whether Falcon's advantage survives amortisation, and the compromised-SecOC-key-holder threat model. None
  of the abstracts read address this.
- Not a minor variant of D1 (KEM handshake energy over Thread) or D5 (firmware signatures). It uses a different
  primitive mode (stream signatures), a different medium, and a different adversary.
- Risks: (a) the full text of app16083705 was not read and may discuss amortisation in its guidelines or future work.
  (b) Batch/MTL already establishes that amortisation shrinks the per-message cost, so the novelty sits in
  the CAN FD timing and deadline analysis and the threat model, not in the crypto. (c) The Mitacs CAN FDx project
  may be working on the same question.
