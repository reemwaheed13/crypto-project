# Collision check, round 2, B2: "Brownouts break FROST: nonce state on intermittently powered sensor groups"

Date: 2026-11-05. Lane B (threshold signatures, outside course scope).

## Verdict: PARTIAL OVERLAP, with high risk of being a minor variant

- KNOWN (cited): if a FROST nonce is reused across two sessions, the signer's key share can be recovered (RFC 9591 Sec. 7; FROST paper ePrint 2020/852). Preprocessed nonces must be persisted with rollback/replay protection (TALUS arXiv 2603.22109, according to a search snippet. The abstract page does not cover storage).
- KNOWN (practitioner): the crash-safe fix is already deployed for multi-party Schnorr on embedded signing devices. The Ledger MuSig2 design stores the nonce seed in flash and deletes it from flash before any partial signature leaves the device. A later failure forces a restart with fresh randomness (Delving Bitcoin thread 626, Mar 2024; Bitcoin Core PR 29675 discussion). That is consume-before-emit.
- KNOWN: the state-reservation and skip-ahead pattern for stateful signatures, with state kept in non-volatile storage and synchronisation-failure and cloning categories (SSR 2016, Springer 10.1007/978-3-319-49100-4_11; McGrew et al. slides; NIST SP 800-208). GKMN21 (CRYPTO 2021, ePrint 2021/1055) gives stale-but-consistent restored state as the reason to avoid stateful threshold Schnorr altogether.
- KNOWN: intermittent-computing checkpoint security (SICP, DATE 2019; a TrustZone port, Polito 2024; an attack on TI CTPL checkpoints that extracted an AES key). None of these covers threshold-signature nonces.
- What remains open as far as searched: no paper found that measures the brownout window for FROST nonce bookkeeping on a Cortex-M or energy-harvesting MCU, or quantifies flash wear and energy per signature for crash-safe nonce consumption in a k-of-n sensor group. Not found either: FROST on Cortex-M at all. "Completely FROST-ed" is on mid-range IoT devices and smartphones, and its hardware was not confirmed.
- Minor-variant risk: HIGH. The attack is textbook. The fix is the same pattern as LMS/XMSS reservation and Ledger's MuSig2 delete-before-sign. What is left is an engineering measurement: the cost of one flash erase/write per signature, or per batch of nonces. The FROST-specific twist is that a coordinator who replays a request with a different message after a brownout gets share recovery. That twist is already in RFC 9591's threat discussion. A reviewer could call it "LMS state management applied to FROST nonces".

## Queries run (all web search, standard mode)
1. FROST threshold signature nonce reuse power failure crash state rollback embedded
2. threshold Schnorr signing sensors energy harvesting intermittent power nonce reuse (FAILED: rate limit, no results used)
3. eprint.iacr.org FROST nonce preprocessing stateful rollback reset attack
4. intermittent computing checkpoint flash crash consistency cryptographic state nonce reuse attack microcontroller (embedded community)
5. LMS XMSS state management power loss microcontroller flash counter reservation stateful hash-based signature
6. "Completely FROST-ed" IoT threshold signatures
7. threshold signatures embedded systems microcontroller FROST implementation nonce storage 2025 (date-restricted variant)
8. nonce reuse after power failure non-volatile memory torn write attack signatures 2026 (date-restricted variant)
9. stateless deterministic multi-party EdDSA state rollback VM reset nonce reuse Garillot Kondi Mohassel Nikolaenko
10. FROST threshold signature wireless sensor network energy consumption Cortex-M evaluation (networking/WSN community)
11. intermittent devices security attacks power failure crypto state energy harvesting "nonce"
12. MuSig2 hardware wallet secnonce persistent storage power loss reuse BIP327 Ledger

## Sources opened or seen
| ID | Status | Depth |
|---|---|---|
| Delving Bitcoin "State minimization in MuSig2 signing sessions" (salvatoshi, 2024-03-01), delvingbitcoin.org/t/626 | verified (fetched) | full post skimmed. Delete-from-flash-before-sign design. No flash-wear or energy numbers |
| TALUS, arXiv 2603.22109 (Kao, Chang, v5 Aug 2026) | verified (abs page fetched) | abstract only. Persistence and rollback wording seen only in a search snippet of the PDF |
| CSEM report on threshold signatures in embedded systems (yoda.csem.ch) | NOT verified (HTTP 403) | snippet only |
| Completely FROST-ed (Khattat, Kromes, ICBC 2024, TU Delft) | verified (portal listing) | abstract-level via search. No nonce-persistence content seen |
| GKMN21, ePrint 2021/1055 / CRYPTO 2021 | verified (search results) | snippet: stale restored state leads to randomness reuse |
| RFC 9591 | verified (search results) | table-of-contents level: Sec. 7.3 nonce reuse |
| ePrint 2026/563 (Optimizing FROST for Message Capacity) | seen in search results | snippet only. Not about state loss |
| SSR 2016 (Springer 10.1007/978-3-319-49100-4_11), state reservation for N-time HBS | seen in search results | snippet only |
| SICP (DATE 2019); Polito TrustZone port (2024); TI CTPL checkpoint attack (par.nsf.gov) | seen in search results | snippets only |
| Ledger app-bitcoin-new commit; Bitcoin Core PR 29675 | seen in search results | snippet only |
| TF-M mailing list (flash counter for AES-GCM nonce under power cycling) | seen in search results | snippet only |

## To kill or confirm next
Read the CSEM report in full (use another access route). Read TALUS Sec. on preprocessing storage. Check the BIP-327 security notes and the Ledger app-bitcoin-new MuSig2 docs for any measured flash cost. Search for "FROST Cortex-M4" and "frost-ed25519 no_std".
