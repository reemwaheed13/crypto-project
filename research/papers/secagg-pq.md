# Paper note: SecAgg++ (post-quantum secure aggregation)

## Verified identity
The candidate ID **DID resolve** — it is a real paper, not bogus.

- **ID:** IACR ePrint **2026/2497** (resolves; `https://eprint.iacr.org/2026/2497`).
- **Title:** "SecAgg++: A Post-Quantum Privacy Preserving Federated Learning Protocol".
- **Authors:** Alessandro Perez, Giulia Cavicchioni, Silvio Cretti, Riccardo Longo,
  Alessandro Tomasi, Silvio Ranise (Fondazione Bruno Kessler; Ranise also University of Trento).
- **Venue/year:** Published at AISec — the 19th ACM Workshop on Artificial Intelligence and Security,
  2026. ePrint page metadata states "Published elsewhere. AISec: 19th ACM Workshop on
  Artificial Intelligence and Security"; received/approved 15 Oct 2026 (IACR news item 30136).
- **Status:** ePrint + workshop-published (AISec 2026). KNOWN from the ePrint landing page and IACR news.

Caveat on the task prompt: it warned the ID might be "future-dated / bogus". It is NOT bogus — the
2026 ePrint numbering is current as of the reading date (2026-11-05), and the page/abstract are
consistent across the ePrint listing, the IACR news item, and AISec 2026.

Closely related verifiable alternatives found (kept for the collision analysis, see Relevance):
- **ePrint 2026/2394** — "Post-Quantum Dropout-Resilient Verifiable Secure Aggregation for Federated
  Learning" (PQ-DVSA), Fateme Sadat Azimi, Hossein Pilaram, Javad Mohajeri (Sharif University of
  Technology), 2026. Resolves; abstract confirmed.
- **arXiv 2601.01053** — "Byzantine-Robust Federated Learning Framework with Post-Quantum Secure
  Aggregation for Real-Time Threat Intelligence Sharing in Critical IoT Infrastructure" (listed in
  harvesting; title confirmed via search index, not read in full here).

## Reading depth
**Abstract + metadata + search snippets only.** The full PDF (`/2026/2497.pdf`, and the ACM /
Semantic Scholar mirrors) consistently returned **HTTP 403** to the fetch tool, so I could NOT open
the body, tables, or figures. Everything below the abstract line is therefore either (a) the verbatim
abstract, (b) a clearly-labelled inference, or (c) explicitly marked UNVERIFIED. No numbers, hardware
specs, or quotes from the paper body are reported because I could not confirm them.

## Core claim
(KNOWN, from the abstract) SecAgg++ is a **masking-based** secure-aggregation protocol for the
**single-server** federated-learning setting that adds **post-quantum security** while claiming to
**preserve the communication and computation efficiency** of the classical SecAgg / SecAgg+ line, and
that **handles client dropout via an efficient recovery procedure**.

## Platform & what was ACTUALLY measured
- **Lattice primitive:** NOT confirmable from the abstract. The abstract says only "integrates
  post-quantum cryptographic primitives". (INFERENCE / UNVERIFIED: secondary summaries in the search
  index describe the PQ-SecAgg family as using "Kyber KEM for pairwise seed establishment, expanded by
  a PRG into mask vectors" — i.e. ML-KEM/Kyber for the pairwise-seed step — but I could not verify this
  is what *this* paper does from the PDF. Treat as unconfirmed.)
- **Per-client communication / computation:** NOT available (PDF not read). Abstract only makes the
  qualitative claim of matching SecAgg/SecAgg+ efficiency.
- **Round count:** NOT available. (INFERENCE: SecAgg+ is a ~3–4-round masking protocol; "preserving
  efficiency of SecAgg/SecAgg+" suggests a comparable round structure, but this is not stated in the
  material I could read.)
- **Dropout handling:** abstract states "an efficient recovery procedure" — mechanism and cost NOT
  available.
- **Hardware used:** **UNKNOWN / not confirmable.** No evaluation platform (PC, server, or any
  constrained / IoT / MCU device) could be read from the body. **Explicit flag:** given the AISec FL
  framing and the SecAgg lineage, any benchmarks are *almost certainly PC/server-only* — I found **no
  evidence of any microcontroller or embedded-device measurement** — but this is an inference, not a
  confirmed fact, because I could not open the evaluation section.

> The entire "numbers that matter" row is a gap here: cycles, bytes, key/ciphertext sizes, and round
> latency are all UNVERIFIED because the PDF was inaccessible. Do not cite any figure for this paper
> until the body is read.

## Threat model
- **Servers:** single-server setting (KNOWN, abstract).
- **Adversary strength:** NOT stated in the abstract. (INFERENCE: the SecAgg/SecAgg+ line it builds on
  is classically **semi-honest / honest-but-curious** for the input-privacy guarantee; SecAgg++ likely
  inherits this. UNVERIFIED whether malicious-server robustness or verifiability is claimed — the
  abstract does NOT mention verifiability, unlike the 2026/2394 alternative which explicitly does.)
- **Quantum adversary:** yes — post-quantum security is the headline goal (KNOWN).
- **Verifiability:** not mentioned in the abstract (so presumed out of scope; contrast 2026/2394).
- **Dropout:** in scope, with an "efficient recovery procedure" (KNOWN).

## How it achieves PQ security (the distinction that matters)
**Cannot be confirmed from the abstract, and this is the single most important open question for our
purposes.** The abstract says the protocol "integrates post-quantum cryptographic primitives". Two very
different readings are possible and the abstract does not disambiguate:
1. **PQ-channel only** — classical pairwise-mask SecAgg with the Diffie-Hellman key agreement swapped
   for a lattice KEM (ML-KEM/Kyber), the mask PRG and secret-sharing otherwise unchanged. Input hiding
   would still rest on the PRG/one-time-mask argument, not on a lattice assumption.
