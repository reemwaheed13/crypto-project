# Collision check — Candidate 4: PQ secure-aggregation MCU cost + channel-vs-mask distinction

Date: 2026-11-05
Checker: collision-check agent

## Candidate claim (verbatim)
Replacing classical DH key agreement in single-server secure aggregation (Bonawitz SecAgg / MicroSecAgg)
with a lattice KEM (ML-KEM) to get a post-quantum-SECURE-CHANNEL version has a measurable, possibly
prohibitive cost on a Cortex-M33-class MCU (peak RAM, per-round uplink bytes, cycles) that scales with
model/cohort size; AND a KEM swap alone does not make the input-hiding MASK post-quantum because the
pairwise mask is discrete-log-structured.

Two separable novelty questions:
- (a) Has anyone MEASURED PQ-secagg CLIENT cost on a constrained MCU / Cortex-M?
- (b) Has anyone made the explicit point that a KEM swap PQ-protects only the channel, not the DL mask?

## Queries run (verbatim)
1. [technical] post-quantum secure aggregation federated learning ML-KEM lattice masking Cortex-M microcontroller cost
2. [plain] cost of making secure aggregation quantum-safe on small IoT devices per-round bytes memory
3. [ePrint-only] SecAgg++ post-quantum privacy preserving federated learning protocol (domain: eprint.iacr.org)
4. [neighbour/embedded+recency] post-quantum secure aggregation federated learning embedded device benchmark 2026
5. ML-KEM swap secure aggregation only protects channel not pairwise mask discrete log quantum
6. Bonawitz secure aggregation pairwise mask discrete log Diffie-Hellman post-quantum vulnerability lattice mask (domain: eprint.iacr.org)
7. [embedded, recency] federated learning secure aggregation client cost Cortex-M33 EFR32 pqm4 Kyber RAM cycles on-device 2025 2026
8. "post-quantum" secure aggregation federated learning microcontroller "Cortex-M" client-side benchmark RAM flash constrained device (domains: arxiv.org, eprint.iacr.org)
9. SecAgg++ Fondazione Bruno Kessler masking lattice PRG seed ML-KEM768 dropout recovery benchmark
10. [plain/recency] how expensive is quantum-safe federated learning aggregation on tiny sensor devices 2026

## Papers opened / verified
- ePrint 2026/2497 "SecAgg++: A Post-Quantum Privacy Preserving FL Protocol" (AISec 2026, FBK). VERIFIED (abstract page + search).
  Depth: abstract + protocol description via search; PDF 403-blocked. Masking-based, single-server, ML-KEM used
  "for pairwise mask ESTABLISHMENT" + "unilateral masking mechanism" + PRG seed expansion. => KEM transports a
  SEED that feeds a PRG mask; it is NOT a lattice-structured mask. Benchmark = dropout-scenario protocol comparison;
  NO microcontroller/Cortex-M; no RAM/flash/cycle numbers on constrained device. No explicit channel-vs-mask framing found.
- ePrint 2026/2394 "Post-Quantum Dropout-Resilient Verifiable Secure Aggregation (PQ-DVSA)". VERIFIED (abstract page).
  Depth: abstract. Reports per-client runtime vs SWAS; hardware NOT specified in abstract, no MCU mention; verifiability focus.
- arXiv 2601.01053 "Byzantine-Robust FL w/ PQ Secure Aggregation for Critical IoT". VERIFIED (abstract).
  Depth: abstract + search. "IoT" = industrial IDS DATASETS; benchmark on Raspberry Pi 4 / Cortex-A72, 4GB RAM (Linux SBC),
  NOT a Cortex-M MCU. Uses CRYSTALS-Kyber KEM + homomorphic encryption (channel-type), not lattice pairwise masking.
- arXiv 2505.05751 "Efficient Full-Stack Private FL with PQ Security (Beskar)". VERIFIED (abstract). No MCU; FL-server setting.
- Context (NOT collisions, building-block cost only): pqm4; arXiv 2603.19340 (ML-KEM/ML-DSA on Cortex-M0+/RP2040);
  arXiv 2309.00440 (Plantard Kyber on low-end 32-bit). These measure the KEM PRIMITIVE, not protocol-level secagg composition.

## Ruling
- (a) MCU client-cost measurement of PQ-secagg: OPEN as far as searched. No opened paper benchmarks a PQ secure-aggregation
  CLIENT on a Cortex-M-class MCU with RAM/uplink-bytes/cycles. Closest (2601.01053) uses a Cortex-A72 Linux SBC = different device class.
- (b) Channel-vs-mask distinction: PARTIAL. SecAgg++ already PQ-protects the seed transport via ML-KEM AND uses a PRG mask
  (so its mask seed is PQ-delivered). It does NOT frame the "KEM swap protects channel not the DL mask" argument explicitly,
  and it does not build a lattice-STRUCTURED mask. The candidate's negative claim (plain KEM swap leaves a DL-structured mask
  quantum-broken) is not stated as such in the literature found, but SecAgg++ partially occupies this design space.

## Verdict
PARTIAL OVERLAP. The MCU/Cortex-M client-cost measurement (question a) is open as far as searched — no PQ-secagg paper
benchmarks on a constrained MCU (closest is a Raspberry Pi 4 / Cortex-A72 SBC). The channel-vs-mask conceptual point (b) is
partially pre-empted: SecAgg++ (ePrint 2026/2497) already delivers mask seeds via ML-KEM with PRG-expanded unilateral masking,
so framing a bare KEM swap as "channel-only, mask still DL-broken" is weakened unless sharpened against SecAgg++'s exact design.
