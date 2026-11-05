# Direction mining — CS 5803 IoT crypto project — 2026-11-05

Full round across PQC, FHE and FL-privacy for IoT. 14 papers read in depth (notes in `research/papers/`),
one-hop reference follow, 6 candidate directions collision-checked (logs in `research/collision-cand*.md`).
Selection rule used this round: **top directions by score, overall** (families not forced).

> Conventions: **KNOWN** = from a cited, verified source. **CONJECTURE** = our hypothesis, unproven.
> **ASSUMPTION** = a modelling choice we adopt. No MCU number below is measured — any MCU figure named
> in a plan is a target to obtain via the `mcu-cost-estimator` skill, labelled "(estimate — to calibrate
> against <source>)". IDs were verified by the reader/collision agents; items still behind paywalls are
> flagged in the verification log. "Open as far as searched" never means "proven open".

---

## 1. Bottom line

Every one of the six candidates came back **Partial overlap** — the field is moving fast enough that no
clean greenfield gap survived, but none was killed either; in each case the collision-checkers isolated a
specific sliver that is still open. The strongest three, by score, are: **(D3) a client-side, on-MCU
benchmark of HE-friendly symmetric ciphers for transciphering and the transciphering-vs-direct-HE
crossover** — no collision found, maximally feasible in software, and with a natural side-channel DRP;
**(D2) the on-device cost of defeating gradient-*inversion* reconstruction on low-dimensional IoT sensor
time series under realistic federated learning** — the nearest work does membership inference on a
desktop, not reconstruction on a constrained device; and **(D1) a compute-vs-radio energy decomposition
of a post-quantum EDHOC/DTLS handshake on a Cortex-M33 over 802.15.4/Thread, with a compact-ciphertext-KEM
crossover** — where the "radio dominates" headline is already published (so we quantify a known effect and
add the crossover, capping novelty). We recommend D3 and D2 as the two primary directions and D1 as a
measurement-heavy third option; we drop D5 (firmware signatures — already done) and deprioritise D4 (PQ
secure-aggregation cost — feasibility risk) and D6 (small-N SA leakage — largely pre-empted by Elkordy
PoPETs 2023).

---

## 2. Mining table

Reading depth: F = full-read, S = skimmed/partial, A = abstract-only (full text paywalled/403).
Status: all IDs verified unless noted. "Measured on" is what the paper *actually* ran on.

