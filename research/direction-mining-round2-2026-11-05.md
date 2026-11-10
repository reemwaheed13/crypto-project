# Direction mining, round 2 (novelty-focused): CS 5803 IoT crypto project, 2026-11-05

Second round, run after `direction-mining-2026-11-05.md`. There are two separate lanes. **Lane A** stays inside
PQC / FHE / FL privacy. **Lane B** uses other primitives, and every Lane B card needs instructor approval.
22 harvest queries plus 3 reference-follow queries were run. 10 papers were read by paper-reader agents (notes in
`research/papers/`). 8 candidates were collision-checked (logs in `research/collision-r2-*.md`).

> Conventions are unchanged from round 1. **KNOWN** = from a cited, verified source. **CONJECTURE** = ours, unproven.
> **ASSUMPTION** = a modelling choice. No MCU number in this report is measured. Every cost figure is either
> quoted from a paper (and labelled with it) or is a target to obtain with the `mcu-cost-estimator` skill,
> written "(estimate, calibrated against <source>)". "Open as far as searched" never means "proven open".
> **Reading-depth warning:** this round, many ePrint, MDPI and TCHES PDFs returned HTTP 403 to the agents, so several
> neighbours were read as abstracts only. Each card names the full texts that must be read before committing.

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

> **Update 2026-11-10 (full-text checks).** app16083705 does **not** discuss batching, Merkle trees or amortised
> signatures, so A4's flagged risk is cleared. A new neighbour, CAIBA (EuroS&P 2025), already gives *symmetric* source
> authentication on CAN, so A4 must argue why signatures are needed. Re-scores: D3 27, D2 26, A4 25; the order is
> unchanged. Comparison: `decision-summary.md`.

The novelty push produced **four survivors out of eight candidates, all Partial overlap**. None is clean
greenfield, and two Lane A candidates were dropped.

- **Lane A, best: (A4) amortised post-quantum source authentication of CAN FD sensor frames.** As far as
  searched, no paper signs a Merkle root or a TESLA-style anchor over a window of CAN FD frames with a PQ signature.
  The nearest work measures *per-message* PQ signatures on CAN. The threat model is crisp: a compromised ECU that
  already holds the shared SecOC keys.
- **Lane A, second: (A3) whether a Cortex-M33-class V2X unit can sign with FN-DSA (Falcon) at 10 messages per second.**
  It is narrower than hoped, because the M4 results largely carry over to the M33. Its DRP fit is the best of the
  round, since the EFR32xG24 *is* an M33 and Falcon's sampler is a classic side-channel target.
- **Lane B: (B3) how long a duty-cycled IoT group stays compromised under MLS/TreeKEM, and (B4) anonymous BBS
  credentials presented from a Cortex-M33 whose secure element only does ECDSA.** Both are open as far as searched
  at the device level. Both need approval because they are outside the course scope.

In the combined ranking, round 1's **D3 and D2 still lead**. **A4 ranks third, ahead of round 1's D1.**

---

## 2. What was treated as already taken

These were not re-proposed. Each card below says why it is distinct from them.

| Source | Item |
|---|---|
| Round 1 D1 | PQ EDHOC/DTLS handshake, compute-vs-radio energy and the KEM-size crossover on M33/Thread |
| Round 1 D2 | On-device cost of defeating gradient-inversion reconstruction in sensor FL |
| Round 1 D3 | HE-friendly cipher client cost and the transciphering crossover |
| Round 1 D4 | PQ secure-aggregation client cost on an MCU |
| Round 1 D5 | Low-stack PQC signatures for firmware/OTA verification |
| Round 1 D6 | Secure-aggregation leakage at small N |
| Instructor | Gradient leakage in FL, and the cost of DP and secure aggregation on constrained devices |

---

## 3. Lane A: inside PQC / FHE / FL privacy

### 3.1 Mining table (Lane A)

Depth: F = full text, S = partial or snippets, A = abstract only. "Measured on" is what the paper actually ran on.

| # | Paper (verified ID) | Status | Depth | Measured on | Seed | Move | → Candidate |
|---|---|---|---|---|---|---|---|
| A-p1 | Deployment feasibility of PQ signatures in C-V2X (arXiv 2608.05087) | arXiv only | F | **PC simulation** (OpenCV2X/Veins); sign and verify time excluded by design | Falcon-512 is the only PQ signature under the 2,481 B transport block; signer cost never measured | Move to device | → **A3** |
| A-p2 | "Flying, Not Floating": fixed-point FN-DSA (ePrint 2026/2431) | ePrint only | A (PDF 403) | Cortex-M4 | ~45% fewer cycles than emulated double; RAM 59n→83n B; no M33 | Move to device | → **A3** |
| A-p3 | PQ-CAN simulation framework (arXiv 2504.10730) | arXiv only | F (PDF, 2026-11-10) | **One PC**, Docker containers capped to ECU clock shares, classical CAN 2.0 | Per-message PQ signatures only; no CAN FD, no amortisation, no deadline metric | Account for whole system + crossover | → **A4** |
| A-p4 | Li & Micciancio, IND-CPA-D on CKKS (ePrint 2020/1533) | EUROCRYPT 2021 | A (PDF 403) | HE libraries on PC | Needs the full decrypted value to recover the key | Change threat model | → A1 (dropped) |
| A-p5 | Federated unlearning in edge networks, survey (arXiv 2601.09978) | arXiv only | F (HTML) | none (survey) | Unlearning privacy risk described only in general terms | Change threat model | → A2 (dropped) |
| A-p6 | When Machine Unlearning Jeopardizes Privacy (arXiv 2005.02205) | ACM CCS 2021 | F | Xeon server; tabular and image data, centralised | Before/after-deletion membership inference | Change data | → A2 (dropped) |

Harvest hits triaged out without a deep read. IDs in this paragraph are taken from search-result pages only and
were **not verified by an agent**: HQC on Cortex-M4 (arXiv 2608.18145) and hybrid KEM benchmarks on
M4 (ePrint 2026/1414) were too close to D1/D5. PQ smart-meter migration (DLMS UA position paper, PQ-REACT pilot)
had no technical paper to build on. The MIT PQC biomedical ASIC (news, April 2026) is hardware, not software.
LoRaWAN PQ root-key renewal (SBSeg, CSIDH-based) is too close to D1. Property inference and split learning in
HAR/ECG (pre-2025 work only) is gradient-leakage-adjacent. TinyGuard robust aggregation (arXiv 2602.02615) found no
MCU angle. SecureBadger HE wearable inference sits on the D3 side.

### 3.2 Direction cards (Lane A, top 2)

