# Transistor: a TFHE-friendly Stream Cipher — paper note

## Reading depth
**Skimmed / partial-read.** The ePrint landing page (abstract + metadata) was read in full.
The full PDF (eprint.iacr.org/2025/282.pdf) and the CRYPTO/Springer chapter returned HTTP 403 /
required authentication on every attempt, so the full technical body (exact tables, figures,
per-element bootstrap counts, benchmark CPU model) could NOT be read directly. A 2-page C2-2025
conference-talk abstract by co-author N. Bon (members.loria.fr/EThome/c2/594611.pdf) was read in
full and corroborates the abstract. Remaining structural numbers below are from the abstract plus
secondary sources (search snippets, citing papers) and are flagged as such. Numbers I could not
confirm from a primary/full-text source are marked "NOT CONFIRMED".

---

## Verified identity (KNOWN)
- **ePrint ID:** IACR ePrint 2025/282 — confirmed.
- **Exact title:** "Transistor: a TFHE-friendly Stream Cipher". (The LORIA talk abstract uses the
  variant title "Transistor: a FHE-friendly stream cipher"; the ePrint/CRYPTO title is the
  TFHE-friendly one.)
- **Authors:** Jules Baudrin, Sonia Belaïd, Nicolas Bon, Christina Boura, Anne Canteaut,
  Gaëtan Leurent, Pascal Paillier, Léo Perrin, Matthieu Rivain, Yann Rotella, Samuel Tap.
  (Affiliations: CryptoExperts, Inria, UVSQ, ENS Paris, Zama — a Zama/CryptoExperts + academic team.)
- **Venue:** CRYPTO 2025 (ePrint notes "a major revision of an IACR publication in CRYPTO 2025";
  also appears as a Springer LNCS chapter, DOI 10.1007/978-3-032-01901-1_17). Not ePrint-only.
- **Year:** 2025. ePrint history: received 2025-02-18, revised 2025-06-16.

---

## Core claim (KNOWN)
Transistor is a stream cipher purpose-built so that a server can evaluate it *homomorphically*
under TFHE cheaply, to support **transciphering** (client ships symmetric-ciphertext + a
homomorphically-encrypted key; server homomorphically re-encrypts data into FHE ciphertext,
avoiding FHE ciphertext expansion on the wire). It works over the small prime field **F_17 (p=17)**
to match TFHE's cheap low-precision programmable bootstrapping, uses a **SNOW-like design**
(LFSRs + a small non-linear AES-like FSM), and reports throughput **over 60 bits/s on a standard
CPU** with no expensive initialization, which the authors claim significantly outperforms prior
TFHE-friendly stream ciphers.

---

## Platform & what was ACTUALLY measured
- **Server-side homomorphic (TFHE) evaluation throughput:** "over 60 bits/s on a standard CPU"
  (KNOWN, from abstract). The exact CPU model, thread count, and TFHE library/params were in the
  full text — **NOT CONFIRMED** (full PDF unreadable).
- **Latency / bootstraps per output:** The design rationale is that only the small FSM is updated
  non-linearly, and "each non-linear operation corresponds in TFHE to a rather expensive
  Programmable Bootstrapping" (KNOWN, from talk/snippet). The *exact* number of programmable
  bootstraps (PBS) per keystream element — **NOT CONFIRMED** (would be in the full-text cost table).
- **State / structure:** Secondary sources describe an **LFSR of length 32** over F_17 plus a
  **16-element FSM register** (AES-like S-box layer), with LFSR-based key schedule and whitening.
  (INFERENCE from search snippets / citing papers — treat LFSR=32, FSM=16 as **NOT CONFIRMED**
  from primary text.) Keystream elements are in F_17; the scheme targets Z_q with q=2^5-adjacent
  plaintext handling.
- **Client (symmetric) side cost:** The client only runs a plain SNOW-like stream cipher over
  F_17 (LFSR steps + a few S-box/FSM updates per element) — no big multiplications, small state.
  **No client-side cycle/byte or energy benchmark is reported in what I could read**, and
  **NO embedded / MCU / Cortex-M measurement exists** in the paper or any citing source I found.
  → Client-side-on-constrained-hardware cost: **NOT FOUND / not measured.** All reported
  performance is the server-side homomorphic evaluation on a PC/standard CPU.

> Bottom line for the "is the client cheap enough for an MCU?" question: the *design* is
> MCU-plausible (tiny state, F_17 arithmetic, no FHE on the client), but the paper measures
> **only the server (PC) homomorphic side**, not the embedded client. This is the key gap for us.

---

## Threat model (KNOWN / partial)
- **Security target:** 128-bit security (KNOWN, from snippet of full text).
- **Transciphering model:** honest client holds the symmetric key; the FHE/decryption key is
  managed under FHE; the server is the evaluator and is treated as an adversary with access to
  keystream outputs. (INFERENCE — standard transciphering model; exact formalization NOT CONFIRMED.)
- **Information-theoretic argument:** the abstract/snippets state an attacker "cannot obtain any
  information about the secret key from three or fewer consecutive keystream outputs," plus analysis
  bounding the minimal keystream length needed for security. This bounds short-window key leakage.
- **Out of scope (INFERENCE):** side channels on the client implementation; fault attacks
  (note: separate papers — FRAST fault/cryptanalysis works — target the TFHE-friendly-cipher class,
  and 2025/1346 is a cryptanalysis of the *FRAST* cipher, not Transistor).

---

