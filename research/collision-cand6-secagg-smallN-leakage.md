# Collision check — Candidate 6: Secure aggregation does not zero gradient leakage at small cohort size for low-dimensional sensor data

Date: 2026-11-05
Checker: collision-check agent

## Precise claim under test
Under honest-but-curious single-server secure aggregation (SA) with a small cohort (N <= ~8,
typical of a Thread/802.15.4 cell), a curious server can still partially invert the AGGREGATE to
recover per-device low-dimensional sensor signal above a privacy threshold. Therefore SA alone is
insufficient and must be combined with DP even among honest participants. There is an identifiable
N-threshold below which SA leakage is unacceptable.

Novelty hinges on the CONJUNCTION:
single-server honest-but-curious SA + small N + LOW-DIMENSIONAL sensor/time-series data (not images)
+ explicit N-threshold + "you still need DP" conclusion.

## Queries run (verbatim)
1. (technical) `secure aggregation federated learning gradient leakage small cohort size N threshold honest-but-curious server`
2. (plain-language) `can the server still learn individual data from secure aggregation with few participants federated learning why you still need differential privacy`
3. (data-regime technical) `secure aggregation privacy leakage low-dimensional sensor time-series data inversion aggregate IoT federated learning differential privacy`
4. (ePrint/arXiv-only) `eprint.iacr.org secure aggregation cohort size leakage threshold gradient inversion number of clients` [domains: eprint.iacr.org, arxiv.org]
5. (recency 2025-2026 / SoK) `gradient inversion secure aggregation small number clients underexplored N-threshold 2025 2026 survey`
6. (neighbouring community — IoT/wearables) `IoT wearable sensor federated learning secure aggregation insufficient need differential privacy small Thread cell reconstruct per-device reading`
7. (recency 2026, scoped domains) `honest-but-curious server invert aggregate recover per-client contribution low dimensional data federated learning privacy threshold 2026` [domains: arxiv, eprint, petsymposium, usenix]

## Papers opened and verified

- **Elkordy, Zhang, Ezzeldin, Psounis, Avestimehr — "How Much Privacy Does Federated Learning
  with Secure Aggregation Guarantee?"** PoPETs 2023 Issue 1 (popets-2023-0030), arXiv:2208.02304.
  Verified: title/authors/venue confirmed. Read: abstract + fetched analysis (deep on the N-claim).
  - Quantifies leakage vs N: "the amount of privacy leakage reduces linearly with the number of
    users participating in FL with SA" (mutual-information metric). Leakage also drops with larger
    local batch size.
  - DATA: images only — MNIST and CIFAR10. No sensor/time-series, no low-dimensional tabular data.
  - No explicit actionable N-threshold ("unacceptable below N=x"); gives a monotone/linear trend,
    not a privacy-threshold crossing for a specific application.
  - No IoT/constrained-device framing; no explicit "therefore use DP" design recommendation keyed
    to small-N IoT cells. ==> MOST LIKELY PARTIAL-COLLISION on the N-dependence mechanism only.

- **Kerkouche, Ács, Fritz — "Client-specific Property Inference against Secure Aggregation in FL."**
  WPES'23 (with CCS'23), arXiv:2303.03908. Verified: title/authors/venue confirmed. Read: abstract.
  - Threat model MATCHES: honest-but-curious server, single-server SA.
  - But recovers categorical client-specific PROPERTIES (e.g., presence of training data of a type,
    poisoning detection) via a cross-round linear-model optimization — NOT reconstruction of a
    per-device low-dimensional signal, and NOT an N-threshold result.
  - Data regime not sensor/time-series. Supports "SA gives limited privacy in practice" but does
    not deliver the conjunction. ==> adjacent, not a collision.

- **Carletti et al. — "SoK: Gradient Inversion Attacks in Federated Learning."** USENIX Security 2025.
  Verified: title/venue confirmed. Read: fetched full-PDF analysis (skim of SA + open-problems).
  - Catalogs GIAs, predominantly image classification (DLG/iDLG/GGL etc.).
  - Does NOT give an explicit numerical N-threshold for SA leakage; small/colluding-N and non-image
    (sensor/temporal) modalities appear under-covered / implicit rather than resolved.
  - Confirms the target regime is not closed by a known result. ==> supports "open", not a killer.

- **arXiv:2106.03328 — "Securing Secure Aggregation: Mitigating Multi-Round Privacy Leakage in FL."**
  Seen in results; scope is MULTI-ROUND participation leakage (overlapping cohorts across rounds),
  a different axis from single-round small-N aggregate inversion. ==> distinct, not a collision.

## Neighbour scan notes (no collision found)
- arXiv:2609.02971 (Sep 2026) "Gradient-Based Client Identity Inference ... Inertial Sensing in
  Vehicular Edge Networks" — honest-but-curious server, SENSOR (inertial) data, but task is client
  IDENTITY inference from per-client weight deltas (undefended updates), not inversion of the SA
  AGGREGATE at small N, and no N-threshold. Closest on data modality; distinct on mechanism. Worth
  reading in full in phase 2 to ensure its sensor-FL setup does not pre-empt the measurement design.
- IoT-FL + SA + DP works found (healthcare/IDS/wearables) propose COMBINING SA with DP as a design,
  but none locate an N-threshold where SA-alone leakage of a reconstructed per-device sensor signal
  becomes unacceptable. They assume DP is needed; they do not prove the small-N inversion claim for
  low-dim sensor data.
- IACR 2024-2026 SA papers (TACITA 2025/1579, Lighthouse 2026/346, SealAgg 2026/869, AlgOssa
  2026/2659) use "threshold" in the protocol/dropout/committee sense (minimum online clients for
  correctness/robustness), NOT a privacy-leakage N-threshold for aggregate inversion. Distinct.

## Verdict
PARTIAL OVERLAP. popets-2023-0030 (Elkordy et al.) already establishes the core N-dependence of SA
leakage (leakage decreases ~linearly in N) under honest-but-curious SA, which substantially pre-empts
the "SA does not zero leakage at small N, so you still need DP" message. What remains open as far as
searched: that result is image-only (MNIST/CIFAR10), gives a trend rather than an application-pinned
N-threshold, and is not instantiated for LOW-DIMENSIONAL sensor/time-series data on a constrained-N
IoT cell. The still-open sliver is the CONJUNCTION — per-device low-dim sensor-signal reconstruction
from the single-round aggregate + an explicit, calibrated N-threshold for that data regime + the
quantified DP add-on cost. The qualitative "need DP too" conclusion is NOT novel on its own.
