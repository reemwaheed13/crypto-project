# Collision check, round 3: D2 (sensor gradient-inversion defence cost on Cortex-M33)

Date of check: 2026-11-10. Previous log: research/collision-cand2-sensor-gradinv-defence-cost.md (2026-11-05).

**Claim checked.** Smart-meter / air-quality nodes (EFR32MG24-class Cortex-M33, 78 MHz, 256 kB RAM) train a
forecasting model by FL. Asset: each node's raw time series. Adversary: honest-but-curious server running
TS-Inverse-style gradient inversion. Claim: a cheap on-device defence (clipping + calibrated DP noise at
stated epsilon, or sparsification) pushes reconstruction error above a usefulness threshold at a few %
sMAPE accuracy loss and an affordable per-step cost (cycles, RAM, energy) on the M33, presented as a
three-way privacy / accuracy / node-cost curve.

KNOWN from today's full-text reading (supplied by caller, not re-derived): TS-Inverse Table IX (noise
scale unstated, no epsilon; LTI survives Gaussian noise at sMAPE ~0.22/0.23); MAKE 8(10):320 is membership
inference on PC; Carletti SoK image-centric; Deutel et al. no M33, no privacy.

## Queries run (verbatim)

1. (suspect check) `EPC-TinyAD Electronics 2026 TinyML anomaly detection federated noisy updates privacy`
2. (technical, extended) `gradient inversion time series forecasting federated learning differential privacy epsilon defense 2026`
3. (embedded / MCU cost) `DP-SGD on microcontroller Cortex-M per-step overhead clipping noise federated learning energy measurement`
4. (plain-language) `can a server reconstruct my smart meter readings from federated learning updates and does adding noise stop it`
5. (neighbour verify) `Privacy Auditing Lithium-Ion Battery Ageing Model gradient inversion federated learning differential privacy time series`
6. (TinyML, extended) `differentially private on-device training microcontroller TinyML DP-SGD memory latency energy Cortex-M4 M33 2025`
7. (TinyML lead verify) `"Differential Privacy at the Edge" Federated Learning Framework GDPR-Compliant TinyML Deployments Cortex-M7 gradient inversion`
8. (citing works, arXiv, extended) `TS-Inverse citing follow-up time series gradient inversion attack federated forecasting 2026 arXiv`
9. (learned attack vs DP) `learned inversion model attack against differential privacy gradients time series sensor reconstruction federated`
10. (domain, 2026, extended) `gradient leakage IoT sensor data federated learning microcontroller defence evaluation smart meter load forecasting reconstruction DP 2026`
11. (ePrint-focused) `eprint.iacr.org gradient inversion federated learning embedded microcontroller differential privacy noise cost time series`
12. (neighbour detail) `Sudars Namatēvs Ozols gradient inversion battery transformer differential privacy noise multiplier batch size results`
13. (ML venue) `NeurIPS ICLR 2025 2026 gradient inversion time series forecasting defense differential privacy sparsification evaluation`

Re-run from round 1 in substance: Q2 (≈ old Q3/Q5), Q3/Q6 (≈ old Q4), Q10 (≈ old Q8). New: Q1, Q5, Q7–Q9, Q11–Q13.

## Papers opened / verified

- **EPC-TinyAD**, Electronics 2026, 15(13):2879, doi 10.3390/electronics15132879 (Sun, Qin, Chen, Zhao, Sun).
  VERIFIED exists; abstract via CiteDrive (MDPI page 403). Depth: abstract only. Industrial anomaly
  detection on spectrograms, CNN autoencoder student. "Federated training with clipped and noisy updates"
  is a design feature: NO privacy attack evaluated, NO epsilon, INT8 size *estimated*, latency
  benchmark platform unstated, and runtime memory/latency/power on physical Pi/Jetson/MCU explicitly
  deferred to future work. NOT a collision.
- **Sudars et al., "Privacy Auditing of Li-Ion Battery Ageing Model ... Gradient Inversion Attack in FL"**,
  Applied Sciences 2025, 15(10):5704. VERIFIED exists; abstract via search snippets (MDPI 403, DOAJ
  mirror truncated). Depth: abstract. NEW NEAREST NEIGHBOUR on the attack side: gradient inversion of
  sensor time series from a transformer under Gaussian-mechanism DP at several noise multipliers; finds
  reconstruction error saturates at a level set by privacy budget. Unknown from abstract: epsilon
  accounting, batch size, attack type (optimisation vs learned). No device cost mentioned. Battery
  ageing, not meter/air-quality forecasting. Full text should be read.
