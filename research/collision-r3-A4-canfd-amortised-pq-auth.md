# Collision check: Round 3, A4: Amortised PQ source authentication of periodic CAN FD frames

Date: 2026-11-10. Searcher: collision-check subagent. Previous log: research/collision-r2-A4-canfd-amortised-pq-auth.md.

## Candidate (as received)
Application: Cortex-M ECUs broadcasting periodic 64 B CAN FD sensor/control frames (periods 1–100 ms).
Asset: SOURCE authenticity of each frame. Adversary: a compromised ECU that holds the AUTOSAR SecOC shared MAC keys, plus a
future quantum forger against vehicle-lifetime keys. Construction: one ML-DSA-44 / FN-DSA-512 / SLH-DSA signature per window
of N frames over a Merkle root or TESLA anchor, with a short auth path in each frame. Claim (CONJECTURE): a window size N*
exists that meets a 10 ms authentication deadline at no more than X% added bus load. Second conjecture: under amortisation
the Falcon-over-ML-DSA ranking shrinks or flips.

KNOWN from earlier full-text reading (not re-derived here): Lee et al., Appl. Sci. 16(8):3705 (2026) covers one-off transport
only and never mentions amortisation. PQ-CAN (arXiv 2504.10730) uses per-message signatures, and its Falcon overhead is
dominated by keygen. ePrint 2023/492 covers TLS server batch signing.

## Queries run (verbatim)
1. `post-quantum Merkle batch signature CAN FD frames window amortized authentication` (technical, standard)
2. `SAE 2025-01-0451 post-quantum cryptography intravehicular networks` (SAE/automotive)
3. `ePrint 2026/1915 Falcon low RAM` (ePrint)
4. `LiBrA-CAN lightweight broadcast authentication controller area networks Groza Murvay` (classical neighbour)
5. `TESLA hash chain broadcast authentication in-vehicle network post-quantum signature anchor 2026` (extended, 2026)
6. `quantum-safe source authentication compromised ECU SecOC shared key signatures CAN stream` (plain language / threat model)
7. `CAN XL CANsec CiA 613-2 post-quantum source authentication AUTOSAR SecOC PQC asymmetric` (standards)
8. `hash-based signature in-vehicle network ECU Merkle tree one-time signature CAN bus quantum-resistant` (extended)
9. `Mitacs post-quantum cryptography CAN FDx automotive security performance evaluation publication`
10. `aggregate OR batch signature CAN bus frames ECU authentication latency bus load 2026 ML-DSA Falcon` (extended, 2026)
11. `draft-harvey-cfrg-mtl-mode latest version 2026 use cases` (IETF)
12. `post-quantum CAN authentication Merkle signature ECU` (allowed_domains: eprint.iacr.org, arxiv.org)
13. `automotive Ethernet SOME/IP post-quantum signature stream authentication batch Merkle ECU 2025 2026` (networking neighbour)
14. `"CAN FD" "post-quantum" broadcast authentication delayed key disclosure OR "hash chain" ECU 2025 2026` (2025–26)
15. `Castiglione Elia 2026 lightweight post-quantum PUF in-vehicle communication CAN` (follow-up)

