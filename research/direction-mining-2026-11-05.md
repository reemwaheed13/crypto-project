# Direction mining — CS 5803 IoT crypto project — 2026-11-05

Full round across PQC, FHE and FL-privacy for IoT. 14 papers read in depth (notes in `research/papers/`),
one-hop reference follow, 6 candidate directions collision-checked (logs in `research/collision-cand*.md`).
Selection rule used this round: **top directions by score, overall** (families not forced).

> Conventions: **KNOWN** = from a cited, verified source. **CONJECTURE** = our hypothesis, unproven.
> **ASSUMPTION** = a modelling choice we adopt. No MCU number below is measured — any MCU figure named
> in a plan is a target to obtain via the `mcu-cost-estimator` skill, labelled "(estimate — to calibrate
> against <source>)". IDs were verified by the reader/collision agents; items still behind paywalls are
> flagged in the verification log. "Open as far as searched" never means "proven open".

## Target device: verified datasheet numbers (added 2026-11-10)

**Part.** The EFR32xG24 Dev Kit (xG24-DK2601B, board BRD2601B) carries the **EFR32MG24B310F1536IM48-B**. Source caveat: the kit user guide (UG524) is *not* in `research/papers/` and its Silicon Labs URL returned HTTP 403. The part number therefore comes from the Zephyr board documentation for xg24_dk2601b, not from the user guide. Confirm it against UG524 (or the chip marking) once the guide is added. If the group's kit is a different board (e.g. a BRD4186C radio board), the row below changes.

**Source.** *EFR32MG24 Wireless SoC Family Data Sheet*, Rev. 1.2 (local `research/papers/efr32mg24-datasheet.pdf`). Page numbers are the datasheet's own page numbers.

| Quantity | Value for EFR32MG24B310F1536IM48-B | Datasheet row |
|---|---|---|
| Core | Cortex-M33 with DSP extensions, FPU, TrustZone | §1 Feature List (p. 2); block diagram (p. 1) |
| Max CPU clock | **78 MHz** (needs VSCALE2 and 1 flash wait state, WS1; with 0 wait states the max is 40 MHz) | Table 4.2 General Operating Conditions, f_HCLK (p. 24) |
| Flash / RAM | **1536 kB / 256 kB** | Table 2.1 Ordering Information (p. 3) |
| Max TX power (this OPN) | **10 dBm** (the 19.5 dBm PA is only on the x20 OPNs) | Table 2.1 (p. 3) |
| Secure Vault level | **High** | Table 2.1 (p. 3); feature list Table 3.1 (p. 13) |
| Active current, EM0, DC-DC at 3.0 V, 78 MHz (HFRCO + DPLL, VSCALE2) | **33.3 µA/MHz** running Prime from flash; 32.8 µA/MHz while-loop; **49.1 µA/MHz** CoreMark | Table 4.5 (p. 29) |
| Radio TX current, 2.4 GHz CW, DC-DC 3.0 V, VSCALE2 | **5.2 mA** at 0 dBm (0 dBm PA); **19.2 mA** at 10 dBm (10 dBm PA) | Table 4.6 I_TX (p. 33) |
| Radio RX current, 802.15.4, DC-DC 3.0 V, VSCALE2 | **5.7 mA** receiving a frame; **5.6 mA** listening | Table 4.6 I_RX_ACTIVE (p. 32), I_RX_LISTEN (p. 33) |
| 802.15.4 data rate | **250 kbps** O-QPSK DSSS | §1 Feature List (p. 2); 802.15.4 RX table is Table 4.20 (p. 55) |
| Hardware crypto (SE Manager API) | ECDSA P-256 sign **5.9 ms**, verify **6.2 ms**; AES-128-CTR 1 kB **474 µs**; SHA-256 1 kB **308 µs** | Table 4.53 (p. 92–93) |
| CAN / CAN FD controller | **none listed** (the word "CAN" as a bus peripheral does not appear in the datasheet) | §3 System Overview peripheral list; full-text search |