- **SVDefense**, arXiv:2510.03319. VERIFIED; HTML read to ~100k of 137k chars (medium). Gradient-inversion
  defence incl. HAR (IMU) and keyword spotting; DP-Gaussian/Laplace baselines with noise scale only (no
  epsilon); attacks IG, ROG, LTI (LTI only on CIFAR-10); B=1 attacks; device testbed = Raspberry Pi 4,
  Jetson TX2/Nano: normalised latency only, NO memory/energy, NO MCU. Not forecasting. Partial overlap
  (sensor modality + on-edge defence cost), but not MCU and not calibrated DP.
- **VAHANA**, ePrint 2026/1936 (Malik, Anand, Wong, Aysu, NCSU). VERIFIED; abstract page (PDF 403). Depth:
  abstract. Measures cost of noise-based GIA defence on an MCU-class RISC-V CV32E40P softcore (Kintex-7
  FPGA): memory -34%, latency up to 28.1x lower vs Gaussian-sampler baseline. Images only (Medical MNIST,
  CIFAR). Overlaps the "cost of DP noise on a small core" sliver; not M33, not time series, epsilon unknown.
- **arXiv:2609.02971** (FL privacy leakage, inertial sensing, vehicular). VERIFIED; abstract. Client
  IDENTITY inference, not reconstruction; RDP epsilon reported; HAR proxy; no MCU cost. Not a collision.
- **arXiv:2506.15711** Shadow defence. VERIFIED; abstract. Medical images; no cost. Not a collision.
- **CENSOR**, arXiv:2501.15718 (NDSS 2025). Seen in results only (snippet); image-domain defence; notes
  clipping/sparsification often ineffective. Not a collision. Depth: snippet.
- **arXiv:2602.16181** (FL energy-theft detection for resource-constrained smart meters). VERIFIED; HTML
  (medium). Gaussian noise optional, sigma unstated, no epsilon, no attack, laptop only. Not a collision.
- **ST-GIA**, arXiv:2407.08529 (DASFAA 2024) / 2410.16121. VERIFIED; abstract. Location reconstruction from
  spatiotemporal FL gradients with adaptive per-round perturbation; no MCU cost. Not a collision.
- **arXiv:2510.23931** (DP-SGD vs gradient leakage). VERIFIED; abstract. Vision only; no device cost.
- Seen but not opened (snippet only): DP-FlogTinyLLM arXiv:2604.19118; ECG federated AE with (eps,delta)
  arXiv:2606.11556; SBSEG TinyML DP-SGD IoT-23 on Raspberry Pi 4 (eps=0.5); "Differential Privacy at the
  Edge ... TinyML" (ResearchGate 390947565, claims Cortex-M7 testing, could not verify; Q7 found no
  indexable copy); Venom (AAAI) claims inversion under strong DP noise; L-SGD Applied Sci 12(9):4653 and
  arXiv:2509.18118 (Cortex-M/RISC-V training, no DP).

## Analysis

- Attack + calibrated DP on sensor time series: partly covered by Sudars et al. 2025 (battery, DP noise
  multipliers, saturation) and TS-Inverse Table IX (no epsilon). Neither reports on-device cost.
- Defence cost on constrained hardware: SVDefense (Pi/Jetson, latency only) and VAHANA (RISC-V softcore,
  images). Nobody found measures/estimates clipping+DP or sparsification per-step cycles/RAM/energy on a
  Cortex-M (any M-class), and no one does so for forecasting.
- Learned attacks: TS-Inverse shows LTI keeps sMAPE ~0.22 under Gaussian noise, so a defence judged only
  against optimisation attacks would overstate privacy. D2 must evaluate LTI-style learned inversion
  trained on defended gradients (adaptive attacker) at each epsilon.
- Unverified risk: the ResearchGate "DP at the Edge ... TinyML / Cortex-M7" item; should be resolved.

## Verdict

**Partial overlap.** Nearest: Sudars et al. (Appl. Sci. 15(10):5704), SVDefense (arXiv:2510.03319),
VAHANA (ePrint 2026/1936). Open as far as searched: calibrated-epsilon (accounted) clip+noise / sparsification
against BOTH optimisation and LTI-style learned inversion on low-dim meter/air-quality forecasting
windows, with per-step cycles, RAM and energy measured or estimated on a Cortex-M33, giving the
three-way privacy/accuracy/node-cost curve. EPC-TinyAD is not a collision (no attack, no epsilon,
hardware cost deferred).