#### A4: Can a PQ signature authenticate a *stream* of CAN FD frames within a 10 ms deadline?

**Move used:** Account for the whole system (bus, deadlines) + find the crossover (window size).
**Application:** In-vehicle ECUs (Cortex-M class) broadcast periodic sensor and control frames on CAN FD:
64-byte payloads with periods of 1–100 ms. Today frames are protected with AUTOSAR SecOC: short MACs
under keys that several ECUs share.
**Threat model:** Asset = *source* authenticity of each frame ("this braking-pressure frame really came from the
brake ECU"). Adversary = an attacker who has compromised one ECU on the bus. Because SecOC keys are shared, that
ECU can forge MACs for others, so MACs give no source authentication against it. A quantum forger later is also
in scope, for signing keys that live as long as the vehicle (15+ years). Out of scope: bus flooding (DoS) and
side channels (phase 2). We measure authentication latency, bus-load overhead and loss tolerance.
**Claim (falsifiable):** Signing every frame with a PQ signature is impossible within CAN FD's bus budget. If the
sender signs one Merkle root (or TESLA-style anchor) per window of N frames, there is a window size N* for each
scheme (ML-DSA-44 / FN-DSA-512 / SLH-DSA) that meets a 10 ms authentication deadline under ≤ X% extra bus load
on a Cortex-M ECU. CONJECTURE: once amortised, Falcon's size advantage over ML-DSA largely disappears, and
ML-DSA's cheaper, float-free signing wins on latency.
**Plain-language explanation:** A PQ signature is hundreds to thousands of bytes; a CAN FD frame carries 64.
Signing every frame would multiply bus traffic many times over. The standard trick is to *batch*. The sender
collects N frames, builds a hash tree over them, signs only the tree's root, and sends each frame with a short
proof of membership. Now one big signature is shared by N frames, but a receiver must wait for the root
signature before it can trust a frame. So N is a dial. Small N means low delay but a heavy bus. Large N means
a light bus, but frames are trusted too late for a control loop. TESLA-style schemes make a similar trade with a
delayed key reveal. The interesting point is that batching changes *which* PQ scheme wins. Falcon's main
advantage is small signatures, and once a signature is spread over many frames, size matters less while signing
time (where Falcon is awkward, because it needs high-precision floating point) matters more.
**Status of evidence:**
- KNOWN: per-message PQ signatures on CAN have been simulated (PQ-CAN, arXiv 2504.10730: one PC, classical
  CAN 2.0, Falcon-512 overhead 47–167.5 ms under their container model).
- KNOWN: Lee et al., "Design-Space Mapping of PQC Artifact Transport on CAN-FD" (Applied Sciences
  16(8):3705, 2026, doi:10.3390/app16083705; abstract only) finds FN-DSA the best *per-message* signature on CAN
  FD and SLH-DSA nearly always infeasible.
- KNOWN: generic Merkle batch signing exists (ePrint 2023/492; IETF MTL mode draft).
- KNOWN: classical TESLA-on-CAN exists (LiBrA-CAN), with no PQ anchor.
- CONJECTURE: the existence of N* and that the Falcon/ML-DSA ranking flips under amortisation.
- ASSUMPTION: CAN FD at 2–5 Mbit/s data phase; published background bus loads; Cortex-M4/M33 signing cycles
  from pqm4 / published papers.
**Collision check:** **Partial overlap.** Per-message PQ signatures on CAN and CAN FD are taken. *Amortised*
PQ source authentication of periodic CAN FD frame streams is open as far as searched: N* versus deadline and
bus load, the scheme ranking under amortisation, and the compromised-ECU-with-SecOC-keys threat model. Risk:
the full text of app16083705 (403 this round) may discuss amortisation in its guidelines, so read it before
committing. A Mitacs "PQC in CAN FDx" project could become a competitor. (Log:
`collision-r2-A4-canfd-amortised-pq-auth.md`.)
**Distinct from the exclusion list:** D1 is a KEM handshake's energy over a radio. A4 is signatures over
a wired real-time bus, with a deadline metric and a different adversary. D5 is one-shot verification of a firmware image.
**Kill experiment (week 1):** A spreadsheet model with no crypto code. Take published Cortex-M4 sign and verify
cycles for ML-DSA-44 and FN-DSA-512 (pqm4; ePrint 2026/1915 for low-RAM Falcon), their signature sizes, CAN FD
frame overheads and a published background load. Compute N* for a 10 ms deadline. If no N ≥ 1 meets the deadline
at any sane bus load, or N* = 1 already works (so batching is unnecessary), the claim is dead. Half a day.
**Measurements:** sign and verify cycles per scheme (estimate, re-costed to M33 via `mcu-cost-estimator`, calibrated
against pqm4 M4 numbers). Merkle hashing cycles per frame (same method). Bytes per window = signature + N ×
authentication path. Bus load and worst-case authentication latency from a CAN FD timing simulation (frame
arbitration, published schedules). Loss tolerance = fraction of frames still verifiable when k frames or the
root frame are dropped. No energy figure is needed: this is a wired, vehicle-powered setting.
**Semester plan:** (1) kill-experiment spreadsheet plus the CAN FD timing simulator. (2) Merkle-window and
TESLA-anchor schemes with real signing code under emulation; produce latency-vs-N and load-vs-N curves per scheme.
(3) Add frame loss and a compromised-ECU forgery analysis. (4) Write up, centred on the N* table and the
ranking-flip figure.
**DRP phase 2:** Side-channel study of the PQ signer on the EFR32xG24 (Cortex-M33). The signing core is the
same, so this transfers directly. KNOWN: the EFR32MG24 has **no CAN or CAN FD controller** (none in the datasheet's
peripheral list; see *Target device*), so bus-level timing would stay simulated unless a CAN FD board is obtained. That is the DRP weakness of this card.
**Group split:** PQ signer and Merkle/TESLA implementation / CAN FD timing simulator / MCU cost model /
security analysis and writing.
**Scores (1–5):** novelty **4** (amortised PQ stream authentication on CAN FD not found), threat-model clarity **5**
(compromised SecOC-key holder is concrete), falsifiability **5**, feasibility **4** (simulation plus emulated
signing; no hardware needed), DRP continuity **3** (signer SCA transfers; bus validation does not), group split
**5**. **Total 26.** *(Round 2 scores, superseded below.)*

**Full-text update (2026-11-10). Reading depth: full text** for the A4 papers held locally: Applied Sciences
16(8):3705 (all 40 pages), PQ-CAN (arXiv 2504.10730, PDF) and Batch Signatures Revisited (ePrint 2023/492, 11-page ePrint
version). Notes: `papers/app16083705-pqc-canfd-design-space.md` (new), `papers/batch-signatures-revisited.md` (new),
and a full-text addendum in `papers/pq-can-framework.md`. Collision re-check (15 queries, 11 sources):
`collision-r3-A4-canfd-amortised-pq-auth.md`. The IETF MTL draft, SAE 2025-01-0451, ePrint 2026/1915 and LiBrA-CAN are
**not** in `research/papers/`; they were checked from abstracts, snippets or landing pages only.
- **app16083705 does not discuss batching, Merkle trees or amortised signatures (KNOWN, full text).** None of the
  words batch, Merkle, tree, amortise, TESLA, chain or MTL occurs; "window" always means a scheduling window. The
  "artifacts" are *one-off* transfers: a startup key exchange and one signed diagnostic response. They go over
  ISO-TP at 1/5 Mbit/s with 40–80% background load, against a 50 ms completion deadline. The model is transport-only
  (§6.5: no sign or verify compute), and the adversary is a passive HNDL recorder that "does not inject or modify
  frames". It ranks FN-DSA-512 > ML-DSA-44 > SLH-DSA by *single-shot* size (Table 7b). Its future work is CAN XL /
  CANsec, vehicle traces and a hardware bench, not amortisation. **Cite it as A4's transport baseline.**
- **PQ-CAN correction (KNOWN, PDF Table II).** The "Falcon-512 overhead 47–167.5 ms" quoted above is mostly
  per-run *key generation* (43–140 ms). Signing is 0.49–0.61 ms. Do not cite it as a signing or transport cost.
- **Batch Signatures Revisited (KNOWN, full text).** Provides the Merkle batch construction (random batch id,
  randomised leaves, the inner scheme signs id‖root‖N) with a BEUF-CMA proof. Per-message overhead is 82 B at N = 16 and 98 B at
  N = 32 (Table 1). Its setting is TLS servers only (N ≤ 32 on a cloud server), with no embedded, CAN or deadline
  model, and batches fill "when the signer is ready", not from a periodic source. The abstract's 3.2× Falcon gain
  comes from the *FPU-emulation* row (Table 2). That matters for FPU-less ECUs.
- **New neighbours from the re-check.**
  - CAIBA (Wagner et al., arXiv 2504.16695, EuroS&P 2025): multicast *source authentication* on CAN against
    masquerading compromised ECUs. A trusted authenticator overwrites tags on the fly, compatible with SecOC, with
    "no communication overhead or verification delays". The mechanism is symmetric, so it is already quantum-safe.
  - arXiv 2611.08108 (Sie, 5 Nov 2026): device-signed PQ Merkle roots over IoT records, costed in Ethereum gas,
    not CAN.
  - SAE 2025-01-0451 (Smith, Owens, Kozan, GVSETS 2025): per-message PQC on in-vehicle networks; no batching seen.
  - ePrint 2026/1915 (Pornin): low-RAM Falcon-512, about 11 kB signing RAM and about 13.45 M cycles average on M4.
  - LiBrA-CAN (Groza, Murvay, Van Herrewege, Verbauwhede; CANS 2012 and ACM TECS 16(3) 2017): classical, no PQ
    follow-up found.
- **Revised collision verdict:** **Partial overlap** (unchanged). Open as far as searched: amortised *PQ-signature*
  source authentication of *periodic* CAN FD frame windows, with N* set jointly by an authentication deadline and a
  bus-load cap, including Cortex-M signing and verification time, and the FN-DSA vs ML-DSA vs SLH-DSA ranking *under
  amortisation*.
- **Biggest remaining risk: motivation, not novelty.** CAIBA already gives source authentication against a
  compromised ECU, symmetrically and with no delay. A4 must therefore argue why signatures are needed. The case is
  no trusted authenticator node on the bus and no bus-level bit manipulation, which matters for ECUs from
  different suppliers, gateways and CAN XL. Two further arguments: non-repudiation for forensic logs, and keys
  that live as long as the vehicle. The ranking-flip conjecture is also weaker now: Pornin's low-RAM Falcon removes over half the
  floating-point work, which narrows ML-DSA's "float-free" advantage.
- **Kill experiment:** unchanged (half-day spreadsheet). Add CAIBA as a baseline column: if no N* beats CAIBA on
  any axis other than "no trusted authenticator", the contribution reduces to that single argument.

**Scores (1–5), after full-text check (2026-11-10):** novelty **4** (no amortised PQ work on CAN found; app16083705
cleared), threat-model clarity **4** (was 5: CAIBA means the case for signatures must be argued), falsifiability **5**,
feasibility **4**, DRP continuity **3** (no CAN FD controller on the EFR32MG24, now KNOWN; the signer's side channel
still transfers), group split **5**. **Total 25** (was 26).

---

#### A3: Can an M33-class V2X unit sign FN-DSA at 10 messages per second, and what does it give up to do so?

**Move used:** Move to the constrained device + find the crossover (RAM vs speed).
**Application:** Low-cost V2X units built on MCUs rather than application processors: two-wheeler and
micromobility V2X modules (an SAE 2026 paper targets two-wheeler ECUs on Cortex-M) and roadside sensor units.
They broadcast signed safety messages at ~10 Hz under IEEE 1609.2 / C-V2X.
**Threat model:** Asset = authenticity of safety messages and the long-term signing key. Adversary = a radio
attacker spoofing messages now, plus a quantum forger later. Side-channel extraction of the Falcon key by a
local observer is out of scope this semester (it is the DRP). We measure sustained signing rate,
worst-case time per signature, RAM, and how many incoming messages per second the unit can verify.
**Claim (falsifiable):** On a Cortex-M33 at EFR32xG24-class clock, *no* published Falcon signing variant sustains
10 signatures per second *and* fits a ~64 KB RAM budget at once. The choices are emulated-double (Pornin), low-RAM
(ePrint 2026/1915), fixed-point (ePrint 2026/2431) or triple-word float (TCHES 2026), each with or without the
expanded-key speed-up. So the design must give up one of rate, RAM or standard conformance.
**Plain-language explanation:** Falcon is the only NIST PQ signature small enough for a V2X frame (KNOWN,
arXiv 2608.05087). Falcon signing needs very precise floating-point arithmetic, and the M33 (like the M4) only
has single-precision floats in hardware. So implementers either fake double precision in integer code (slow), switch to
fixed-point (faster but more RAM), or chain single floats together (fast but possibly non-conforming). Keeping a
larger pre-expanded key makes signing faster again but costs RAM. Every published option sits at a different
point of the speed-RAM trade-off. On a V2X unit both limits bind at once: it must sign 10 times a second
*and* verify dozens of neighbours' messages. The question is whether any point satisfies both, and if not, which
compromise is least bad.
**Status of evidence:**
- KNOWN: Falcon-512 is the only fitting PQ signature, and signing cost is unmeasured on a constrained signer
  (arXiv 2608.05087: PC simulation, sign and verify excluded).
- KNOWN: fixed-point signing is ~45% faster than emulated double on M4 (ePrint 2026/2431, abstract).
- KNOWN: low-RAM Falcon-512 signs in ~11 kB RAM at 13.45 M cycles average on M4 (ePrint 2026/1915, from a search
  snippet of its PDF). Triple-word float runs on M4F (TCHES 2026 artifact a27).
- KNOWN: the only Cortex-M33 source found is a wolfSSL vendor blog with no numbers.
- CONJECTURE (the collision-checker's back-of-envelope): 13.45 M cycles at 78 MHz is ≈ 172 ms, i.e. ≈ 5.8
  signatures/s, already below 10 Hz *if* M4 cycles transfer to the M33 (uncalibrated estimate, assumes equal
  cycles. The 78 MHz clock is confirmed by datasheet Table 4.2, p. 24, but at 78 MHz the flash needs 1 wait state, so
  M4 cycle counts measured at zero wait states may undercount on the M33).
- ASSUMPTION: the M33 and M4 have similar pipelines and DSP multiply instructions, so cycle counts transfer within a
  small factor. This is to be checked, not assumed, in week 1.
**Collision check:** **Partial overlap.** The per-variant M4 results are published. Open as far as searched is a
calibrated M33 comparison of the variants against a combined 10 Hz signing + verification budget and a RAM
budget. Novelty risk: because the M33 and M4 are architecturally similar, "the M33 winner differs" probably
won't hold, so the contribution has to be the joint rate/RAM/verify-load feasibility map, not a port. A snippet
suggests NIST plans 64.64 fixed-point for FN-DSA, which could make the triple-word option non-conforming (VERIFY).
(Log: `collision-r2-A3-fndsa-v2x-m33-signer.md`.)
**Distinct from the exclusion list:** D5 was *verification* of a firmware image, and its stack crossover was already
published. A3 is *sustained signing* at message rate plus a verification load. D1 is a KEM handshake.
**Kill experiment (week 1):** Run Pornin's reference Falcon-512 under a Cortex-M33 instruction-count harness
(QEMU `mps2-an505`) and compare the counts with the published M4 cycles. If any single variant already clearly
meets 10 Hz signing *plus* a realistic verification load within 64 KB RAM, the "trade-off is forced" claim is
dead. The direction then shrinks to a benchmark note. One to two days.
**Measurements:** sign cycles (mean and worst case over many signatures, since Falcon's sampler has variable
time) and verify cycles per variant (instruction-count estimate, calibrated against published M4 cycles from ePrint
2026/1915 / 2026/2431 and pqm4). Peak RAM and stack (static analysis plus stack painting under emulation). Expanded vs
non-expanded key. Signature and certificate bytes from arXiv 2608.05087. Energy per signature = cycles ×
active current per MHz (estimate; 33.3–49.1 µA/MHz at 78 MHz and 3.0 V, datasheet Table 4.5, i.e. about 0.100–0.147 nJ/cycle).
**Semester plan:** (1) harness plus kill experiment on reference Falcon. (2) Port or build the fixed-point,
low-RAM and triple-word variants and measure all four with and without the expanded key. (3) Build the feasibility map
(rate × RAM × verify load) and the conformance note. (4) Write up.
**DRP phase 2:** The strongest of the round. Run the winning variant on the real EFR32xG24 (it *is* an M33), then
mount a power/EM side-channel study of Falcon's Gaussian sampler and floating-point or fixed-point arithmetic. That is
an active topic, and fixed-point FN-DSA has, as far as its abstract shows, no leakage evaluation (ePrint 2026/2431).
**Group split:** Falcon variant implementers (2) / MCU cost harness / V2X budget model, evaluation and writing.
**Scores (1–5):** novelty **3** (partial port risk), threat-model clarity **4**, falsifiability **4** (back-of-envelope
already leans one way), feasibility **4**, DRP continuity **5**, group split **4**. **Total 24.**

### 3.3 Dropped (Lane A)

- **A1: Actuators as CKKS decryption oracles (IND-CPA-D in closed-loop IoT FHE).** **Dropped (near collision).**
  ePrint **2026/2490** (Bian, Choe, Kim, Lee, "Reaction Attack on CKKS Deployments with Asymptotically Optimal
  Query Complexity", listed as accepted to IEEE S&P 2027) already recovers a CKKS key from a client's **1-bit**
  valid/invalid reaction in O(H) queries, given published bootstrapping (coefficient-to-slot) keys. arXiv
  **2607.12742** ("Stability Buys Time") already argues that encrypted control loops leak the key because "the loop must
  decrypt to actuate". The remaining sliver (thresholded actuation in a deployment with *no* bootstrapping keys)
  is open only as far as three queries went, and is too thin to carry a project. (Log:
  `collision-r2-A1-ckks-actuator-oracle.md`; note `papers/ckks-indcpad-li-micciancio.md`.)
- **A2: Privacy leakage from federated unlearning on wearables.** **Dropped (minor variant of excluded work).**
  Before/after-unlearning attacks in FL are published: FUIA arXiv 2502.14558, DRAUN arXiv 2506.01777 (AISTATS
  2026), DRAGD arXiv 2507.09602, arXiv 2505.11097, ULIA arXiv 2508.06789. DRAUN already compares five unlearning
  methods. The before/after model difference is effectively a gradient of the forgotten data, and the defences are
  the same noise and pruning. A2 therefore collapses into round 1's D2 / the instructor's gradient-leakage example,
  moved from images to time series. (Log: `collision-r2-A2-fed-unlearning-leak.md`.)

---

## 4. Lane B: outside PQC / FHE / FL

> Every card in this section is **NEEDS INSTRUCTOR APPROVAL: outside PQC/FHE/FL**.

### 4.1 Mining table (Lane B)

| # | Paper (verified ID) | Status | Depth | Measured on | Seed | Move | → Candidate |
|---|---|---|---|---|---|---|---|
| B-p1 | Quarantined-TreeKEM (ePrint 2023/1903) | ACM CCS 2024 | A (PDF 403) + code README | Kotlin PoC; no device numbers seen | Inactive members break PCS; inactivity counted in epochs, not time | Account for whole system | → **B3** |
| B-p2 | Vision: modular anonymous-credential framework, BBS + ECDSA binding (ePrint 2025/1981) | SSR 2025 | A (PDF 403) + IETF draft + code | Android wallet + Rust server | Secure element signs one P-256 ECDSA per presentation; rest in software | Move to device | → **B4** |
| B-p3 | TALUS threshold ML-DSA (arXiv 2603.22109) + Garillot et al. CRYPTO 2021 (ePrint 2021/1055) + RFC 9591 | arXiv only / CRYPTO / RFC | F (HTML, truncated) / A / F | 16-core desktop only | Nonce state must be consumed-before-emit and survive crashes; no MCU analysis | Attack the defence | → B2 (dropped) |
| B-p4 | Low-latency masked Ascon gadgets, leveled (Cassiers, Standaert, Verhamme, TCHES 2024(3):603–633) | TCHES 2024 | A + artifact README | **Hardware only** (FreePDK45, Sakura-G) | Short vs long messages change the best masking building block | Find crossover | → B1 (dropped) |

Harvest hits triaged out (IDs from search-result pages only, **not verified by an agent**): on-device Groth16 proving for IoT (zk-IoT; zkmcu verifier on RP2350) had mostly vendor
or hobbyist numbers. ZK smart-meter billing on MCUs was already done (Molina-Markham et al., FC 2012, MSP430).
OPRF/PSI (arXiv 2604.14909, ePrint 2026/1532) had no IoT angle. Older BLS / Schnorr half-aggregation for sensors
(ePrint 2022/222) had no 2025–26 device work.

### 4.2 Direction cards (Lane B, top 2)

#### B3: How long does a sleepy IoT fleet stay compromised? Post-compromise recovery in MLS/TreeKEM under duty cycling

**NEEDS INSTRUCTOR APPROVAL: outside PQC/FHE/FL**

**Move used:** Account for the whole system (sleep schedules, radio) + find the crossover (update policy).
**Application:** A fleet of duty-cycled nodes shares one MLS (RFC 9420) group for end-to-end encrypted telemetry
and commands, for example Thread sleepy end devices or LoRaWAN class-A sensors in a building or on a farm. Nodes
wake rarely, so they rarely update their keys.
**Threat model:** Asset = confidentiality of group messages *after* a node is compromised. Adversary = an attacker
who extracts one node's key state once (physical access to a field device) and then passively records traffic.
Classical. Out of scope: insider DoS, and a malicious delivery service. We measure the time until the group "heals",
meaning it no longer uses any key the attacker knows, plus the per-node cost of the policy that achieves that.
**Claim (falsifiable):** In a duty-cycled fleet, the group's healing time is set by its sleepiest member and grows
with the wake-up period. An IoT-specific update policy can bound the healing time at a per-node energy and airtime
cost we quantify: forced periodic updates on wake, quarantine as in Quarantined-TreeKEM, or eviction of
inactive nodes. CONJECTURE: the cost-optimal policy for IoT differs from the one recommended for messaging apps,
because quarantine relies on secret-share holders who are themselves asleep.
**Plain-language explanation:** In MLS every member holds keys arranged in a tree. When a member refreshes its
keys, the parts of the tree an attacker had learned become useless. That is "post-compromise security", healing.
But the tree only fully heals once *every* stolen key has been replaced. If a stolen node (or a node whose keys
were copied) never wakes up to refresh, its old keys stay valid and the attacker keeps reading. Messaging apps
see this with phones that go offline. IoT devices sleep *by design*, often for minutes to hours. So the question becomes:
how long are we exposed for a given sleep schedule, and what is the cheapest way to shorten that? Every fix costs
radio bytes and battery, and some fixes (like splitting a sleeper's new keys among others) only work if those
others are awake.
**Status of evidence:**
- KNOWN: inactive members undermine group-wide PCS, and quarantine or removal is the fix (Quarantined-TreeKEM,
  ePrint 2023/1903, CCS 2024; RFC 9420 guidance).
- KNOWN: external operations weaken MLS PCS (ETK, ePrint 2025/229, EUROCRYPT 2026).
- KNOWN: MLS cost has been measured on a server VM only (arXiv 2502.18303).
- KNOWN: TreeKEM-based group key agreement for CPS buses exists, evaluated on paper, with inactive nodes removed
  (arXiv 2601.21966).
- CONJECTURE: the healing-time scaling and that the policy ranking flips for IoT.
- ASSUMPTION: an X25519 + AES-GCM MLS ciphersuite; published Thread/LoRaWAN duty cycles; 802.15.4 frame limits;
  EFR32MG24 radio energy per byte of about 0.50 µJ/B TX at 0 dBm (1.84 µJ/B at 10 dBm) and 0.55 µJ/B RX, derived from datasheet
  Table 4.6 and the 250 kbps rate (see *Target device*).
**Collision check:** **Partial overlap.** "Inactivity hurts PCS" is known. Open as far as searched: a
wall-clock healing-time model as a function of Thread/LoRaWAN duty cycles; per-policy energy and airtime per node;
and evidence that the best IoT policy differs from the messaging one. Must read before committing: the
Quarantined-TreeKEM full PDF (403) and Lebrun's ANSSI thesis (Dec 2025), which may already simulate inactivity
periods; also the Group OSCORE / ACE rekeying drafts, the IoT community's own sleepy-node rekeying work (only
skimmed). (Log: `collision-r2-B3-mls-pcs-duty-cycle.md`.)
**Kill experiment (week 1):** A discrete-event simulation of TreeKEM epochs with nodes waking on published
duty cycles; no crypto code yet (MLS message sizes from an existing PC library such as OpenMLS). If healing time
under plain "update on every wake" is already bounded by one wake period with negligible extra bytes, there is
no problem to solve and the direction is dead. Two days.
**Measurements:** healing time distribution vs duty cycle and group size (simulation). Commit and update message
bytes (exact, from an MLS implementation on PC). 802.15.4 fragments per commit. HPKE / X25519 / AES-GCM cycles per
update on Cortex-M33 (instruction-count estimate, calibrated against published X25519 Cortex-M4 cycle counts). Energy
per policy = cycles × active current + bytes × radio energy per byte (estimate; constants in *Target device*).
**Semester plan:** (1) simulator plus kill experiment. (2) Implement the three policies, producing the healing-time vs
duty-cycle curves. (3) Add the MCU cost model and produce the healing-time vs energy frontier. (4) Write up.
**DRP phase 2:** The EFR32xG24 is a native Thread / 802.15.4 part, so run an MLS client on real sleepy Thread nodes
and measure true energy. Then a side-channel study of the X25519/HPKE update path on the M33.
**Group split:** MLS/TreeKEM protocol and simulator / policy design and security argument / MCU cost and radio
model / evaluation and writing.
**Scores (1–5):** novelty **4**, threat-model clarity **4**, falsifiability **4**, feasibility **3** (MLS on an MCU
is heavy, so the semester uses simulation plus emulated primitives), DRP continuity **4**, group split **4**.
**Total 23.**

---

#### B4: Anonymous credentials from a microcontroller: BBS presentation with secure-element ECDSA binding on Cortex-M33

**NEEDS INSTRUCTOR APPROVAL: outside PQC/FHE/FL**

**Move used:** Move to the constrained device + find the crossover (vs a plain certificate).
**Application:** An IoT device proves an attribute without becoming trackable, for example an e-bike or EV proving
"valid charging subscription" to a charger, a wearable proving "authorised staff" at a clinic door, or a smart
lock credential. The device's built-in secure element supports standard curves but no pairing-friendly curve. KNOWN: the
EFR32MG24B310 has Secure Vault **High**, with ECDSA/ECDH on P-192/P-256/P-384/P-521, Ed25519 and Curve25519, and no BLS12-381
(datasheet Table 3.1, p. 13). So the BBS part must run in software and only the binding key lives in the secure element.
**Threat model:** Asset = the holder's unlinkability across presentations (chargers or doors cannot track the
device), plus non-transferability (a thief who copies the MCU's flash cannot reuse the credential, because the
device key never leaves the secure element). Adversary = colluding verifiers and issuer (honest-but-curious,
tracking), and a flash-cloning thief. Classical. Out of scope: a subverted secure element (ePrint 2025/1995 covers
that) and side channels (DRP). We measure presentation time, RAM, proof bytes and energy.
**Claim (falsifiable):** A BBS presentation with ECDSA device binding (the ePrint 2025/1981 construction) can be
produced on a Cortex-M33 in under X seconds and Y KB RAM, with a proof that fits one BLE exchange. Its
cost is dominated by BLS12-381 G1 scalar multiplications plus the P-256 proof-of-possession work, *not* by
pairings: the holder computes none, only the verifier does. There is a crossover against a non-anonymous ECDSA
certificate in total energy (compute + radio) that tells a designer when privacy is affordable.
**Plain-language explanation:** A normal device certificate is cheap but works like a fixed ID: every charger
that sees it can link your visits. BBS credentials let the device prove *a fact* about itself ("subscribed",
"staff") with a fresh-looking proof each time, so visits can't be linked. To stop someone copying the credential,
it must be tied to a key that never leaves a secure chip. Most cheap secure chips only do ordinary ECDSA, and a
2025 construction shows how to bind a BBS credential to such an ECDSA key. That work was demonstrated on phones.
On a microcontroller, the unknown is whether the elliptic-curve arithmetic needed is affordable in seconds and
kilobytes, and whether the proof is small enough to send over a low-power radio.
**Status of evidence:**
- KNOWN: the BBS + ECDSA-binding construction exists (ePrint 2025/1981, SSR 2025; IETF draft-cllz-cfrg-ecdsa-pop-00,
  July 2026).
- KNOWN: an optimised version gives proofs of ~1.5 kB in under ~400 ms (circuit-free variant ~122 kB) on an
  unstated platform, likely phones (ePrint 2026/965, abstract only).
- KNOWN: the only known implementation is an Android wallet plus a Rust server.
- KNOWN: pairing-free BBS variants targeting constrained secure elements exist (BBS#, ePrint 2025/619; ePrint 2026/555).
- CONJECTURE: feasible on an M33 within seconds, and an energy crossover vs ECDSA certificates exists.
- ASSUMPTION: a no-std BLS12-381 library can be built for Cortex-M; BLE/802.15.4 energy per byte from the EFR32MG24
  datasheet (802.15.4 values in *Target device*. BLE 1 Mbps TX current is the same CW row of Table 4.6, but per-byte energy at 1 Mbps
  is 8 µs/B, i.e. 0.12 µJ/B at 0 dBm, derived).
- DO NOT CITE: a TechRxiv review's claim of "BBS+ verification < 50 ms on Cortex-M4" was untraceable and implausible.
**Collision check:** **Partial overlap.** The construction and phone-class numbers are published. Open as far as
searched: holder-side presentation (including the ECDSA proof of possession) on a Cortex-M MCU with an ECDSA-only
secure element, proof size vs BLE exchange, and the crossover vs a plain ECDSA certificate. Novelty risk: close to
an engineering port, and the pairing-free variants weaken a "pairing cost" framing, so they must be included as
comparison points. (Log: `collision-r2-B4-bbs-secure-element-mcu.md`.)
**Kill experiment (week 1):** An operation count with no implementation. From the IETF BBS draft and ePrint 2025/1981,
count the G1 scalar multiplications and P-256 operations in one presentation with k hidden attributes, and multiply by
published Cortex-M4 per-operation costs. If the estimate is above ~30 s or needs more RAM than an EFR32xG24-class
part has, the BBS route is dead on MCUs and the card pivots to the pairing-free variants. One day.
**Measurements:** presentation cycles split into BLS12-381 G1, P-256 PoP and hashing (instruction-count estimate on
QEMU M33; calibration source still to be found, since no verified M4 BLS12-381 benchmark is in hand yet). Peak RAM and
stack. Secure-element ECDSA P-256 sign 5.9 ms / verify 6.2 ms (datasheet Table 4.53, p. 92, SE Manager API). Proof bytes per construction. Energy = compute +
radio bytes (estimate). The same metrics for an ECDSA certificate and a pairing-free BBS variant as baselines.
**Semester plan:** (1) operation count plus kill experiment. (2) Build the holder side on emulated M33 with one
BLS12-381 library, producing the cost breakdown. (3) Add the pairing-free variant and the ECDSA-cert baseline,
producing the crossover. (4) Write up.
**DRP phase 2:** Very good hardware fit: run it on the real EFR32xG24 using its *own* Secure Vault for binding.
Then a side-channel study of the software part outside the secure element, for example whether the ECDSA
proof-of-possession step leaks the nonce.
**Group split:** BLS12-381 / BBS holder implementation / ECDSA-binding and secure-element interface / MCU cost and
radio model / evaluation and writing.
**Scores (1–5):** novelty **3**, threat-model clarity **4**, falsifiability **4**, feasibility **3** (BLS12-381 on
an M33 is real engineering), DRP continuity **5**, group split **4**. **Total 23.**

### 4.3 Dropped (Lane B)

- **B2: Brownouts break FROST: nonce state on intermittently powered sensor groups.** **Dropped (minor variant).**
  The attack is known: nonce reuse leaks the key share (RFC 9591 §7; FROST, ePrint 2020/852). The fix is deployed:
  Ledger's MuSig2 deletes the nonce seed from flash before emitting a partial signature (Delving Bitcoin thread 626;
  Bitcoin Core PR 29675). The storage pattern is studied in LMS/XMSS state reservation (NIST SP 800-208; Springer
  10.1007/978-3-319-49100-4_11), and Garillot et al. CRYPTO 2021 (ePrint 2021/1055) remove the stored state entirely.
  What remains is a measurement (brownout timing window, flash wear per signature on a Cortex-M) that a reviewer would
  call "LMS state management applied to FROST". Scored 23 (novelty 2) and cut on novelty. It is the Lane B
  fallback if B3 or B4 dies in week 1. (Log: `collision-r2-B2-frost-intermittent-power.md`.)
- **B1: Leveled masking of Ascon for tiny IoT frames.** **Dropped (weak; possible collision).** Masked *software*
  Ascon with a leveled variant on 32-bit ARM is published (ePrint 2024/755, Gigerl, Mendel, Schläffer, Primas: ~300 /
  ~550 cycles/byte at 2 / 3 shares, ~90 cycles/byte leveled, per its abstract). The full text could not be read, and if
  it reports short-message cost, B1 is a collision. The collision-checker's own arithmetic (CONJECTURE) also undercuts the
  claim. At 64 bytes a frame needs ~4 extra 8-round permutations against 24 protected rounds, so leveling still saves
  roughly 30–55% of the masked rounds, not "little". Either way, the crossover follows from per-round costs on paper.
  (Log: `collision-r2-B1-ascon-leveled-short-frames.md`.)

---

## 5. Combined ranking: round 1 and round 2 survivors

Round 1 scores are copied unchanged from `direction-mining-2026-11-05.md`. Both rounds used the same six criteria
(1–5 each, maximum 30). Ties are broken by novelty, then DRP continuity.

| Rank | ID | Round / lane | Direction | Nov | TM | Fals | Feas | DRP | Split | **Total** | Collision verdict | Scope |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 1 | D3 | R1 | HE-friendly cipher *client* cost on M33 vs Xeon + transciphering crossover | 3 | 4 | 5 | 5 | 5 | 5 | **27** (was 28) | Partial overlap (was Open; SoK Table 4, HHE Kombat) | In scope (FHE) |
| 2 | D2 | R1 | On-device cost of defeating gradient inversion on sensor FL | 3 | 5 | 5 | 4 | 4 | 5 | **26** (was 27) | Partial overlap (Sudars 2025, TS-Inverse Tab. IX) | In scope (FL) |
| 3 | **A4** | R2 / A | Amortised PQ source authentication of CAN FD frame streams | 4 | 4 | 5 | 4 | 3 | 5 | **25** (was 26) | Partial overlap (CAIBA motivation risk) | In scope (PQC) |
| 4 | **A3** | R2 / A | FN-DSA signing at 10 Hz on an M33-class V2X unit: rate × RAM × verify load | 3 | 4 | 4 | 4 | 5 | 4 | **24** | Partial overlap | In scope (PQC) |
| 5 | D1 | R1 | PQ EDHOC compute-vs-radio energy + KEM crossover on M33/Thread | 3 | 4 | 5 | 4 | 4 | 4 | **24** | Partial overlap | In scope (PQC) |
| 6 | **B3** | R2 / B | MLS/TreeKEM recovery time vs duty cycle in sleepy fleets | 4 | 4 | 4 | 3 | 4 | 4 | **23** | Partial overlap | **NEEDS INSTRUCTOR APPROVAL** |
| 7 | **B4** | R2 / B | BBS anonymous credentials on M33 with secure-element ECDSA binding | 3 | 4 | 4 | 3 | 5 | 4 | **23** | Partial overlap | **NEEDS INSTRUCTOR APPROVAL** |

**Re-scored 2026-11-10** after full-text checks of D3, D2 and A4 (see each card and `decision-summary.md`); the order is unchanged.
A3 and D1 tie at 24 and also tie on novelty (3). A3 ranks above D1 on DRP continuity (5 vs 4).

**Reading of the table:** round 2 found nothing that beats D3 or D2, but **A4 is a genuinely new in-scope option**.
It ranks just below them, with the clearest threat model of the round. **A3 is the best DRP fit** of all seven,
because the EFR32xG24 is the target device itself. Lane B adds two device-level directions, both only usable if the
instructor approves leaving PQC/FHE/FL.

---

## 6. Verification log

### 6.1 Papers deep-read this round (notes in `research/papers/`)

All identities were verified by the reader agents against ePrint/arXiv/DOI landing pages. Reading depth is as reported.

| Note | Paper | ID / venue | Depth | Lane |
|---|---|---|---|---|
| `pq-v2x-falcon-cv2x` | PQ signatures in C-V2X (Abrar et al.) | arXiv 2608.05087, arXiv only | Full (HTML). Internal inconsistency: Falcon certificate 1,735 B (Table 2) vs 1,739 B (Table 3) | A |
| `fixedpoint-fndsa-m4` | "Flying, Not Floating" (Halmans, Kumar, Güneysu) | ePrint 2026/2431, preprint | Abstract only (403) | A |
| `pq-can-framework` | PQ-CAN (Conti, Marchiori, Matarazzo, Rubin) | arXiv 2504.10730, arXiv only | Full (HTML); Table II numbers to re-check against the PDF | A |
| `ckks-indcpad-li-micciancio` | On the Security of HE on Approximate Numbers | ePrint 2020/1533, EUROCRYPT 2021 | Abstract + secondary (403) | A |
| `fed-unlearning-edge-survey` | Federated Unlearning in Edge Networks: survey (Ng et al.) | arXiv 2601.09978, arXiv only | Full (HTML, chunked) | A |
| `unlearning-jeopardizes-privacy` | When Machine Unlearning Jeopardizes Privacy (Chen et al.) | arXiv 2005.02205, ACM CCS 2021 | Full (~93%) | A |
| `masked-ascon-leveled` | Low-latency masked Ascon gadgets (Cassiers, Standaert, Verhamme) | TCHES 2024(3):603–633 | Abstract + artifact README | B |
| `threshold-nonce-state-rollback` | TALUS threshold ML-DSA (Kao, Chang) + GKMN21 + RFC 9591 | arXiv 2603.22109 v5 / ePrint 2021/1055 (first author **Garillot**, corrected) | Full (truncated) / abstract / full | B |
| `quarantined-treekem` | Quarantined-TreeKEM (Chevalier, Lebrun, Martinelli, Taleb) | ePrint 2023/1903, ACM CCS 2024 | Abstract + code README (403) | B |
| `bbs-ecdsa-device-binding` | Vision: modular AC framework (Lehmann, Sidorenko, Zacharakis) | ePrint 2025/1981, SSR 2025 | Abstract + IETF draft + code (403) | B |

### 6.2 Papers surfaced by collision checks or reference follow (verified by agents; not deep-read)

- **Lane A:**
  - FN-DSA on Cortex-M: ePrint 2026/1915, 2025/123, 2026/2559 (M55); TCHES 2026 artifact a27 (triple-word FP).
  - CAN FD and automotive: Applied Sciences 16(8):3705 (doi:10.3390/app16083705); SAE 2025-01-0451; ePrint
    2023/492 (Merkle batch signing); ePrint 2026/2149.
  - CKKS attacks: ePrint 2026/2490 (S&P 2027, kills A1); arXiv 2607.12742; ePrint 2024/424, 2024/853, 2024/127,
    2024/116, 2024/203, 2025/045, 2025/395, 2025/1618, 2025/1627, 2026/285, 2026/2546.
  - Federated unlearning: arXiv 2502.14558, 2506.01777, 2507.09602, 2505.11097, 2508.06789, 2404.03659, 2603.24213;
    IEEE Network 38(2) 2024 (Wang, Li, Li); Ameen & Wang MSN 2024 (exists on DBLP, content unchecked).
- **Lane B:**
  - Masked Ascon: ePrint 2024/755 (masked software Ascon), 2026/1082, 2023/484.
  - Threshold signatures: ePrint 2020/852 (FROST); Springer 10.1007/978-3-319-49100-4_11.
  - MLS / group keys: ePrint 2025/229 (ETK, EUROCRYPT 2026); arXiv 2502.18303, 2601.21966.
  - Anonymous credentials: ePrint 2026/965 (S&P 2027), 2025/1995 (EUROCRYPT 2026), 2025/824, 2025/619 (BBS#),
    2026/555; IETF draft-cllz-cfrg-ecdsa-pop-00.
  - Industry sources (not peer-reviewed): Delving Bitcoin thread 626 / Bitcoin Core PR 29675.

### 6.3 Queries run (orchestrator)

- **Lane A harvest:**
  1. "FN-DSA Falcon Cortex-M implementation floating point 2025 2026 eprint"
  2. "HQC post-quantum KEM microcontroller Cortex-M benchmark 2025"
  3. "post-quantum V2X IEEE 1609.2 signature size vehicle CAN bus 2025 2026"
  4. "post-quantum smart meter DLMS COSEM wM-Bus migration 2025"
  5. "post-quantum medical implant wearable BLE pacemaker cryptography 2025 2026"
  6. "property inference attribute inference federated learning wearable sensor data 2025 2026"
  7. "LoRaWAN post-quantum join procedure ML-KEM agriculture 2025"
  8. "federated unlearning IoT privacy leakage edge devices 2025 2026 arXiv"
  9. "split learning label leakage activation inversion IoT wearable ECG 2025 2026"
  10. "Byzantine-robust aggregation poisoning defense federated learning IoT client cost microcontroller 2025"
  11. "homomorphic encryption wearable ECG inference client-side encryption decryption cost embedded 2025 2026"
  12. "post-quantum BLE pairing key exchange Bluetooth Low Energy ML-KEM 2025 2026"
  13. "FHE leakage IND-CPA-D attack approximate CKKS decryption results IoT client 2025"
  14. "post-quantum signatures intra-vehicle CAN FD ECU authentication Cortex-M 2025 2026"
- **Lane B harvest:**
  1. "zero-knowledge proof microcontroller prover sensor data attestation Cortex-M 2025 2026"
  2. "Ascon SP 800-232 IoT implementation side-channel masked Cortex-M 2025 2026"
  3. "FROST threshold Schnorr signature IoT devices microcontroller group 2025"
  4. "MLS TreeKEM IoT group key post-compromise security constrained devices 2025 2026"
  5. "anonymous credentials BBS signatures microcontroller wearable vehicle EV charging 2025 2026"
  6. "oblivious PRF private set intersection IoT device constrained 2025 2026"
  7. "verifiable computation proof of sensor reading IoT data provenance SNARK lightweight 2025 2026 eprint"
  8. "aggregate signatures IoT sensor network bandwidth BLS half-aggregation Schnorr 2025"
- **Reference follow:**
  1. "FROST threshold signature nonce reuse state rollback power failure preprocessing nonces stateful"
  2. "masked Ascon software Cortex-M implementation cost leveled leakage-resilient short messages"
  3. "privacy-preserving smart metering billing zero-knowledge range proof on meter microcontroller cost"
- **Per-candidate collision queries** (10–14 each, covering technical, plain-language, ePrint and
  neighbouring-community phrasings): verbatim in `research/collision-r2-*.md`. Several agents hit one or two
  rate-limited queries; each log says which, and how they were re-run.

### 6.4 Not yet checked (read before committing to a round 2 direction)

- **A4:** ~~full text of app16083705; CAN FD peripheral; PQ-CAN Table II~~ **done 2026-11-10** (no amortisation; no CAN
  peripheral; Table II matches). Still to read in full: CAIBA (arXiv 2504.16695), arXiv 2611.08108, SAE 2025-01-0451, ePrint 2026/1915.
- **A3:** full text of ePrint 2026/2431 and 2026/1915 (403; cycle and RAM figures). Whether NIST's FN-DSA draft
  specifies 64.64 fixed point (affects the conformance of the triple-word option). Any real M33 Falcon numbers.
- **B3:** full Quarantined-TreeKEM PDF (403); Lebrun ANSSI thesis (Dec 2025); Group OSCORE / ACE group-rekeying
  drafts.
- **B4:** full texts of ePrint 2025/1981 and 2026/965 (403; platform and op counts). A verified Cortex-M BLS12-381
  benchmark for calibration. Secure Vault's exact ECDSA capabilities.
- **Carried over from round 1:** ~~EFR32xG24 datasheet electrical numbers~~ done 2026-11-10 (see *Target device*). Still open:
  confirm the kit part number from the UG524 user guide.

---

*Next step:* if the group wants an in-scope alternative to D3/D2, run A4's half-day spreadsheet kill experiment,
and read app16083705 in full first. Before choosing any Lane B card, ask the instructor whether out-of-scope
primitives are acceptable.