## Convenient assumptions
- Server/benchmark runs on a "standard CPU" (PC-class); cost on constrained hardware is not
  considered (ASSUMPTION that PC numbers are the relevant metric).
- F_17 chosen because TFHE's programmable bootstrapping is cheap at low precision; assumes the
  TFHE parameter set that makes p=17 PBS cheap is the deployment target.
- Security relies on the stream-cipher attacker model (keystream-only / known-plaintext on the
  symmetric side); the IT argument covers only short (≤3-output) windows, with heuristic/analytic
  bounds beyond that.
- No communication-cost or radio/energy accounting for the IoT client (no transmit cost model).

---

## Stated limitations & future work (paraphrase)
- The homomorphic throughput, while best-in-class for TFHE transciphering, is still only tens of
  bits/s — practical but far from line-rate; implicit room for further TFHE-parameter / engineering
  optimization. (INFERENCE from the "over 60 bits/s" framing; explicit limitations section NOT read.)
- Full security relies partly on analytic arguments beyond the proven ≤3-output window; continued
  third-party cryptanalysis is invited. The one short quote I can verify on design philosophy:
  > "A small Finite State Machine is the only part of the state updated non-linearly, each
  > non-linear operation corresponding in TFHE to a rather expensive Programmable Bootstrapping."
- Detailed "future work" bullets were in the full text — **NOT CONFIRMED.**

---

## Load-bearing references (3–5)
1. **Elisabeth** (Cosseron, Hoffmann, Méaux, Standaert — "Towards Case-Optimized Hybrid Homomorphic
   Encryption: Featuring the Elisabeth Stream Cipher", ASIACRYPT 2022) — prior TFHE-friendly stream
   cipher, the main baseline Transistor improves on.
2. **FiLIP** (Méaux et al.) — improved-filter-permutator TFHE-friendly cipher; part of the baseline
   lineage (Elisabeth-4 vs FiLIP comparison is referenced as motivation).
3. **FRAST** (ePrint 2024/745 / ToSC — "TFHE-Friendly Cipher Based on Random S-Boxes") —
   contemporary TFHE-friendly cipher; competing design point.
4. **TFHE** (Chillotti–Gama–Georgieva–Izabachène) — the underlying FHE scheme whose programmable
   bootstrapping dictates the F_17 / low-precision design choices.
5. **SNOW family** (SNOW 2.0 / SNOW 3G / SNOW-V) — architectural template (LFSR + FSM) that
   Transistor adapts to the homomorphic setting.

(Reference list is approximate — exact citation keys NOT read from the full bibliography.)

---

## Relevance to our project (one honest paragraph)
For a "transciphering client cost on Cortex-M33 (EFR32xG24)" direction, Transistor is one of the
most promising candidates on paper: the client runs an ordinary SNOW-like stream cipher over F_17
with a small state (reportedly an LFSR of ~32 elements plus a ~16-element FSM, each element ~5 bits),
LFSR shifts, and a handful of small S-box lookups per output — no modular multiplications of big
integers, no FHE on the device, no heavy initialization. That profile is exactly what an MCU can do
in a few hundred cycles per keystream element, and the per-byte RAM footprint should be well under
a kilobyte. BUT the paper measures **none of this on constrained hardware** — every reported number
is the *server-side* homomorphic evaluation ("over 60 bits/s on a standard CPU"). What is unmeasured
for us: (a) client cycles/byte and energy on Cortex-M33, (b) whether F_17 arithmetic (mod-17
reduction, 5-bit packing) is cheap or branchy on a 32-bit MCU without a fast divider, (c) constant-
time / side-channel behavior of the S-box and reduction (directly relevant to our phase-2 SCA goal),
and (d) the uplink/communication cost of the symmetric ciphertext vs. the homomorphically-encrypted
key. So Transistor is a strong *design* match for an MCU client, but "MCU-friendly client" is a
conjecture we would have to actually measure — the paper does not establish it.

---

## Possible seeds (direction-miner moves)
- **Port-and-measure:** implement Transistor's client (keystream gen) on EFR32xG24 Cortex-M33;
  report cycles/byte, flash/RAM, energy — the explicit unmeasured gap.
- **Representation sweep:** compare F_17 element packing / mod-17 reduction strategies on a 32-bit
  MCU (table vs. Barrett vs. bit-tricks) for speed and constant-time-ness.
- **Threat-model extension:** first-order SCA (power/EM) of the F_17 S-box + FSM update on our own
  MCU implementation — aligns with the course's phase-2 side-channel follow-up.
- **Baseline bake-off:** on the same MCU, compare client cost of Transistor vs. Elisabeth/FiLIP vs.
  Kreyvium to see whether "TFHE-friendly server" also means "IoT-friendly client".
- **Communication accounting:** model uplink bytes (ciphertext expansion + one-time FHE-key upload)
  under a typical LPWAN radio to close the missing comms/energy picture.

---

## Sources consulted
- https://eprint.iacr.org/2025/282 (landing page / abstract — read)
- https://members.loria.fr/EThome/c2/594611.pdf (C2 2025 talk abstract — read in full)
- https://link.springer.com/chapter/10.1007/978-3-032-01901-1_17 (CRYPTO/LNCS chapter — auth-walled)
- Secondary/citing sources for structural params (LFSR 32 / FSM 16, p=17, 128-bit, SNOW-like):
  search snippets from eprint.iacr.org/2025/093 (transciphering survey) and
  eprint.iacr.org/2025/782 ("AES Is Not Enough"). PDFs 403-blocked; used snippet text only.