**Derived constants. These are arithmetic from the rows above, not datasheet values (ASSUMPTION: 3.0 V supply, DC-DC, VSCALE2, typical values).**
- Energy per CPU cycle = I/MHz × V = 33.3 µA/MHz × 3.0 V ≈ **0.100 nJ/cycle** (Prime) to **0.147 nJ/cycle** (CoreMark). Use the range, because crypto code sits between the two workloads.
- Time on air per byte at 250 kbps = **32 µs**. TX energy per byte ≈ 5.2 mA × 3.0 V × 32 µs ≈ **0.50 µJ/B at 0 dBm** and **1.84 µJ/B at 10 dBm**. RX ≈ **0.55 µJ/B**. These exclude preamble/PHY header, MAC header, ACKs, CSMA back-off and radio wake-up, which are protocol-dependent and must be modelled separately.
- So one transmitted payload byte costs as much energy as roughly **3,400–5,000 CPU cycles at 0 dBm** and **12,500–18,400 cycles at 10 dBm**. This ratio drives every compute-vs-radio crossover in these reports.
- The TX current rows are continuous-wave measurements, not 802.15.4 packet measurements, and they were taken with the MCU in EM1 at 39 MHz (Table 4.6 header, p. 32). Treat them as radio-only adders. The phase-2 kit measurement replaces them.

---

## 1. Bottom line

> **Update 2026-11-10 (full-text checks).** D3's verdict moves from *Open* to *Partial overlap*: the SoK's Table 4 and
> HHE Kombat already rank HE-friendly ciphers by *client* cost, on Xeons. Its re-score is 27, and its kill experiment
> has not run yet (harness ready). D2's re-score is 26 (Sudars et al. 2025 already tests DP against time-series gradient
> inversion; the on-device cost against a learned attacker remains open). EFR32MG24 datasheet numbers are filled in
> under *Target device*. Side-by-side comparison: `decision-summary.md`.

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
| 6 | SoK: FHE-Friendly Ciphers & Transciphering (ePrint 2025/669) | TCHES 2025 i3 | F (§5–6, local PDF, 2026-11-10) | **Server-side Xeon only** | cipher zoo; no client/embedded cost | Move to device | → **D3** |
| 7 | Transistor: TFHE-friendly stream cipher (ePrint 2025/282) | CRYPTO 2025 | S | **Server-side CPU only** | cheap-client cipher, MCU cost unproven | Move to device | → **D3** |
| 8 | Towards Lightweight CKKS: client cost (ePrint 2025/720) | ASIACCS 2026 | A | PC; keys still ~285 MB | direct CKKS client infeasible on MCU | Find crossover | → **D3** (motivation) |
| 9 | HHEML: Hybrid HE PPML on Edge (arXiv 2510.20243) | arXiv 2025 | F | **PYNQ-Z2 FPGA SoC**, not an MCU | "edge" overstated; re-cost PASTA on M33 | Move to device | → **D3** |
| 10 | MicroSecAgg (PoPETs 2024.3; ePrint 2022/714) | PETS 2024 | F | AWS EC2 (PC), ≤1024 users | classical SecAgg baseline; DH swap point | Swap primitive | → D4 (deprioritised) |
| 11 | TS-Inverse: gradient inversion for FL time series (arXiv 2503.20952) | IEEE SaTML 2025 | F | PC/GPU; smart-meter/air-quality | low-dim inversion attack; no on-device/DP/SA | Change data + attack defence | → **D2** |
| 12 | SoK: Gradient Inversion Attacks in FL (USENIX Sec 2025, Carletti) | USENIX 2025 | F | Literature only (no re-runs) | image-centric; low-dim & small-N SA open | Change data | → D2, D6 |
| 13 | On-Device Training of Quantized DNNs on Cortex-M (arXiv 2407.10734) | IEEE TCAD 2024 | F (v2 PDF, 2026-11-10) | Real M0+/M4/M7 (no M33) | on-device gradients are real; cost anchor | Account for system | realism anchor for D2 |
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
radio-energy-per-byte (estimate; constants from the EFR32MG24 datasheet, see *Target device*: 0.100–0.147 nJ/cycle,
0.50 µJ/B TX at 0 dBm or 1.84 µJ/B at 10 dBm, 250 kbps).
Crossover = payload size where transciphering-energy = AEAD-baseline-energy.
**Semester plan:** (1) harness + AEAD baseline + two ciphers, reproduce server ordering as a sanity check;
(2) add the remaining HE-friendly families, produce the client-cost ranking table; (3) add the radio-energy
model and locate the crossover; (4) write up, with an explicit "server-optimal ≠ client-optimal" figure.
**DRP phase 2:** Port the winning cipher(s) to the real EFR32xG24 kit, measure true energy, then a
side-channel study of the symmetric client encryption (power/EM on the keystream) — a live topic
(ePrint 2026/1485 studies SCA-protected PASTA), giving a clean phase-2 that matches the instructor's hint.
**Group split:** cipher implementers (split the cipher zoo) / MCU cost-model & harness / direct-HE baseline
& crossover analysis / evaluation & writing.
**Scores (1–5), round 1 (superseded):** novelty **4**, threat-model clarity **4**, falsifiability **5**, feasibility **5**,
DRP continuity **5**, group split **5**.

