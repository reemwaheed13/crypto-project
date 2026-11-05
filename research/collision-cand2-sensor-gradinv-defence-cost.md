# Collision check — Candidate 2

**Candidate:** "On-device cost of defeating gradient-inversion on low-dimensional IoT sensor time series."

**Precise claim checked:** For FL on low-dimensional IoT sensor time series (smart-meter / air-quality
forecasting), there exists a cheap on-device defence (gradient clipping, calibrated DP noise, or update
sparsification) that defeats TS-Inverse-style gradient-INVERSION RECONSTRUCTION while costing less than a
small accuracy drop and a quantifiable, affordable energy/RAM overhead measured/estimated on a
Cortex-M33-class MCU.

**Novelty conjunction being tested:** gradient-inversion RECONSTRUCTION (not membership inference)
+ low-dim sensor time series + defence effectiveness-vs-COST on a constrained MCU + realistic FL
(FedAvg, batch>1, optional secure aggregation).

Date of check: 2026-11-05.

## Queries run (verbatim)

1. (technical) `gradient inversion attack defense cost federated learning time series sensor data differential privacy clipping on-device MCU`
2. (plain-language) `cheap defense to stop reconstructing sensor readings from gradients in federated learning on tiny devices energy cost`
3. (attack-specific) `TS-Inverse gradient inversion time series federated learning defense secure aggregation differential privacy`
4. (neighbouring community / TinyML) `TinyML embedded federated learning privacy gradient leakage defense Cortex-M33 energy RAM overhead`
5. (ePrint/arXiv + 2026 recency) `arXiv 2026 gradient inversion defense low-dimensional sensor time series microcontroller energy cost differential privacy accuracy tradeoff`
6. (ePrint-focused) `VAHANA Hardware-Aware Noise Addition Against Gradient Inversion Attacks ePrint 2026`
7. (MDPI partial-collision verify) `on-device privacy membership leakage lightweight sensor anomaly detectors INT8 differential privacy cost edge`
8. (domain-specific recency) `federated learning smart meter air quality forecasting gradient inversion defense clipping sparsification energy cost embedded device 2025 2026`
9. (MDPI deep verify) `"On-Device Is Not Private" membership leakage sensor anomaly detectors federated or centralized DP-SGD utility cost not characterized reconstruction model`
10. (SoK framing verify) `SoK Gradient Inversion Attacks federated learning Carletti USENIX 2025 open problems defense cost non-image modalities`

## Papers opened / verified

- **TS-Inverse**, arXiv:2503.20952 (IEEE SaTML 2025) — VERIFIED (abstract + review, medium depth).
  Attack tailored to federated time-series forecasting. Evaluated pruning / DP / sign-SGD as defences but
  reports NO on-device cost, NO calibrated (eps,delta), NO secure aggregation. Confirms the gap stance.
- **VAHANA**, ePrint 2026/1936 — VERIFIED (search snippet, shallow-medium). Hardware-aware noise against
  gradient inversion, but IMAGE datasets (Medical MNIST, CIFAR-10/100) on a RISC-V CV32E40P softcore +
  Kintex-7 FPGA. Accelerator/image domain, NOT low-dim sensor time series, NOT Cortex-M33, NOT FL time
  series. Not a collision.
- **MDPI MAKE 2025, "On-Device Is Not Private"**, doi 10.3390/make8100320 — VERIFIED via search snippets
  (page itself 403 to WebFetch), medium depth. CLOSEST NEIGHBOUR. Studies MEMBERSHIP leakage through
  anomaly scores on compact reconstruction-autoencoders over 6 multivariate time-series datasets (sensor/
  facility/finance). Defence = gradient clipping + Gaussian noise, reduces attack AUC 0.688 -> 0.508 at a
  utility cost of 0.054 detection AUC; INT8 artifacts run on a DESKTOP CPU (~56 mJ/inference reported).
  It is MEMBERSHIP INFERENCE on INFERENCE-TIME anomaly scores — NOT gradient-inversion reconstruction,
  and the setting is on-device inference, not FedAvg gradient sharing. Explicitly says DP utility cost on
  tiny reconstruction models "has not been characterized." PARTIAL overlap only.
- **Privacy Leakage in FL / inertial sensing**, arXiv:2609.02971v2 — VERIFIED (WebFetch, medium). FedAvg,
  batch 32, but threat is CLIENT-IDENTITY inference (not input reconstruction); DP eps + FL accuracy on a
  PC, HAR proxy data, NO MCU cost. Not a collision.
- **SVDefense** arXiv:2510.03319, **CENSOR** (NDSS 2025) arXiv:2501.15718, **Shadow defense**
  arXiv:2506.15711 — VERIFIED as image-domain gradient-inversion defences (shallow). Report accuracy/
  comms trade-offs, not low-dim sensor time series, not MCU cost. Not collisions.
- **SoK: Gradient Inversion Attacks in FL** (Carletti et al., USENIX Sec 2025) — VERIFIED exists
  (DL/ACM + GitHub taxonomy, shallow). Systematization; corroborates that non-image modalities and
  defence COST are under-studied. Supports the gap, does not fill it.

## Analysis against the conjunction

- Reconstruction vs membership: the only sensor-time-series + measured-defence-cost paper (MDPI MAKE) is
  MEMBERSHIP INFERENCE, not gradient-inversion reconstruction. The reconstruction-attack work on time
  series (TS-Inverse) does NOT measure defence cost and is not on-MCU.
- Device class: no opened paper measures or MCU-estimates defence cost against gradient-inversion
  reconstruction on a Cortex-M33-class device. VAHANA is RISC-V/FPGA + images; MDPI is desktop CPU +
  membership.
- FL realism: TS-Inverse supplies the FedAvg/time-series reconstruction threat but leaves defence cost,
  calibrated (eps,delta), batch-size sweep and secure aggregation open.

No single paper covers the conjunction {gradient-inversion reconstruction} x {low-dim sensor time series}
x {defence effectiveness-vs-cost on a constrained MCU} x {realistic FedAvg}.

## Verdict

**Partial overlap.** MDPI MAKE 2025 "On-Device Is Not Private" (doi 10.3390/make8100320) already measures a
clip+Gaussian-noise defence cost (utility drop + per-inference energy) on lightweight sensor time-series
models — but for MEMBERSHIP leakage, on a desktop CPU, outside FedAvg gradient sharing. What remains novel:
defence effectiveness-and-cost specifically against TS-Inverse-style gradient-INVERSION RECONSTRUCTION,
under realistic FedAvg (batch>1, calibrated (eps,delta), optional secure aggregation), with energy/RAM
measured or credibly estimated on a Cortex-M33-class MCU. Open as far as searched on that reconstruction+
MCU conjunction; recommend tightening the write-up to foreground reconstruction (not membership) and the
MCU cost measurement to avoid drifting into the MDPI membership result.
