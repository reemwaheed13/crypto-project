# Collision check: B4 (Lane B, anonymous credentials, outside course scope)

**Candidate:** Anonymous credentials from a microcontroller. A BBS presentation with ECDSA P-256 device binding done by the secure element (Silicon Labs Secure Vault, EFR32xG24, Cortex-M33). We measure time, RAM and proof bytes against a BLE exchange, or find the dominating cost (BLS12-381 operations) and where it crosses over with a plain ECDSA certificate.
**Threat model:** The asset is holder unlinkability and non-transferability. The adversaries are colluding honest-but-curious verifiers and issuer, plus a flash-cloning thief. Classical setting.
**Date:** 2026-11-05

## Verdict: Partial overlap. Open as far as searched for the MCU-holder measurement.

- **Already covered:**
  - The cryptographic construction: BBS core plus ECDSA proof-of-possession device binding.
    - KNOWN: ePrint 2025/1981 (Lehmann, Sidorenko, Zacharakis), ePrint 2026/965 (Celi, Lehmann, Levin, Zacharakis), and IETF draft-cllz-cfrg-ecdsa-pop-00 (July 2026).
  - Proof sizes, from the 2026/965 abstract:
    - about 1.5 kB for the circuit variant;
    - about 122 kB for the circuit-free variant;
    - about 400 ms to prove, on an unnamed platform; the abstract implies phones.
  - Pairing-free alternatives aimed at constrained secure elements: BBS# (ePrint 2025/619) and ePrint 2026/555. These undercut the "pairing cost dominates" framing.
  - BBS device binding with a minimal secure-element role: ePrint 2025/1995 (EUROCRYPT 2026).
- **Still open as far as searched:**
  - No paper found that runs the whole holder side of a BBS presentation (G1 operations on BLS12-381 plus the ECDSA-PoP sub-proof) on a Cortex-M-class MCU.
  - No paper found that does this with an on-chip ECDSA-only secure element, or that checks the result against the size limits of a BLE exchange.
  - Note: the holder does not compute pairings in a BBS presentation; only the verifier does. The "dominating cost" claim needs restating as G1 multi-scalar multiplication plus P-256 work.
- **Risk:**
  - CONJECTURE: this is mostly an engineering port of the HPI construction, and its novelty is thin unless the crossover analysis or the IoT threat model adds something.
  - The HPI group, or the BBS# authors, could publish an MCU or secure-element benchmark at any time.

## Queries run
1. `BBS signature anonymous credential microcontroller Cortex-M implementation benchmark` (standard)
2. `ePrint 2025/1981 BBS ECDSA device binding` (standard)
3. `privacy-preserving attribute proof IoT device unlinkable credential embedded secure element` (standard, plain-language)
4. `BLS12-381 pairing Cortex-M4 performance embedded anonymous credentials smart card BBS+` (standard)
5. `eprint.iacr.org anonymous credentials constrained device microcontroller BBS 2025 2026 implementation IoT` (extended, ePrint-focused, 2025-2026)
6. `BBS selective disclosure embedded IoT ESP32 OR STM32 OR nRF52 benchmark verifiable credentials device` (standard, embedded community)
7. `pairing-based anonymous credential OR group signature ARM Cortex-M0 OR Cortex-M4 implementation timing BN254 OR BLS12` (standard)
8. `2026 BBS presentation ECDSA proof of possession embedded hardware wallet microcontroller performance` (extended, 2026)
9. `EV charging ISO 15118 anonymous credential privacy-preserving plug and charge unlinkable` (standard, application neighbour)
10. `Idemix OR U-Prove smart card implementation anonymous credentials Java Card MULTOS performance seconds` (standard)
11. `"BBS" signatures "Cortex-M33" OR "Cortex-M4" OR "microcontroller" anonymous credentials 2025 2026 arXiv` (extended, 2025-2026)
12. `Richards BBS+ verification ARM microcontroller Cortex-M4 optimisation` (standard; follow-up on an unverified claim)

## Papers / sources opened or inspected