**Full-text update (2026-11-10).** The three papers that were 403-blocked are now read from local PDFs. The SoK
(ePrint 2025/669) was read in full for §5–6. HHE Kombat (ePrint 2026/1314) was read in its client-side sections;
see `papers/hhe-kombat.md`. PETCHA (ePrint 2026/112) was searched for client or embedded costs and has none.
- **Claim (a) as worded is KNOWN, on a Xeon.** SoK Table 4 (p. 20) measures *client-side* cycles for 23 cipher
  settings, and its §6(2) says outright that Rubato, the server-side winner, is slow on the client. HHE Kombat measures
  184 client-side encryptions (Table 15 lists the winners) on an Intel Xeon Gold 6130. Neither runs on an MCU,
  phone or embedded board. The round-1 note on the SoK wrongly said client cost was unmeasured; this is corrected in
  `papers/sok-transciphering.md`.
- **Revised collision verdict: Partial overlap** (was "Open as far as searched"). Open as far as searched:
  (a′) the client ranking *on a Cortex-M33*, and whether it differs from the Xeon client ranking (32-bit core, no
  AVX2 Keccak, software 64-bit modular reduction, 256 kB RAM). Also (b) the radio-inclusive crossover against an AEAD
  for small sensor frames.
- **Why (b) is more interesting than it looked (CONJECTURE, back-of-envelope).** One 802.15.4 byte at 0 dBm costs
  about as much energy as 3,400–5,000 CPU cycles (see *Target device*). On the Xeon, Pasta-4 already spends about
  3.6×10³ cycles per byte (SoK Table 4), and FiLIP-DSM about 1.6×10⁶. So on an MCU the keystream compute for several
  HE-friendly ciphers plausibly costs *more* energy than transmitting the bytes, inverting the usual "radio
  dominates" intuition. The EFR32MG24's hardware AES (AES-128-CTR, 1 kB in 474 µs, Table 4.53) makes the AEAD
  baseline even cheaper.
- **Kill experiment status: NOT RUN.** A Cortex-M33 instruction-count harness is written in
  `experiments/d3-transciphering-client/mcu/`. It has a QEMU `mps2-an505` TCG plugin that counts instructions between markers,
  stack painting, a malloc high-water wrapper, and known-answer-test checks. It covers 14 of the SoK's Table 4 cipher
  settings via the IAIK hybrid-HE-framework, plus tiny-AES-c and Ascon. Running it was blocked because compiling and
  executing the downloaded third-party reference code needs the user's approval. **No M33 ranking exists yet**, so
  there is no comparison with the SoK's Xeon ranking to report.
- **Biggest remaining risk:** the M33 ranking may simply match the Xeon ranking (same reference code, same
  PRNG-dominated cost structure). Then (a′) is dead and D3 rests on the crossover (b) alone.

**Kill experiment result (2026-11-10): FAILED.** (a′) M33 client ranking = Xeon client ranking (Spearman ρ = 0.976 over
8 shared settings; one adjacent swap, not robust). The ranking holds even if all software 64-bit division is removed.
(b) No crossover against Ascon-AEAD128: every HE-friendly cipher costs more at every payload from 16 to 1024 B (Kreyvium
1.2–2×, HERA-5 1.7–4×, Pasta-4 8–27×, FiLIP more than 10³×). Dasta-6 and Rasta-5 also exceed 256 kB RAM. Novelty → 2,
total → **26**. Full report: `cost-he-friendly-ciphers-2026-11-10.md`.

**Scores (1–5), after full-text check (2026-11-10, superseded by the kill result above):** novelty **3** (client ranking now KNOWN on a Xeon; only the MCU
ranking and the radio crossover remain), threat-model clarity **4**, falsifiability **5**, feasibility **5**, DRP
continuity **5**, group split **5**. **Total 27** (was 28). Re-score once the kill experiment has run.

---

### D2 — What does private federated learning cost a sensor? DP, secure aggregation, and gradient inversion on a Cortex-M33