| # | Paper (verified ID) | Status / venue | Depth | Measured on | Seed it gives | Move | → Candidate |
|---|---|---|---|---|---|---|---|
| 1 | SLOTHY ML-KEM/ML-DSA M4→M7 (ePrint 2025/366) | AsiaCCS 2025 (not TCHES) | F | Real Cortex-M7 (Nucleo-F767) | M-class PQC cycle template; M33 untested | Move to device | feeds D1 |
| 2 | Evaluating PQC in IoT Networks: fragmentation (Future Internet 18(6):316) | Journal 2026 | A | liboqs on PC; LoRa link | handshake = many packets; comms-bound | Whole-system | feeds D1 |
| 3 | Enabling Quantum-Resistant EDHOC (IEEE Access 2025, 10.1109/ACCESS.2025.3554010) | Journal 2025 | S | nRF52840 M4, 6LoWPAN over **BLE** | PQ-EDHOC exists; no radio/CPU energy split | Whole-system | feeds D1 |
| 4 | Low-Stack HAETAE for MCUs (arXiv 2604.15868 = ePrint 2026/635) | arXiv/ePrint 2026 | F | Real Cortex-M4 (Nucleo-L4R5) | stack crossover vs ML-DSA — **already done** | Move to device | → D5 (dropped) |
| 5 | Realistic PQC eval for consumer IoT (IoT j. vol33 2025, 10.1016/j.iot.2025.101650) | Journal 2025 | A | Consumer IoT, BLE vs Wi-Fi | link dominates cost; per-interface suites | Whole-system | feeds D1 |
| 6 | SoK: FHE-Friendly Ciphers & Transciphering (ePrint 2025/669) | TCHES 2025 i3 | S | **Server-side Xeon only** | cipher zoo; no client/embedded cost | Move to device | → **D3** |
| 7 | Transistor: TFHE-friendly stream cipher (ePrint 2025/282) | CRYPTO 2025 | S | **Server-side CPU only** | cheap-client cipher, MCU cost unproven | Move to device | → **D3** |
| 8 | Towards Lightweight CKKS: client cost (ePrint 2025/720) | ASIACCS 2026 | A | PC; keys still ~285 MB | direct CKKS client infeasible on MCU | Find crossover | → **D3** (motivation) |
| 9 | HHEML: Hybrid HE PPML on Edge (arXiv 2510.20243) | arXiv 2025 | F | **PYNQ-Z2 FPGA SoC**, not an MCU | "edge" overstated; re-cost PASTA on M33 | Move to device | → **D3** |
| 10 | MicroSecAgg (PoPETs 2024.3; ePrint 2022/714) | PETS 2024 | F | AWS EC2 (PC), ≤1024 users | classical SecAgg baseline; DH swap point | Swap primitive | → D4 (deprioritised) |
| 11 | TS-Inverse: gradient inversion for FL time series (arXiv 2503.20952) | IEEE SaTML 2025 | F | PC/GPU; smart-meter/air-quality | low-dim inversion attack; no on-device/DP/SA | Change data + attack defence | → **D2** |
| 12 | SoK: Gradient Inversion Attacks in FL (USENIX Sec 2025, Carletti) | USENIX 2025 | F | Literature only (no re-runs) | image-centric; low-dim & small-N SA open | Change data | → D2, D6 |
| 13 | On-Device Training of Quantized DNNs on Cortex-M (arXiv 2407.10734) | IEEE TCAD 2024 | S | Real M0+/M4/M7 (no M33) | on-device gradients are real; cost anchor | Account for system | realism anchor for D2 |
| 14 | SecAgg++: PQ privacy-preserving FL (ePrint 2026/2497) | AISec 2026 | A | server/PC; no MCU | PQ secagg protocol exists; no MCU cost | Swap primitive | → D4 (deprioritised) |

Papers surfaced by the reference-follow / collision hop (verified by the collision agents, not deep-read
this round): arXiv 2602.18708, 2610.13026, 2607.23007, ePrint 2026/2557 (PQC handshake energy, D1);
MDPI MAKE 10.3390/make8100320, ePrint 2026/1936 (VAHANA), arXiv 2510.03319, 2506.15711, 2609.02971, NDSS-25
CENSOR (D2 neighbours); SEAL-Embedded (TCHES 2021), Zeppelin (arXiv 2610.08301), PASTA-on-Edge (ePrint
2024/1919), ePrint 2026/1485 / 2026/1314 / 2026/112 (D3 neighbours); arXiv 2601.01053, ePrint 2026/2394,
arXiv 2505.05751 (D4); ePrint 2026/2578 / 2026/2066 / 2026/2149, ePrint 2022/1198, arXiv 2106.05577 (D5);
Elkordy PoPETs 2023 (popets-2023-0030 / arXiv 2208.02304), arXiv 2303.03908, 2106.03328 (D6).

---

## 3. Direction cards (top 3 by score)

### D3 — Which HE-friendly cipher is cheapest on the *client*, and does transciphering actually pay off on a Cortex-M33?