| ID | Title | Verified | Depth | Relation |
|---|---|---|---|---|
| ePrint 2025/1981 | Vision: A Modular Framework for Anonymous Credential Systems (Lehmann, Sidorenko, Zacharakis) | Yes (ePrint page) | Abstract | BBS core plus ECDSA binding construction. The abstract shows no MCU benchmark. |
| ePrint 2026/965 | Device Binding for Anonymous Credentials on Legacy Phones (Celi, Lehmann, Levin, Zacharakis) | Yes (ePrint page) | Abstract | Closest neighbour. Targets phones; platform not named in the abstract. About 1.5 kB / 122 kB proofs, about 400 ms. |
| ePrint 2025/1995 | Device-Bound Anonymous Credentials With(out) Trusted Hardware (Friedrichs, Harding, Lehmann, Lysyanskaya), EUROCRYPT 2026 | Yes (ePrint page) | Abstract | BBS device binding; the secure element computes one BLS or Schnorr signature. No benchmark platform on the page. |
| draft-cllz-cfrg-ecdsa-pop-00 | Schnorr-Type Proofs of Possession for ECDSA Device Binding (IETF CFRG, July 2026) | Search snippet only | Snippet | Standardisation of the ECDSA-PoP sub-proof. |
| ePrint 2025/619 | Making BBS Anonymous Credentials eIDAS 2.0 Compliant (BBS#) | Search snippet only | Snippet | Pairing-free variant for constrained secure elements; benchmarks on devices (phones per snippet). |
| ePrint 2026/555 | Improved Issuer Hiding for BBS-based Anonymous Credentials | Search snippet only | Snippet | Pairing-free; smartphone timings. |
| ePrint 2025/824 | Specification of an AC system using BBS+ with revocation and device binding | Search snippet only | Title/snippet | DAA-style binding for BBS+. |
| HPI slides | Device Binding for BBS from ECDSA-PoPs (Lehmann, May 2025) | Search snippet only | Snippet | Overview of the same construction. |
| Turku MSc thesis (Pesonen) | Performance Evaluation of Optimal Ate Pairing on Low-Cost Single Microprocessor Platform | Fetch failed (403); snippet only | Snippet | BN254 pairing on a 64 MHz Cortex-M33: "only a handful of pairings per second". Calibration point; not a credential system. |
| blst GitHub issue #213 | BLS verify on Cortex-M4/M33-class device | Search snippet only | Snippet | About 3 s BLS verify against about 75 ms ECDSA on the same class of MCU. Calibration point. |
| Vullers and Alpar, IFIP IDMAN 2013; Bichsel et al. 2009 | Idemix on MULTOS and Java Card | Search snippet only | Snippet | Older smartcard RSA-based credential work: about 1-1.5 s on MULTOS, about 4-7 s on Java Card. Not BBS, not MCU, no ECDSA binding. |
| Hajny et al. (Brno) | Attribute proofs on MULTOS; pairing ECC on constrained devices (2018) | Search snippet only | Snippet | Neighbouring constrained-device work; not BBS with ECDSA binding. |
| PoPETs 2022-0066; POPCORN 2013; arXiv 2403.06632; SECRYPT 2025 | EV-charging privacy (DAA on TPM; anonymous credentials; SSI) | Search snippet only | Snippet | Application neighbours. The DAA/TPM charging paper overlaps the EV use case, but not the BBS, MCU, or ECDSA secure-element design. |
| TechRxiv 175492163 (2025 SoK) | Selective-Disclosure in Decentralised Identity | Fetch failed (403) | Snippet | Claims "Richards" got BBS+ verification under 50 ms on Cortex-M4. UNVERIFIED: no source found (query 12), and the figure is implausible given about 3 s BLS verify on M4. Treat as a possible hallucinated citation. Do not cite. |

## Notes for the caller
- ASSUMPTION: the EFR32xG24 Secure Vault exposes ECDSA P-256 signing only, with no pairing or BLS12-381 support. This matches the candidate's premise; not re-checked here.
- The holder side needs no pairings. The verifier does them, and it is not the MCU. The measurable bottleneck on the MCU is G1 scalar multiplication on BLS12-381 plus the ECDSA-PoP proof.
- 2026/965 proofs of 1.5 kB need several BLE packets. The 122 kB circuit-free variant is likely impractical over BLE (CONJECTURE).
