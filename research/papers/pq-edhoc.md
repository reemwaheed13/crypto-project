# Paper note: Enabling Quantum-Resistant EDHOC (PQ-EDHOC)

## Verified identity (VERIFIED)
- **Title:** "Enabling Quantum-Resistant EDHOC: Design and Performance Evaluation"
- **Authors:** Lidia Pocero Fraile, Georgios Tasopoulos, Christos Koulamas, Raymond K. Zhao,
  Nazatul Haque Sultan, Francesco Regazzoni, Apostolos P. Fournaris.
- **Venue / status:** IEEE Access, vol. 13, pp. 75861-75884, 2025. **Published** (peer-reviewed, open access).
- **DOI:** 10.1109/ACCESS.2025.3554010 (published 24 Mar 2025).
- **Stable copies seen:** Zenodo record 18175014 (open-access PDF); ResearchGate publication 390155853
  (403 to fetcher); DBLP `journals/access/fraile-tkzsrf-25`. Companion open-source code:
  GitHub `LPFraile/PQ-EDHOC`.
- **Note:** The candidate's ResearchGate lead (390155853) is confirmed to be the SAME paper; the
  authoritative verifiable version is the IEEE Access DOI above (preferred over ResearchGate).
- This is distinct from the **IETF LAKE "PQ EDHOC Design Team"** work (standardization effort, separate).
  I pulled IETF-126 DT slides only as context, not as this paper.

## Reading depth
**Skimmed.** Verified identity, abstract, author list, platform, algorithm list, and two headline
comparative numbers from the abstract + multiple independent search snippets and the companion GitHub
README. The full numeric results TABLES (absolute handshake ms, absolute energy mJ, RAM/flash KB,
per-message byte counts) were present in the downloaded PDF but **could not be reliably text-extracted
by the available tools** (font/image-encoded tables defeated the fetch summarizer; no local shell to
re-OCR). Those absolute values are therefore marked "not reliably extracted" below rather than guessed.

## Core claim
EDHOC (RFC 9528), the lightweight key-exchange that bootstraps an OSCORE security context, can be made
fully quantum-resistant by swapping its classical ECDH/ECDSA for NIST PQC KEMs and PQC signatures, and
this "PQ-EDHOC" is practical on a constrained Cortex-M4 IoT node. Among the combinations tested, the
**ML-KEM-512 + HAWK1** suite gives the best overall handshake time, memory, and energy — reportedly
~54% faster and ~80% lower energy than classical ECDSA/secp256r1 over BLE (KNOWN, from abstract/snippets).

## Platform & what was ACTUALLY measured
- **Device:** Nordic **nRF52840-DK**, ARM **Cortex-M4** (KNOWN). Companion repo runs Zephyr RTOS on the
  nRF52840-DK and also a Linux host build.
- **Network stack:** **6LoWPAN over BLE** (KNOWN). NOTE: this is BLE, *not* 802.15.4/Thread — see Relevance.
- **Measurement type:** **real on-board** benchmarking on the nRF52840 for the embedded figures (handshake
  time, energy, memory), with additional Linux-host runs for broader algorithm coverage (ASSUMPTION based
  on repo split: ML-KEM-768/HQC/BIKE are "Linux only"). Energy "least" claim is a BLE-transport measurement.
- **Base implementation:** extends the **uOSCORE-uEDHOC** library (RFC 9528 EDHOC) with PQC (KNOWN, repo).
- **PQC KEMs swapped in:** ML-KEM-512 / ML-KEM-768 / ML-KEM-1024; paper also discusses/benchmarks
  HQC and BIKE (repo marks HQC1, BIKE1 as Linux-only). Paper's verdict: "ML-KEM emerges as the only
  suitable KEM choice for resource-constrained environments" (KNOWN, abstract).
- **PQC signatures swapped in:** ML-DSA(-44), Falcon (FALCON1), HAWK (HAWK1), HAETAE, SPHINCS+
  (KNOWN — paper text + repo). Verdict: emerging **HAWK** outperforms standardized ML-DSA and Falcon
  for this use (KNOWN, abstract).
- **Message counts:** EDHOC is a **3-message** handshake (message_1/2/3) in both classical and PQ form;
  PQ-EDHOC keeps the EDHOC message structure and inflates payloads (INFERENCE — standard EDHOC/RFC 9528,
  not a new count claimed by the paper).
- **Headline numbers (KNOWN, from abstract + snippets, comparative only):**
  - ML-KEM-512/HAWK1 handshake ~**54% faster** than classical ECDSA/secp256r1.
  - ML-KEM-512/HAWK1 ~**80% less energy** than classical ECDSA/secp256r1 over BLE.
- **Absolute numbers (NOT reliably extracted):** exact handshake time in ms, absolute energy in mJ/J,
  RAM and flash in KB, and per-message (message_1/2/3) byte sizes for each PQ suite — these ARE in the
  paper's tables but could not be pulled cleanly by the tools here. Do not cite specific absolutes from
  this note; re-open the IEEE/Zenodo PDF for them.