**Move used:** Move to the constrained device + find the crossover.
**Application:** An IoT node (environmental / smart-metering sensor on an EFR32xG24-class Cortex-M33)
periodically uploads readings to a cloud service that computes on them under homomorphic encryption
(e.g. aggregate statistics, anomaly scoring). To avoid the huge ciphertext expansion of sending CKKS/BFV
ciphertexts directly, the node instead encrypts with a small **HE-friendly symmetric cipher** and the
server *transciphers* (homomorphically re-encrypts) server-side.
**Threat model:** Asset = the raw sensor readings. Adversary = an honest-but-curious cloud server (and a
passive network eavesdropper); classical adversary (this is a confidentiality-of-outsourced-data question,
not a PQ one). Out of scope: a malicious server that returns wrong results, and side channels (that is the
DRP). What we measure is therefore *client-side* cost, not server security.
**Claim (falsifiable):** On a Cortex-M33, (a) the HE-friendly ciphers rank differently by *client* cost
(cycles + stack + uplink bytes + energy) than they do by the *server-side* homomorphic cost the SoK
tabulates — i.e. the server-optimal cipher is **not** the client-optimal one; and (b) there is a
per-upload payload size below which transciphering costs the client *more* total energy than just sending
the readings under a conventional AEAD (because the HE-friendly cipher's keystream/state work and the
larger symmetric ciphertext outweigh the savings).
**Plain-language explanation:** HE-friendly ciphers are designed so the *server* can evaluate them cheaply
inside FHE; nobody optimised them for a tiny client. Some (e.g. the Rubato/HERA family) trade heavier
client work for lighter server work. A microcontroller is the opposite of a server — it has kilobytes of
RAM, no vector units, and pays for every byte it transmits over the radio. So the cipher that looks best
in a server benchmark may be the worst on the node, and vice-versa. Separately, transciphering only saves
the client work when the thing being avoided (direct FHE encryption) is genuinely expensive; for very
small payloads the fixed overheads dominate and the clever scheme loses to plain symmetric crypto. Both
effects are measurable.
**Status of evidence:** KNOWN — all public HE-friendly-cipher benchmarks are server-side (ePrint 2025/669
Xeon; 2025/282 server CPU); direct-FHE *client* encryption on an MCU has been measured (SEAL-Embedded on
nRF52840 Cortex-M4, TCHES 2021; Zeppelin BFV on Cortex-M55, arXiv 2610.08301) — these are our calibration
baselines, not collisions. CONJECTURE — the client ranking inverts and a payload crossover exists.
ASSUMPTION — EFR32xG24-class M33 with hundreds of KB RAM; reference C implementations of PASTA / FiLIP /
Elisabeth / Transistor are faithful enough to benchmark.
**Collision check:** **Open as far as searched** — no paper benchmarks HE-friendly symmetric ciphers by
*client* cost on a Cortex-M MCU, nor gives the on-device transciphering-vs-direct-HE crossover (log:
`collision-cand3-transciphering-client-mcu.md`). Caveat: three 2026 ePrints (2026/1314 HHE-Kombat,
2026/112 PETCHA, and 2025/669's full tables) were 403-blocked and must be read in full before we publish,
in case a Cortex-M client-cost table is buried in one.
**Kill experiment (week 1):** Compile reference C of two contrasting HE-friendly ciphers (one bit-oriented,
e.g. FiLIP; one word-oriented over F_p, e.g. PASTA) plus a baseline AEAD (AES-GCM / ASCON), run them under
a Cortex-M33 instruction-count harness (QEMU `mps2-an505` or pqm4-style), and measure cycles + static
stack for encrypting one sensor frame. If the client ranking *and* the per-byte cost already match the
server-side ordering with no crossover in sight, the headline claim is dead — one day of work.
**Measurements:** per-cipher client cycles and peak stack (instruction-count estimate — to calibrate
against SEAL-Embedded's published M4 numbers); flash/code size (static); uplink bytes = symmetric
ciphertext size vs direct-HE ciphertext size; energy = cycles × active-current-per-MHz + TX-bytes ×
radio-energy-per-byte (estimate — to calibrate against the EFR32xG24 datasheet, numbers VERIFY-before-use).
Crossover = payload size where transciphering-energy = AEAD-baseline-energy.
**Semester plan:** (1) harness + AEAD baseline + two ciphers, reproduce server ordering as a sanity check;
(2) add the remaining HE-friendly families, produce the client-cost ranking table; (3) add the radio-energy
model and locate the crossover; (4) write up, with an explicit "server-optimal ≠ client-optimal" figure.
**DRP phase 2:** Port the winning cipher(s) to the real EFR32xG24 kit, measure true energy, then a
side-channel study of the symmetric client encryption (power/EM on the keystream) — a live topic
(ePrint 2026/1485 studies SCA-protected PASTA), giving a clean phase-2 that matches the instructor's hint.
**Group split:** cipher implementers (split the cipher zoo) / MCU cost-model & harness / direct-HE baseline
& crossover analysis / evaluation & writing.
**Scores (1–5):** novelty **4**, threat-model clarity **4**, falsifiability **5**, feasibility **5**,
DRP continuity **5**, group split **5**.

---

### D2 — What does it actually cost a sensor node to defeat gradient-inversion reconstruction?

**Move used:** Change the data (image → low-dimensional sensor time series) + attack the defence.
**Application:** A fleet of smart meters / air-quality nodes trains a shared forecasting model by federated
learning; each node computes gradients locally (on-device training on Cortex-M is real — IEEE TCAD 2024)
and uploads them to a server that aggregates with FedAvg.
**Threat model:** Asset = each node's raw readings (a household's load curve is sensitive). Adversary =
honest-but-curious server with white-box model access that runs a TS-Inverse-style gradient-*inversion*
attack to reconstruct a node's input series from its uploaded gradient; passive, classical. Out of scope
this semester: active parameter-manipulation ("malicious server") attacks, and Sybil servers.
**Claim (falsifiable):** For low-dimensional sensor series there is an on-device-cheap defence setting —
gradient clipping + calibrated DP noise at some (ε, δ), or update sparsification — that pushes the
reconstruction error above a usefulness threshold (attacker can no longer recover the load curve) while
costing **less than a stated accuracy drop** (CONJECTURE: a few percent sMAPE) and an **affordable** extra
energy/RAM budget per training step on a Cortex-M33 (CONJECTURE: within the same order of magnitude as the
training step itself).
**Plain-language explanation:** Gradient-inversion attacks were built and mostly tested on images, where a
single example has thousands of pixels; a short sensor window has maybe tens of numbers, so the attack
behaves differently — sometimes easier (less to guess), sometimes harder (less structure to exploit). The
usual defence is differential privacy: clip each gradient and add noise. The open question is not "does DP
work" (it does, with a utility cost) but "how much noise is *enough* for this low-dimensional data, and
what does that specific amount cost a battery-powered node to apply at every training step?" Nobody has
put the reconstruction attack, the low-dimensional sensor data, the calibrated privacy budget, and the
on-device cost in the same study.
**Status of evidence:** KNOWN — TS-Inverse (SaTML 2025) reconstructs smart-meter series but tests only
uncalibrated noise/pruning, no secure aggregation, no on-device cost; the GIA SoK (USENIX 2025) names
non-image data and defence *cost* as open problems. The closest neighbour, MDPI MAKE 2025
("On-Device Is Not Private", 10.3390/make8100320), measures a DP cost on sensor models but for
*membership inference* on a *desktop CPU*, not gradient-inversion reconstruction on an MCU, and not in FL.
CONJECTURE — a cheap defence suffices at low dimensionality. ASSUMPTION — FedAvg, batch > 1, a published
low-dim dataset (Electricity-370 / London smart-meter), on-device cost estimated from TCAD-2024 per-step
figures.
**Collision check:** **Partial overlap** — the reconstruction + low-dim-sensor + on-MCU-cost + realistic-FL
conjunction is open; distinct from the membership-inference MDPI paper and from image-domain defences
(CENSOR/SVDefense/Shadow) (log: `collision-cand2-sensor-gradinv-defence-cost.md`). Novelty rests on keeping
the framing on *reconstruction* and on the *on-device cost*, not on "DP helps".
**Kill experiment (week 1):** Reproduce TS-Inverse on one public low-dim dataset at batch 1, then rerun
with FedAvg batch > 1 and no defence. If reconstruction already fails once the attacker sees a realistic
averaged gradient (batch > 1, multi-step), the premise ("a defence is needed") collapses and the direction
pivots to "low-dim FL is accidentally private" — either way, one cheap run decides it.
**Measurements:** attacker reconstruction error (sMAPE / correlation) vs (ε, δ) and vs clipping/sparsity;
model accuracy (sMAPE) vs the same; on-device per-step cost of the defence = extra cycles for clip+noise +
RAM for the noise draw (instruction-count estimate — to calibrate against IEEE TCAD 2024 per-step figures
and the EFR32xG24 datasheet); communication = uplink bytes under sparsification.
**Semester plan:** (1) reproduce attack + the kill experiment; (2) sweep defences × privacy budget, build
the leakage-vs-accuracy curve on low-dim data; (3) add the on-device cost model, produce the
cost-vs-privacy curve; (4) write up the three-way (privacy / accuracy / node-cost) trade-off.
**DRP phase 2:** Run the defended training step on the real EFR32xG24 and measure true energy; optionally
compose with a secure-aggregation layer (links to D4) to test leakage under aggregation. Side-channel
continuity is weaker here than for D3 (this is an ML-privacy study, not a crypto primitive).
**Group split:** attack reproduction / defence + DP calibration / FL training harness & datasets /
on-device cost model & evaluation.
**Scores (1–5):** novelty **4**, threat-model clarity **5**, falsifiability **5**, feasibility **4**,
DRP continuity **4**, group split **5**.

---

### D1 — Is a post-quantum handshake radio-bound on a Cortex-M33 over Thread, and where is the KEM-size crossover?

**Move used:** Account for the whole system (compute + radio + fragmentation) + find the crossover.
**Application:** A Cortex-M33 node (EFR32xG24) joins a Thread / 802.15.4 network and runs a post-quantum
EDHOC (RFC 9528) or DTLS 1.3 key exchange to set up an OSCORE context, using ML-KEM.
**Threat model:** Asset = long-term confidentiality of the session (harvest-now-decrypt-later by a future
quantum adversary). Adversary = passive network recorder now + quantum decryptor later. Out of scope:
active downgrade/MITM and side channels (DRP). Measured quantity = energy, split into compute vs radio.
**Claim (falsifiable):** On a Cortex-M33 over 802.15.4/Thread, the **radio + 6LoWPAN-fragmentation** energy
of a PQ EDHOC/DTLS handshake exceeds the **lattice-compute** energy by a large factor (CONJECTURE: >5×),
and there is a crossover model size / parameter choice at which a compact-ciphertext KEM beats ML-KEM-512
in **total** (compute + radio) handshake energy.
**Plain-language explanation:** ML-KEM is fast to compute even on a microcontroller, but its public keys
and ciphertexts are ~1 KB+, and a Thread frame carries only ~tens to ~hundred-odd usable bytes, so the
handshake fragments into many packets — and on a radio, sending bytes is where the energy goes. So the
"cost" of PQC on such a node is mostly a *networking* cost, not a *crypto* cost. If that is true, then a
scheme with smaller ciphertexts can win overall even if its math is slower — there should be a measurable
crossover. The subtlety (and the reason novelty is capped) is that "communication dominates" has already
been shown on BLE and asserted for Thread; our contribution is the *quantified Cortex-M33 + Thread + EDHOC
energy split in millijoules* and the *crossover*, which no single paper gives.
**Status of evidence:** KNOWN — "radio/communication dominates" is published: arXiv 2602.18708 shows the
compute-vs-comms energy split on **BLE/nRF52840**; arXiv 2610.13026 asserts it for **Thread** but is
simulation-only (Matter CASE, no millijoules, no split); arXiv 2607.23007 has M33 energy over **NB-IoT**;
Future Internet 18(6):316 shows fragmentation but on PC/LoRa. CONJECTURE — the >5× factor and the KEM
crossover on M33/Thread/EDHOC specifically. ASSUMPTION — EFR32xG24 radio-energy-per-byte and MCU
active-current from the datasheet (VERIFY), a 6LoWPAN MTU and a retransmission model.
**Collision check:** **Partial overlap** — the phenomenon is published, but the measured M33 + 802.15.4/
Thread + EDHOC/DTLS millijoule energy split *and the compact-KEM total-energy crossover* are open as far as
searched (log: `collision-cand1-pqc-handshake-energy.md`). Reposition the contribution onto the crossover
and the device/link/protocol specificity, citing 2602.18708 and 2610.13026 as baselines we quantify.
**Kill experiment (week 1):** Take published ML-KEM-512 M33 cycle counts (SLOTHY lineage / pqm4) and
published EFR32xG24 datasheet currents, plus the ML-KEM message sizes and a Thread MTU, and compute a
back-of-envelope compute-energy vs radio-energy ratio for one handshake. If compute already dominates or
the ratio is near 1, the premise is wrong before any code is written — half a day.
**Measurements:** lattice compute energy = handshake cycles × active-current-per-MHz (estimate — calibrate
against SLOTHY/pqm4 M-class cycles, re-cost to M33 via `mcu-cost-estimator`); radio energy = (handshake
bytes + fragmentation overhead + retransmissions) × TX-energy-per-byte (estimate — calibrate against
EFR32xG24 datasheet, VERIFY); totals and the crossover vs a compact-ciphertext KEM.
**Semester plan:** (1) the envelope kill-check + message-size/fragmentation model for Thread; (2) emulate
ML-KEM-512 on QEMU M33 for cycle→energy, assemble the compute-vs-radio split; (3) add a compact-ciphertext
KEM and locate the crossover; (4) write up with the millijoule breakdown figure.
**DRP phase 2:** Real EFR32xG24 radio-energy measurement to replace the datasheet model; side-channel on
the ML-KEM implementation (masked Kyber on Cortex-M is an active area) for a strong phase-2.
**Group split:** PQC implementation/emulation / radio & fragmentation model / energy cost model /
evaluation & crossover writing.
**Scores (1–5):** novelty **3**, threat-model clarity **4**, falsifiability **5**, feasibility **4**,
DRP continuity **4**, group split **4**.

---

## 4. Dropped / deprioritised candidates

- **D5 — Low-stack PQC signatures for firmware/OTA authentication.** **Dropped (collision-adjacent).** The
  stack crossover ("HAETAE/HAWK fits where ML-DSA-87 doesn't") is already published (ePrint 2026/635 =
  arXiv 2604.15868; "HAETAE on a Diet" ePrint 2026/2578), HAETAE energy on Cortex-M4 is largely covered
  (ePrint 2026/2066), and PQC secure-boot/OTA framing exists (ePrint 2026/2149). Hash-based signatures
  (LMS/XMSS, NIST SP 800-208, CNSA 2.0; ePrint 2022/1198) are the incumbent firmware-signature choice, so
  the project would have to argue *lattice over hash-based* — a burden, not an open question. Residual is
  incremental. (log: `collision-cand5-lowstack-pqc-firmware-sig.md`)
- **D4 — PQ secure-aggregation cost on a constrained MCU.** **Deprioritised (feasibility).** The
  *measurement* question is genuinely open — no PQ-secagg paper benchmarks a *client* on a Cortex-M MCU
  (closest is a Raspberry-Pi-class Cortex-A72, arXiv 2601.01053) — and the "KEM-swap only PQ-protects the
  channel, not the DL-structured mask" point is only partly pre-empted by SecAgg++ (ePrint 2026/2497).
  But implementing a full secure-aggregation protocol (Shamir sharing, dropout recovery, masking) correctly
  in one semester is heavy for an undergraduate team. Keep as a stretch/fallback; it pairs naturally with
  D2's phase 2. (log: `collision-cand4-pq-secagg-mcu-cost.md`)
- **D6 — Secure aggregation doesn't zero leakage at small N (low-dim sensor data).** **Deprioritised
  (strong partial).** Elkordy et al., "How Much Privacy Does FL with Secure Aggregation Guarantee?" (PoPETs
  2023, popets-2023-0030 / arXiv 2208.02304) already establishes that leakage drops ~linearly in N under
  honest-but-curious single-server SA — so the qualitative message "you still need DP at small N" is not
  novel. The only remaining sliver is the conjunction {low-dimensional sensor data + a calibrated,
  application-pinned N-threshold + per-device signal reconstruction}, which overlaps D2's skill set.
  Better absorbed as an extension of D2 than run standalone. (log: `collision-cand6-secagg-smallN-leakage.md`)

---

## 5. Verification log

### 5.1 Papers — verified identity & reading depth
All verified via the reader/collision agents against ePrint/arXiv/DOI landing pages unless marked.

**Deep-read this round (notes in `research/papers/`):**
- `slothy-mlkem-mldsa` — ePrint **2025/366**, AsiaCCS 2025 (corrected from "TCHES"), Abdulrahman/Kannwischer/Lim. VERIFIED, full-read. M7 numbers; **no M33**.
- `pqc-iot-fragmentation` — Future Internet **18(6):316**, DOI 10.3390/fi18060316, Sakk et al., 2026. VERIFIED id; **abstract-only** (MDPI 403 — tables unread).
- `pq-edhoc` — IEEE Access 2025, DOI **10.1109/ACCESS.2025.3554010**, Fraile et al. (Zenodo 18175014). VERIFIED, skimmed. Note: the "~80% less energy, ML-KEM-512+HAWK1" figure is from **this** paper, not the consumer-IoT eval — read the PDF body before quoting the number.
- `low-stack-haetae` — arXiv **2604.15868** (= ePrint **2026/635**), Banegas/Kim/Seo/van Vredendaal. VERIFIED, full-read.
- `pqc-consumer-iot-eval` — Internet of Things (Elsevier) vol 33, 2025, art **101650**, DOI 10.1016/j.iot.2025.101650, Hanna et al. VERIFIED citation; **abstract-only** (paywalled). Mis-attribution of the "80%" claim corrected.
- `sok-transciphering` — ePrint **2025/669**, TCHES 2025 i3 pp583-613, Niu et al. VERIFIED; skimmed (PDF 403 — some figures from snippets, flagged in note).
- `transistor-tfhe-cipher` — ePrint **2025/282**, CRYPTO 2025. VERIFIED; skimmed (PDF 403 — LFSR/FSM sizes "NOT CONFIRMED").
- `lightweight-ckks-client` — ePrint **2025/720**, ACM ASIACCS 2026 (DOI 10.1145/3779208.3785268), Cheon/Kang/Park. VERIFIED; abstract-only (PDF 403).
- `hheml-edge` — arXiv **2510.20243**, Chan/Yang/Shen et al., 2025. VERIFIED, full-read. "Edge" = PYNQ-Z2 FPGA.
- `microsecagg` — PoPETs **2024.3** pp246-275, DOI 10.56553/popets-2024-0077, ePrint **2022/714**, Guo et al. VERIFIED, full-read (title/issue corrected).
- `ts-inverse` — arXiv **2503.20952**, IEEE SaTML 2025, Meijer et al. VERIFIED, full-read. (reader flagged & discarded an earlier corrupt-PDF hallucination.)
- `sok-gradient-inversion` — USENIX Security 2025 pp6439-6459, Carletti et al. VERIFIED, full-read. (literature-only SoK — not a cost source.)
- `ondevice-training-cortexm` — arXiv **2407.10734**, IEEE TCAD 2024, DOI 10.1109/TCAD.2024.3484354, Deutel et al. VERIFIED, skimmed. (no M33; some headline % numbers unconfirmed in v1.)
- `secagg-pq` — ePrint **2026/2497**, AISec 2026, Perez et al. (FBK/U.Trento). VERIFIED; abstract-only (PDF 403 — masking internals unread).

**Surfaced by reference-follow / collision hop (verified by collision agents; not deep-read):** arXiv
2602.18708, 2610.13026, 2607.23007, ePrint 2026/2557; MDPI MAKE 10.3390/make8100320, ePrint 2026/1936,
arXiv 2510.03319, 2506.15711, 2609.02971, NDSS-25 CENSOR; SEAL-Embedded (TCHES 2021), arXiv 2610.08301,
ePrint 2024/1919, 2026/1485, 2026/1314, 2026/112; arXiv 2601.01053, ePrint 2026/2394, arXiv 2505.05751;
ePrint 2026/2578, 2026/2066, 2026/2149, 2022/1198, arXiv 2106.05577; Elkordy PoPETs 2023
(popets-2023-0030 / arXiv 2208.02304), arXiv 2303.03908, 2106.03328, 2603.19340, 2603.26417.

### 5.2 Queries run (orchestrator)
Harvest batch 1: (1) "ML-KEM ML-DSA Cortex-M4 Cortex-M33 benchmark stack usage 2025 pqm4"; (2) "post-quantum
EDHOC OSCORE constrained IoT ciphertext size 2025 2026"; (3) "transciphering HE-friendly cipher
microcontroller FHE IoT client encryption 2025"; (4) "gradient inversion attack federated learning sensor
time series IoT 2025 2026"; (5) "post-quantum secure aggregation federated learning lattice 2025 eprint".
Harvest batch 2: (6) "differential privacy federated learning microcontroller energy accuracy IoT sensor
2025"; (7) "secure aggregation constrained device microcontroller cost communication 2025 2026"; (8) "CKKS
client-side encryption embedded edge device cost RAM 2025 homomorphic"; (9) "post-quantum DTLS TLS handshake
IoT energy LoRaWAN Thread Matter 2025 2026"; (10) "on-device training federated learning Cortex-M
microcontroller TinyML privacy 2025". Reference-follow: (11) "HE-friendly cipher PASTA FiLIP Transistor
benchmark microcontroller Cortex-M embedded client cost transciphering"; (12) "gradient inversion
low-dimensional tabular sensor data defense differential privacy cost on-device microcontroller 2026";
(13) "secure aggregation small number clients privacy leakage aggregate inversion threshold federated
learning"; (14) "post-quantum secure aggregation Cortex-M microcontroller embedded implementation cost
benchmark lattice Kyber masking".
Per-candidate collision queries (≥4 each, technical/plain/ePrint/neighbouring + recency): see the full
verbatim lists in `research/collision-cand1..6-*.md`.

### 5.3 Not yet checked (carry into phase 2 before any publication)
- Full texts behind 403 this round: ePrint 2025/669 tables, 2025/282 body, 2025/720 body, 2026/2497 body,
  2026/2394 body, 2026/1314, 2026/112; IoT-journal 101650 body. Read these before citing any number from
  them or finalising D3/D4 novelty.
- The "~80% energy" PQ-EDHOC figure: confirm from the IEEE Access / Zenodo PDF body.
- D1 baselines arXiv 2602.18708 and 2610.13026: deep-read to state precisely what energy split each gives.
- All EFR32xG24 datasheet electrical numbers (clock, RAM, active mA/MHz, radio TX mA): VERIFY from the
  official datasheet before any energy estimate — none are currently in hand.

---

*Next step options:* begin D3 (cleanest, most feasible) with the week-1 kill experiment; or run the
`mcu-cost-estimator` skill to turn D3's or D1's measurement plan into concrete emulated numbers.