*(Retitled 2026-11-11; original title: "What does it actually cost a sensor node to defeat gradient-inversion
reconstruction?")*

> **Crypto add-on (2026-11-11, `d2-crypto-addon-2026-11-11.md`).**
>
> **Threat model, extended.**
> - The honest-but-curious server now also faces **secure aggregation (SecAgg)**: pairwise X25519 key agreement,
>   ChaCha20 mask expansion, and Shamir-shared seeds for dropouts (Bonawitz et al.; SecAgg+ with about 2 log₂N
>   neighbours), so it sees only the sum.
> - DP still covers what the sum reveals, plus servers that exploit dropouts or collude.
> - SecAgg is costed on top of DP, not compared against it.
>
> **Gap (open as far as searched):** the measured per-round cost to a Cortex-M sensor of DP-SGD *and* secure aggregation
> together (compute, RAM, 802.15.4 frames, energy), and how it scales with N. Prior SecAgg costs are on servers or a
> Raspberry Pi 4, without DP (`collision-r4-secagg-dp-mcu-cost.md`, partial overlap).
>
> **Estimate** (MEASURED instruction counts × ASSUMPTION/KNOWN energy):
> - **SecAgg+:** masking plus X25519 (411,061 cycles on the M33, ePrint 2025/523 abstract) costs about 0.4–0.7× the
>   DP-SGD round at N = 10–100.
> - **Full pairwise SecAgg:** 0.5×, 1.9× and 4.7× at N = 10, 40 and 100.
> - **Radio:** the 43 mJ upload (32-bit) still dominates.
> - **Integer noise:** the *switch to constant-time integer DP noise* that SecAgg requires costs up to **12–37× the DP step** with
>   a naive constant-time CDT (MEASURED, N = 10–100). This is an upper bound: σ-independent samplers (FACCT,
>   Micciancio–Walter) are unmeasured. It is plausibly the largest crypto line item.
>
> **Applied-crypto contribution:** seven candidates were checked (mask PRG choice, modulus/frames, constant-time Shamir,
> seed KDF lifecycle, accelerator-fed noise, streaming masking, distributed constant-time sampler). **None is Open**
> (five partial overlaps, one of them also failing our measurement; the one literature-Open candidate fails on substance), so none is claimed.
>
> **Phase 2 is unchanged:** power analysis of the PRNG and noise routine on the EFR32 kit.


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
**Scores (1–5), round 1 (superseded):** novelty **4**, threat-model clarity **5**, falsifiability **5**, feasibility **4**,
DRP continuity **4**, group split **5**.

**Full-text update (2026-11-10). Reading depth: full text** for all D2 papers held locally: MDPI MAKE 8(10):320,
TS-Inverse (arXiv 2503.20952), the GIA SoK (USENIX Sec 2025) and Deutel et al. (arXiv 2407.10734 v2). Notes:
`papers/make8100320-ondevice-not-private.md` (new) and full-text addenda in `papers/ts-inverse.md`,
`papers/sok-gradient-inversion.md` and `papers/ondevice-training-cortexm.md`. Collision re-check (13 queries):
`collision-r3-D2-sensor-gradinv-defence-cost.md`. VAHANA (ePrint 2026/1936) and arXiv 2609.02971 are **not** in
`research/papers/` and were checked from abstracts only.
- **make8100320 is not a collision (KNOWN, full text).** It studies membership inference only, with centralised
  training on a PC, multivariate anomaly-detection windows (TSB-AD-M, 64 steps), clipping C=1 plus σ=1 noise with no
  ε accounting (attack AUC 0.688→0.508 at about 0.05 detection-AUC cost, Table 4), and costs on a desktop Ryzen
  5 5500GT. It has no FL, no reconstruction and no MCU. Its future work (on-MCU memory/energy, formal DP) makes the
  same group a possible competitor on the *cost* half of D2.
- **TS-Inverse already answers part of the old kill experiment (KNOWN).** Reconstruction still works at batch
  B = 4 (Figs 9–10), though only under single-step FedSGD. Its "gauss" defence (no σ or ε stated, Table IX) stops
  the optimisation attacks, but the *learned* LTI attack still reaches sMAPE about 0.22.
- **New nearest neighbour (KNOWN, abstract only):** Sudars, Namatēvs, Nikulins, Ozols, "Privacy Auditing of Lithium-Ion
  Battery Ageing Model by Recovering Time-Series Data Using Gradient Inversion Attack in Federated Learning",
  Applied Sciences 15(10):5704, 2025. It runs gradient inversion on battery time series with DP noise and finds the
  reconstruction error plateaus at a level set by the privacy budget. No device cost is mentioned. So "does DP stop
  reconstruction on time series" is no longer open; the *on-device cost* of the DP setting that does it is.
- Other neighbours, from abstracts or HTML: SVDefense (arXiv 2510.03319: HAR data, Raspberry Pi / Jetson latency
  only), VAHANA (ePrint 2026/1936: noise-defence cost on a RISC-V softcore, images) and EPC-TinyAD (Electronics
  15:2879: no attack, no ε, no hardware cost). Unresolved: a ResearchGate item claiming DP tests on a Cortex-M7
  (ID 390947565) could not be verified.
- **Revised collision verdict:** **Partial overlap.** Open as far as searched is the *per-training-step cost on a
  Cortex-M33* (cycles, RAM, energy) of the clipping + noise setting at an *accounted ε* (or the sparsification
  level) that keeps reconstruction of low-dimensional smart-meter / air-quality windows above a usefulness
  threshold *against a learned, defence-aware attacker (LTI-style), not only optimisation attacks*, together with the
  accuracy it costs. No paper found measures DP or sparsification cost per step on any Cortex-M part.
- **Revised claim (falsifiable):** the ε needed to stop a learned attacker on these windows costs either (a) more
  forecast accuracy than a stated threshold (a few % sMAPE), or (b) a per-step overhead on the M33 that is not
  small next to the training step itself. The calibration anchor is CWRU 1×512 transfer learning on an M4: 75.79 ms
  and 2.18 mJ per sample (Deutel et al., Fig. 5). If both (a) and (b) are small, a cheap defence suffices, which is
  still a publishable result once quantified.
- **Revised kill experiment (week 1):** reproduce TS-Inverse and LTI on Electricity-370 / London smart-meter at
  B = 4 with multi-step FedAvg (5 local steps), then add DP-SGD at ε ∈ {1, 8} with an accountant. If the learned
  attacker is already defeated at ε = 8 *and* accuracy loss is below 1% sMAPE, the trade-off is trivial and the
  direction shrinks to a measurement note.
- **Biggest remaining risk:** the cost half may be thin. Clipping plus Gaussian noise is O(model size) per step and
  may be negligible next to backpropagation. To guard against this, also cost the RNG (a CSPRNG for Gaussian draws on the M33) and RAM for
  per-example clipping, which is where an MCU is plausibly expensive (CONJECTURE).

**Scores (1–5), after full-text check (2026-11-10):** novelty **3** (Sudars 2025 and TS-Inverse Table IX take the
"DP vs time-series reconstruction" part; on-device cost against a learned attacker remains), threat-model clarity
**5**, falsifiability **5**, feasibility **4** (needs GPU time to train LTI per ε, but no hardware), DRP continuity
**4**, group split **5**. **Total 26** (was 27).

---

**Kill experiment result (2026-11-10/11): SURVIVES** (`d2-kill-2026-11-10.md`). On Electricity-370, under 5-step FedAvg
with B = 4, the learned LTI attacker leaks without DP (sMAPE 0.25 against the prior's 0.45). At ε = 8 the optimisation
attack is defeated, but LTI is still about 2 SE better than the prior, and accuracy drops 1.3 points. At ε = 1 both
attacks are defeated, at a cost of 5.4 points. On the M33 the DP step costs 4.85× the plain step, and 66% of that is
noise. N1 sampler work package (`n1-kill-2026-11-11.md`): the float32 pair-inversion break does **not** carry over to
this 5-step setting. What stays in D2 is the tail-cap accounting, the instruction-path leak (phase-2 power analysis of
the noise routine) and the cost of a constant-time sampler. Score **26** (novelty 3, threat model 5, falsifiable 5,
feasible 5, DRP 4, split 4).

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
crossover on M33/Thread/EDHOC specifically. KNOWN (datasheet, *Target device*) — MCU active current 33.3–49.1 µA/MHz at 78 MHz
(Table 4.5), radio TX 5.2 mA at 0 dBm / 19.2 mA at 10 dBm (Table 4.6), 250 kbps 802.15.4. ASSUMPTION — a 6LoWPAN MTU and a
retransmission model.
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
bytes + fragmentation overhead + retransmissions) × TX-energy-per-byte (estimate — TX 0.50 µJ/B at 0 dBm or 1.84 µJ/B at 10 dBm,
derived from datasheet Table 4.6 and the 250 kbps rate); totals and the crossover vs a compact-ciphertext KEM.
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
- ~~All EFR32xG24 datasheet electrical numbers~~ **Done 2026-11-10:** filled from the EFR32MG24 datasheet Rev. 1.2
  (see *Target device*). Still open: the kit part number must be confirmed from the UG524 user guide, which is not in hand.

---

*Next step options:* begin D3 (cleanest, most feasible) with the week-1 kill experiment; or run the
`mcu-cost-estimator` skill to turn D3's or D1's measurement plan into concrete emulated numbers.