- **Context size reference (NOT from this paper's tables — FIPS-203 standard values):** ML-KEM-512
  public key = 800 B, ciphertext = 768 B. Useful for back-of-envelope radio-cost math; confirm the
  paper's own on-wire EDMOC message sizes before quoting.

## Threat model
- **Asset protected:** the EDHOC key exchange that establishes the OSCORE context (session keys and
  mutual authentication of two constrained endpoints) (INFERENCE from EDHOC role + paper framing).
- **Adversary:** a future **quantum** adversary able to break ECDH/ECDSA (Shor), plus the implicit
  classical **active** network attacker EDHOC already defends against (MitM on the handshake). The
  driver is "harvest-now-decrypt-later" and quantum forgery of the handshake (KNOWN framing).
- **Classical vs quantum:** target is quantum-resistance; the comparison baseline is classical
  ECDSA/secp256r1 + ECDH.
- **Out of scope (ASSUMPTION / not emphasized):** side-channel / fault attacks on the PQC
  implementation; formal security proof of the modified protocol (a separate SBSEG paper does
  "formal verification" of a PQ EDHOC — not this one).

## Convenient assumptions
- One board (nRF52840) + one transport (BLE/6LoWPAN) stands in for "constrained IoT"; no 802.15.4/Thread,
  no cellular/NB-IoT, no lossy-link fragmentation sweep is the headline setting.
- Energy ranking is transport-specific (BLE); the ~80% energy win bundles compute + BLE radio cost and
  may not transfer to other radios.
- "Best suite" is judged on handshake time / memory / energy; it does not weight signature/key sizes'
  effect on fragmentation or retransmission on lossy links (ASSUMPTION).
- HAWK is an *emerging* (non-standardized) signature; adopting it assumes future standardization/trust.

## Stated limitations & future work
Paraphrase: the work positions ML-KEM as the practical KEM and flags that standardized signatures
(ML-DSA, Falcon) are heavier than the emerging HAWK for constrained nodes; broader algorithm coverage
and some combinations were evaluated on a Linux host rather than the MCU. One short quote (abstract):
> "ML-KEM emerges as the only suitable KEM choice for resource-constrained environments."
(Full "Limitations/Future Work" section text was not cleanly extractable here — verify in PDF before
quoting further.)

## Load-bearing references (3-5)
1. RFC 9528 — Ephemeral Diffie-Hellman Over COSE (EDHOC). (The protocol being modified.)
2. RFC 8613 — OSCORE (the context EDHOC bootstraps).
3. NIST FIPS 203 (ML-KEM) and FIPS 204 (ML-DSA) — standardized primitives swapped in.
4. uOSCORE-uEDHOC library — the constrained EDHOC base implementation that was extended.
5. HAWK signature scheme spec — the emerging signature that wins their benchmark.
(IETF LAKE "PQ EDHOC Design Team" materials are the standardization counterpart — useful context.)

## Relevance to our project
Directly on-point for a "PQC handshake cost on Cortex-M" direction, but with one important mismatch for
OUR intended framing. It gives us a real-board, published baseline: EDHOC made PQ, 3-message flow, on a
Cortex-M4, with ML-KEM + a PQ signature, and a clear comparative result (~54% faster / ~80% less energy
than classical for ML-KEM-512/HAWK1). That is a strong anchor. HOWEVER, it is measured over **BLE /
6LoWPAN on a Cortex-M4**, not **802.15.4/Thread on a Cortex-M33 (EFR32xG24)** — so our planned
"handshake energy is radio-bound over 802.15.4/Thread on M33" question is NOT answered here: different
radio (BLE vs 802.15.4), different MTU/fragmentation behavior, different MCU (M4 vs M33 with its crypto
accelerator). Their energy figure also bundles compute+radio into one ranking rather than *decomposing*
how much of the handshake energy is radio vs CPU — which is exactly the decomposition our direction needs.
What is left unmeasured for us: (1) per-message on-wire bytes vs 802.15.4 MTU and the resulting
fragmentation/retransmission energy; (2) a compute-vs-radio energy split; (3) EFR32xG24/Cortex-M33 numbers
and whether its hardware accelerator changes the ML-KEM compute share; (4) behavior under lossy links. So
this paper is the best starting baseline and reference implementation (we can reuse their PQ-EDHOC code
path), and our novelty would be the radio-bound decomposition on Thread/802.15.4 + M33 that they did not do.

## Possible seeds (direction-miner moves)
- **Port the baseline:** rerun their PQ-EDHOC (uEDHOC fork) on EFR32xG24/Cortex-M33 over 802.15.4/Thread;
  report compute-vs-radio energy split (their BLE M4 number is the control).
- **Change the radio, hold the protocol:** quantify how PQ-EDHOC message sizes interact with the
  802.15.4 MTU (~127 B) and 6LoWPAN fragmentation — the fragmentation/retransmission energy they did not isolate.
- **Decompose the 80%:** separate KEM/signature CPU energy from radio TX/RX energy to test the
  "handshake is radio-bound" conjecture explicitly.
- **Sweep signatures for size, not just speed:** re-rank ML-DSA/Falcon/HAWK/SPHINCS+ by on-wire bytes
  and resulting link cost, since their ranking optimized time/energy/memory, not fragmentation.
- **Accelerator angle:** test whether the xG24 crypto accelerator shifts the KEM compute share enough to
  break their "ML-KEM is the only suitable KEM" conclusion on M33.
