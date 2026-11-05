# Paper note: PQC performance evaluation for consumer IoT devices

## Verified identity
- **Status: VERIFIED (citation + abstract-level content confirmed; full text paywalled).**
- **Title:** "A comprehensive and realistic performance evaluation of post-quantum security for consumer IoT devices"
- **Authors:** Yacoub Hanna, Jessica Bozhko, Samet Tonyali, Ricardo Harrilal-Parchment, Mumin Cebe, Kemal Akkaya
  (author list from search metadata on ResearchGate/ScienceDirect; not cross-checked against the PDF byline since the PDF was inaccessible).
- **Venue:** *Internet of Things* (Elsevier), Volume 33, 2025, article number 101650.
- **DOI:** 10.1016/j.iot.2025.101650
- **pii:** S2542660525001647
- **Access:** ScienceDirect landing page returned HTTP 403 (paywall/bot block); ResearchGate, Elsevier linkinghub, and IACR ePrint review PDF all also blocked (403 / redirect-only). Content below is reconstructed from the abstract and secondary search summaries, NOT from the article body.

## Core claim
KNOWN: Post-quantum KEMs and signatures are viable in TLS on power-constrained consumer IoT devices, but their performance is strongly governed by the underlying wireless link (BLE much more sensitive than Wi-Fi). The paper argues the right PQC choice is device/link dependent and proposes dynamically selecting the KEM/signature suite per the device's physical network interface. It emphasizes "realistic" PKI conditions (server-side certificate chains, OCSP/OCSP-Stapling, CRLs) that prior IoT PQC benchmarks often omitted.

## Platform & what was ACTUALLY measured
- **PQC schemes/params (KNOWN from abstract/summaries):**
  - KEMs: Kyber512 (ML-KEM-512) highlighted as "ideal for BLE when combined with Falcon512".
  - Signatures: Falcon-512 (favored for BLE), Dilithium2 (favored for Wi-Fi).
  - NOTE: HAWK is NOT part of this paper (see attribution note below).
- **Protocol:** TLS handshake/authentication over two wireless stacks: Bluetooth Low Energy (BLE, including IP-over-BLE with a gateway) and Wi-Fi.
- **Realistic PKI factors measured (KNOWN from summaries):** server-side certificate chains, OCSP, OCSP-Stapling (cached and non-cached variants evaluated), and CRLs; reported to add communication overhead, "especially when using OCSP Stapling."
- **Metrics (KNOWN):** handshake latency (ms), communication overhead, and energy consumption of the IoT device.
- **Specific numbers I could confirm (from a search summary of the paper, abstract-level — treat as TENTATIVE, not read in the PDF):**
  - Falcon512 TLS authentication handshake WITHOUT certificate validation: ≈ 131.65 ms over Wi-Fi, ≈ 236.4 ms over BLE.
  - CAVEAT: these two numbers come from a search-engine summary, not from a page I could open directly. Verify against Table/Figure in the PDF before reuse.
- **Energy numbers in mJ/J: NOT FOUND in accessible sources.** The abstract states energy was measured, but no concrete mJ/J figure was visible to me. Do not cite an energy value for this paper until the PDF is read.
- **Real board vs emulator:** Described as measurements on real IoT devices (device energy was measured and IP-over-BLE with a gateway was implemented), so this reads as REAL-HARDWARE measurement, not emulation or pure analysis. **Exact board models (e.g., nRF52840 / ESP32 / Raspberry Pi) NOT CONFIRMED** — a search summary mentioned "nRF and Raspberry Pi" but I could not verify this against the paper, so it is left unconfirmed.

## Threat model
- **Asset:** confidentiality/authenticity of the TLS session (key establishment + authentication) between a consumer IoT device and a server/gateway.
- **Adversary:** INFERRED (not quoted) — a future quantum adversary against the TLS key exchange and PKI signatures; i.e., the standard motivation for migrating TLS KEM/signatures to PQC.
- **HNDL / harvest-now-decrypt-later:** Not explicitly confirmed in the material I could access. The KEM-migration framing is consistent with HNDL motivation, but ASSUMPTION — do not attribute an explicit HNDL threat statement to this paper without reading it.