2. **Lattice masking** — the masking/hiding of the inputs is itself lattice-based (e.g. RLWE-style
   additive noise), so input confidentiality rests on a lattice hardness assumption.

(INFERENCE from the name "masking-based" + "preserving SecAgg/SecAgg+ efficiency" + the Kyber-KEM
hints in secondary sources: reading **(1), PQ-protected key agreement over a classical masking
skeleton, is the more likely design**. But this is NOT verified and must be checked against the body
before relying on it.)

## Convenient assumptions
(Partly KNOWN, partly inferred — flagged.)
- Single aggregation server (KNOWN).
- Honest-but-curious server for the privacy guarantee (INFERENCE from SecAgg lineage; UNVERIFIED).
- A trusted setup / PKI for client key distribution is typical of this family (INFERENCE; UNVERIFIED).
- Clients can afford the lattice-KEM keygen/encaps and the mask-expansion PRG — tractable on a server
  or laptop; **cost on a constrained device is an untested assumption** (see Relevance).
- Efficiency "preserved" relative to SecAgg/SecAgg+ (KNOWN as a claim; the supporting numbers UNREAD).

## Stated limitations & future work
NOT readable from the abstract/metadata I could access — the PDF (which would contain the
limitations/future-work section) returned 403. I therefore record **no quote** rather than risk
misattributing one. (Honest gap: re-fetch the PDF from a non-blocked network to fill this.)

## Load-bearing references
Could not read the reference list (PDF inaccessible). Based on the title, abstract, and topic, the
load-bearing references are almost certainly (INFERENCE):
1. Bonawitz et al., "Practical Secure Aggregation for Privacy-Preserving Machine Learning", CCS 2017
   — the original SecAgg the title responds to.
2. Bell et al., "Secure Single-Server Aggregation with (Poly)Logarithmic Overhead" (SecAgg+), CCS 2020
   — the "SecAgg+" named in the abstract.
3. An ML-KEM / CRYSTALS-Kyber specification (NIST FIPS 203) — the presumed PQ KEM.
4. A MicroSecAgg-style efficiency/line paper (Guo et al., MicroSecAgg) — likely cited as a comparison
   point for communication overhead.
These are inferred placeholders, NOT confirmed citations from the paper.

## Relevance to our project (novelty / collision judgement — one honest paragraph)
This matters directly for our candidate direction (measuring PQ secure-aggregation cost on a
constrained MCU, e.g. the EFR32xG24). From what is verifiable: **SecAgg++ and the two sibling papers
(2026/2394 PQ-DVSA; arXiv 2601.01053) establish that PQ secure aggregation is now an active,
crowded space** — so the *primitive* is not novel and we should not pitch "a PQ secure-aggregation
protocol" as our contribution. **However, I found no evidence that any of these papers measured cost
on a real constrained / IoT microcontroller.** SecAgg++'s own evaluation hardware is unconfirmed but,
by the AISec/FL convention, is very likely PC/server-only; PQ-DVSA reports client-side *runtime*
("up to 96%" reduction vs SWAS — KNOWN from its abstract) but with no stated embedded platform; the
arXiv Byzantine-IoT paper is titled for "critical IoT" but was not read here and should be checked
specifically for whether it benchmarks an actual MCU. **Tentative verdict (CONJECTURE, pending a
full read of the three bodies): the protocol design space collides, but the "what does PQ secure
aggregation actually cost — cycles, flash/RAM, KEM-keygen energy, ciphertext bytes — on a
sub-100 MHz Cortex-M IoT MCU" question appears open as far as searched.** This leaves our candidate-4
direction plausibly alive *as a measurement/systems contribution* rather than a new protocol — but
the judgement is provisional until the SecAgg++ and arXiv 2601.01053 evaluation sections are read on
a network where the PDFs are reachable. Queries run: see Sources.

## Possible seeds (direction-miner moves)
- **Port-and-measure:** implement SecAgg++'s client step (lattice KEM + mask PRG + share) on the
  EFR32xG24 and report cycles / flash / RAM / energy — the cost these papers leave on a PC.
- **Primitive-swap sensitivity:** measure how per-client cost moves across ML-KEM-512/768/1024 on the
  MCU, which server-side papers treat as free.
- **Dropout-cost on-device:** quantify the "efficient recovery procedure" cost specifically on the
  constrained client, not the server.
- **Channel-vs-masking split:** empirically separate the KEM cost from the masking cost on-device to
  test which reading (PQ-channel vs lattice-masking) is actually cheaper for IoT.
- **Threat-model widening:** check whether moving from semi-honest (SecAgg++) to verifiable/malicious
  (PQ-DVSA 2026/2394) is affordable on an MCU — a measured bridge between the two sibling papers.

---
Sources (consulted for this note):
- https://eprint.iacr.org/2026/2497 (SecAgg++ landing page + abstract) — KNOWN
- https://iacr.org/news/item/30136 (IACR news, 15 Oct 2026, SecAgg++) — KNOWN
- https://eprint.iacr.org/2026/2394 (PQ-DVSA landing page + abstract) — KNOWN
- https://arxiv.org/pdf/2601.01053 (Byzantine-robust PQ secure agg for critical IoT — title only)
- https://www.emergentmind.com/topics/post-quantum-secure-aggregation (topic overview; Kyber-KEM hint)
- PDF bodies (eprint .pdf, ACM DL, Semantic Scholar) returned HTTP 403 and were NOT read.
