# Paper note: PQC in IoT — Communication, Fragmentation, and Reliability

## Verified identity (KNOWN)
- **Title:** "Evaluating Post-Quantum Cryptography in IoT Networks: Communication, Fragmentation, and Reliability"
- **Authors:** Eric Sakk, Guobin Xu, Jianzhou Mao, Shuangbao (Paul) Wang
- **Venue:** *Future Internet* (MDPI), open access.
- **Volume/Issue/Article:** 18(6), article no. 316.
- **Year / publication date:** 2026 (published 1 June 2026).
- **DOI:** 10.3390/fi18060316 (resolves to https://www.mdpi.com/1999-5903/18/6/316).
- **ISSN:** 1999-5903.
- **Funding:** NSF Award 2329053 (per NSF PAR record; PAR ID 10715228, public-availability date 1 June 2027).

Identity **matches** the candidate exactly (title, authors, journal, volume 18, issue 6, article 316, year 2026, DOI). Nothing contradicts the brief.

### Verification trail / access note
- `doi.org/10.3390/fi18060316` 302-redirects to `mdpi.com/1999-5903/18/6/316`. The MDPI HTML/PDF and sciprofiles pages returned **HTTP 403** to the fetcher (bot block), so the full text could not be opened directly.
- Primary verified source used: the **NSF PAR biblio record** (par.nsf.gov/biblio/10715228…), which carries the **verbatim abstract** and the publication metadata above.
- A couple of body-text numbers (below) came only from **web-search snippets** summarizing the article, not from a page I could open in full. These are flagged as UNVERIFIED-FROM-FULL-TEXT and kept separate from the abstract-sourced numbers.

## Reading depth
**Abstract-only + search snippets.** Full abstract read verbatim; full text (tables, figures, methods) could NOT be opened (MDPI 403). Treat all per-scheme/per-link detail as low-confidence unless tagged KNOWN.

## Core claim (KNOWN, from abstract)
PQC key-encapsulation and signature operations themselves are fast on general-purpose CPUs (microseconds), but their much larger key/ciphertext/signature sizes blow up communication cost on constrained IoT links; once these payloads are fragmented across BLE / 802.15.4(Zigbee) / LoRa, handshakes need many packets, and under realistic per-packet loss the end-to-end handshake success rate collapses. The bottleneck is **communication, not computation**.

## Platform & what was ACTUALLY measured
- **Crypto engine / measurement platform:** Open Quantum Safe **liboqs** framework, run on **general-purpose processors** (per abstract: "can be completed in microseconds using general-purpose processors"). The NSF PAR record likewise notes evaluation on general-purpose processors, "not specified as hardware-constrained devices."
  - **=> Costs were measured on a PC/server, NOT on an IoT MCU.** No Cortex-M board, no RTOS, no radio chip is reported. The IoT link behavior is reached by **mapping/analysis**: large payloads are divided into packets according to each link's payload limit, then reliability is computed from a per-packet delivery-probability model.
- **PQC schemes (KNOWN):** ML-KEM (key encapsulation) named explicitly in the abstract; "digital signature schemes" evaluated but **specific signature families/parameter sets not stated in the abstract**.
  - UNVERIFIED-FROM-FULL-TEXT (search snippet only): **ML-KEM-1024** cited as the worst fragmentation case; **SLH-DSA** (SPHINCS+) mentioned as needing ~8 KB buffering. Specific ML-KEM-512/768, ML-DSA-44/65/87, and Falcon-512/1024 inclusion = **not found / not confirmed**.
- **IoT link layers (KNOWN):** Bluetooth Low Energy (BLE), IEEE 802.15.4 (Zigbee), and LoRa. (6LoWPAN / Thread / full LoRaWAN MAC = **not mentioned**.)
- **Concrete numbers:**
  - KNOWN (abstract): ML-KEM handshake in low-payload LoRa "can require **up to 62 packets**."
  - KNOWN (abstract): when per-packet delivery probability approaches **99%**, handshake success rate can **drop to values approaching 50%**.
  - KNOWN (abstract): PQC ops complete in **microseconds** on general-purpose CPUs.
  - UNVERIFIED-FROM-FULL-TEXT (snippet): worst case ML-KEM-1024 => 62 packets => handshake success ≈ **53.6%**; SLH-DSA verification buffering ≈ **8 KB**, exceeding RAM on many Class-1 devices.
  - **Not found:** exact public-key / ciphertext / signature byte sizes as reported by THIS paper's tables; per-link MTU/payload values assumed; latency in ms; energy in µJ/J; figure/table numbers.

## Threat model (KNOWN, inferred from framing)
- **Asset:** confidentiality/integrity of IoT key establishment against a future quantum adversary (standard harvest-now-decrypt-later motivation for PQC).
- **Adversary:** quantum-capable attacker that breaks classical KEX/signatures; this paper's *evaluation* adversary is really the **lossy channel** (packet loss during fragmented handshakes).
- **Out of scope:** side-channel / fault attacks on the implementation; active on-path manipulation; the paper is a cost/reliability study, not a protocol-security proof. (Explicit threat-model section **not found** in accessible text.)

## Convenient assumptions (KNOWN where tagged; otherwise inference)
- **General-purpose CPU timing is taken as the compute cost** — MCU compute cost is not measured (liboqs on PC). ASSUMPTION that PC microsecond timings are the relevant compute baseline.
- **Per-packet delivery probability is an independent-loss model** (abstract gives ~99% per-packet => ~50% handshake success, consistent with independent Bernoulli loss over many fragments). Retransmission/ARQ behavior, duty-cycle limits, and LoRaWAN MAC overhead: treatment **not found** in accessible text.
- Fragment counts (e.g. 62) imply **a fixed max payload per link** (BLE/Zigbee/LoRa); the exact MTU/payload constants used are **not found**.
- Energy "consumption" is in the abstract but the measurement basis (model vs. board) is **not found**.

## Stated limitations & future work
Could NOT be read directly (full text blocked). The abstract's own framing is the strongest available signal: it concludes that **"communication metrics, rather than computational performance, pose key challenges to PQC deployment in constrained IoT settings"** — implying the open work is reducing fragmentation/communication overhead (e.g., compression, link-layer tuning, scheme/parameter selection) rather than speeding up the crypto. Detailed limitations/future-work list = **not found**.

## Load-bearing references
Reference list could NOT be extracted (full text blocked). Load-bearing *context* (standards/tools the paper is built on), by name:
1. **Open Quantum Safe / liboqs** — the measurement framework (the paper's main tooling dependency).
2. **NIST FIPS 203 (ML-KEM)** — the KEM standard evaluated.
3. **NIST FIPS 204 (ML-DSA) / FIPS 205 (SLH-DSA)** — PQC signature standards (signature family evaluated; exact set unconfirmed).
4. IoT link-layer specs implied: **IEEE 802.15.4 / Zigbee**, **BLE**, **LoRa**.
(Exact cited works and their IDs = not verified.)

## Relevance to our project
**Strongly supportive** of our working hypothesis that PQC handshake cost on constrained IoT is **radio/fragmentation-bound, not compute-bound** — and it is the closest published statement of exactly that thesis for BLE/Zigbee/LoRa. The headline numbers (ML-KEM up to 62 LoRa packets; ~99% per-packet delivery collapsing to ~50% handshake success) are a clean, citable anchor for our threat-model framing and for motivating secure-aggregation/handshake-cost measurements.

Honest caveats — what it does NOT give us:
- **No MCU numbers.** Compute was liboqs on general-purpose CPUs, so it does **not** measure Cortex-M33 (EFR32xG24) cycles, flash/RAM footprint, or on-device energy. Our phase-2 EFR32xG24 work is therefore **complementary, not redundant** — we can supply the missing constrained-device compute/memory/energy side and check whether "microseconds on a PC" survives on an M33 (likely tens of ms, which could shift the compute-vs-radio balance).
- **Reliability is modeled, not emulated.** The success-rate curve looks analytical (independent per-packet loss); there is no Cooja/ns-3/Contiki run reported. A Cooja/ns-3 replication with realistic ARQ and LoRaWAN duty cycle is an open follow-up.
- **Signature side underspecified** in what we could read — ML-DSA vs Falcon vs SLH-DSA parameter choices and their byte sizes are not confirmed, so our own size/fragment table would add value.

### Possible seeds (direction-miner moves)
- **Port-to-real-hardware:** redo the "microseconds" claim as EFR32xG24 Cortex-M33 cycle/RAM/flash counts; test whether compute stays negligible vs radio on-device.
- **Swap the cost metric:** replace modeled reliability with emulated (Cooja/ns-3) handshakes under duty-cycle + ARQ for LoRaWAN/6LoWPAN; measure real retransmission energy.
- **Change the link layer:** extend from BLE/Zigbee/LoRa to **6LoWPAN/Thread fragmentation** (802.15.4 + IPv6 reassembly), which the paper omits.
- **Harden the threat model:** add the side-channel angle (our DRP phase 2) that this cost-only study explicitly leaves out.
- **Vary the scheme/parameters:** build the missing confirmed size-vs-fragment table across ML-KEM-512/768/1024 and ML-DSA/Falcon/SLH-DSA to find the fragmentation sweet spot per link MTU.

---
*KNOWN = from verbatim abstract or verified metadata. UNVERIFIED-FROM-FULL-TEXT = appeared only in search snippets summarizing the body; not confirmed against an opened page. Full text (MDPI) was inaccessible (HTTP 403) at time of reading, 2026-11-05.*