## Convenient assumptions (inferred — confirm against PDF)
- TLS/PKI setting with a reachable server/gateway (IP-over-BLE needs a gateway) — assumes infrastructure typical of consumer IoT, not ad-hoc constrained D2D.
- Energy/latency reported for the handshake path; steady-state radio duty cycle and application traffic cost not obviously in scope.
- Standard NIST-round parameter sets at the lowest security levels (512/2 level) as the IoT-relevant operating point.
- Single-device / point-to-point measurement; fleet-scale or concurrent-session effects likely out of scope.

## Stated limitations & future work
Could not be read verbatim (PDF inaccessible). INFERRED direction from the abstract: the dynamic per-interface suite-selection mechanism is a proposal whose generality and the overhead of OCSP-Stapling/CRL validation on constrained links are the open-cost questions. One short phrase I can attribute to the authors' summary: OCSP Stapling notably increased "communication overhead." (Do not treat the rest of this section as a direct quote.)

## Load-bearing references (likely; confirm in PDF)
- NIST PQC standards — ML-KEM (FIPS 203 / Kyber), ML-DSA (FIPS 204 / Dilithium), and Falcon.
- TLS 1.3 (RFC 8446) as the handshake baseline.
- OCSP / OCSP-Stapling and CRL specifications (RFC 6960 and related) — central to the "realistic PKI" angle.
- Prior PQC-on-IoT / PQC-in-TLS benchmarking work (e.g., Open Quantum Safe / liboqs-based evaluations) as the comparison baseline.
  NOTE: exact reference list NOT verified against the PDF.

## Attribution note — the "~80% less energy, ML-KEM-512 + HAWK1" claim is NOT this paper
The harvested claim "ML-KEM-512 + HAWK1 ... ~80% less energy than classical ECDSA/secp256r1" belongs to a DIFFERENT paper: "Enabling Quantum-Resistant EDHOC: Design and Performance Evaluation" (attributed to Pocero Fraile et al.), which implements PQ-EDHOC on a Nordic nRF52840 over BLE and reports the ML-KEM-512 + HAWK1 suite as lowest energy/handshake. HAWK does not appear in the Hanna et al. (this) paper, which uses Falcon-512 / Dilithium2 as its signatures. Keep these two separate; do not transfer the 80% figure onto the ScienceDirect paper.

## Relevance to our project
This paper is directly on-target for our PQC-on-IoT direction and is useful mainly as a REAL-DEVICE, realistic-PKI reference point for the "which PQC suite per link" question and for the BLE-vs-Wi-Fi sensitivity story — it supports our intended radio-vs-compute crossover argument qualitatively (handshake cost dominated by the wireless link, with BLE far worse: ~236 ms vs ~132 ms for Falcon512 in the one data point I found). HOWEVER, it does NOT give us the clean energy baseline we wanted: I could not see any mJ/J energy figure, and the only latency numbers I have are from a search summary, not the PDF, so they are tentative. The clean "~80% energy savings" number we were hoping to reuse comes from the separate EDHOC/HAWK paper, not this one, and is itself device-specific (nRF52840/BLE) — so it must be attributed to that work, not folded into this baseline. ACTION before relying on any number here: obtain the PDF (library/author copy) and read the energy and handshake tables directly.

## Possible seeds (direction-miner moves)
- **Swap the asset:** paper measures TLS handshake cost; we could measure the steady-state radio-vs-compute crossover (handshake energy amortized over session lifetime / duty cycle) — the paper gives link-sensitivity but likely not amortization.
- **Stress an assumption:** push the OCSP-Stapling/CRL overhead onto a genuinely constrained MCU (our EFR32xG24 phase-2 target) and see if the "dynamic per-interface selection" still holds.
- **Cross-check the baseline:** reconcile this paper's Falcon512/Kyber512 handshake numbers with the EDHOC/HAWK1 energy numbers on comparable BLE hardware — is the signature choice (Falcon vs HAWK) the real energy lever?
- **Add the missing metric:** if energy per handshake is thin here, our contribution could be a calibrated energy model (cycle counts -> mJ) for the same suites on xG24.

## Reading depth
**Abstract-only + secondary search summaries.** Full text was paywalled/blocked (ScienceDirect, ResearchGate, Elsevier linkinghub, IACR ePrint all 403). Citation and DOI verified; abstract-level findings verified; interior numbers (131.65 ms / 236.4 ms) are from a search summary and remain to be confirmed against the PDF. No energy figure confirmed.