## Sources opened or checked
| ID | Item | Verified | Depth | Relevance |
|---|---|---|---|---|
| ePrint 2026/1915 | Thomas Pornin (NCC Group), "A RAM-Efficient Implementation of Falcon", received 2026-09-07, revised 2026-09-10 | Yes (ePrint abstract page fetched) | Abstract | Falcon-512 signing RAM drops from about 31 kB to about 11 kB. More than half of the float ops are replaced by integer/modular computation. Average Cortex-M4 sign is 13.45 M cycles. This weakens the "float-free ML-DSA" half of the ranking-flip conjecture, which must now be tested against this implementation. No CAN content. |
| SAE 2025-01-0451 | Smith, Owens, Kozan, "Post Quantum Cryptography on Intravehicular Networks", 2025 NDIA Michigan GVSETS | Title, number and authors verified via SAE listing and search snippets. SwRI affiliation from the r2 log, not re-verified. SAE page did not render (JS loader). | Abstract/snippet only | Kyber/Dilithium/Falcon on automotive-class MCUs. Says the communication overhead is acceptable and notes the 8-byte CAN payload. No sign of batching, Merkle or TESLA. |
| LiBrA-CAN | Groza, Murvay, Van Herrewege, Verbauwhede. CANS 2012 (conference) and ACM TECS 16(3), Art. 90, Apr 2017 (journal) | Yes (imec/KU Leuven repository records and author PDF listing). DOI 10.1145/3056506 is inferred from the Unpaywall link and was not opened. | Snippets / metadata | Classical symmetric broadcast authentication (key splitting, MAC mixing). No PQ follow-up found. |
| arXiv 2504.16695 | Wagner, Basels, Bauer, Zimmermann, Wehrle, Henze, "CAIBA: Multicast Source Authentication for CAN Through Reactive Bit Flipping", EuroS&P 2025 | Yes (arXiv abstract page) | Abstract plus snippets | NEW NEIGHBOUR. Same threat (a compromised ECU masquerading despite SecOC), solved classically and symmetrically by an on-bus authenticator with no overhead. It argues TESLA-style delayed disclosure is unsuitable for safety-critical CAN because of verification delay. A reviewer will raise this against A4's TESLA variant and its 10 ms deadline. Not PQ, not amortised signatures. |
| arXiv 2611.08108 | Ming-Fong Sie, "Post-Quantum Authentication of IoT Data on Ethereum: Signatures, the Calldata Floor, and Device-Signed Merkle Roots", 5 Nov 2026 | Yes (arXiv abstract page) | Abstract | NEW NEIGHBOUR. Device-side ML-DSA/Falcon/SLH-DSA/LMS/XMSS measurements and per-device Merkle roots over 100 records. Cost metric is Ethereum gas, not bus load or deadlines. No CAN. The abstract does not say whether the Falcon/ML-DSA ranking changes under batching, so the full text must be checked before claiming the ranking-flip conjecture as new. |
| draft-harvey-cfrg-mtl-mode-09 | IETF MTL mode, 2026-03-24, plus draft-harvey-cfrg-mtl-mode-considerations-03 (2026-02-02) | Yes (datatracker listing) | Listing/snippets | Still the latest version found. Its use cases are DNSSEC and TLS-like settings. No automotive, IoT or CAN use-case document found. |
| CiA 613-2 | CAN XL add-on services, Part 2: CANsec data plane, WD v0.0.6, 2026-03-24 | Yes (CiA page via snippet) | Snippet | Symmetric AES-CMAC/GCM only. No asymmetric or PQ source authentication. No AUTOSAR SecOC PQC proposal found. |
| Mitacs project | "PQC integration in CAN FDx automotive networks" (Branco, PUCRS / U. Ottawa) | Yes (project page listing) | Snippet | Still no publication found. |
| Castiglione, Elia, Esposito, Cirillo (2026) | "Lightweight Cryptography and Physically Unclonable Functions for Secure In-Vehicle Communications" (UNISA IRIS) | Partially (IRIS listing via snippets. Fetch returned 403) | Snippet | PUF authentication, AEAD, and PQC for confidentiality on classical CAN. Not amortised source authentication. |
| doi:10.3390/info13080382 | Cultice and Thapliyal, "PUF-Based Post-Quantum CAN-FD Framework for Vehicular Security", Information 2022 | Yes (listing. MDPI fetch 403) | Snippet | PUF plus PQ key exchange (SIDH-based per snippet) on CAN FD. Message counts, not signature amortisation. |
| arXiv 2606.30542 | Lightweight PQ authentication framework for 5G base-station bootstrapping (TESLA-style hash chain) | Listed | Snippet | Neighbouring field (5G broadcast). Uses a TESLA chain with a PQ setting. Not in-vehicle. |

## Verdict: PARTIAL OVERLAP (unchanged from r2, as far as searched)
- Taken (KNOWN): one-off and per-message PQ transport costs on CAN/CAN FD (Lee et al. 2026; PQ-CAN 2025; SAE 2025-01-0451).
  Generic Merkle batch signing (ePrint 2023/492; MTL-09). Classical source authentication against a compromised ECU on CAN
  (CAIBA, EuroS&P 2025; LiBrA-CAN). Device-signed PQ Merkle roots for IoT records (arXiv 2611.08108, blockchain cost model).
- Open as far as searched: a PQ-signed Merkle root or TESLA anchor per window of N periodic CAN FD frames. This includes
  the deadline-vs-bus-load optimum N* per scheme with Cortex-M compute in the loop, and the Falcon-vs-ML-DSA ranking under
  amortisation measured against Pornin's 2026 low-RAM Falcon.
- Risks: (a) CAIBA's argument that TESLA delays are unacceptable on CAN must be answered, or the TESLA variant dropped.
  (b) The full text of 2611.08108 may already contain the batched Falcon/ML-DSA comparison. (c) At 13.45 M cycles,
  one Falcon sign per window is likely to take tens of ms at typical ECU clocks (ASSUMPTION about clock rate). Signing time
  then sets a floor on the window period, which the model must include.
