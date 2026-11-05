# BBS credentials bound to legacy ECDSA secure hardware (Vision / LSZ25)

Lane: B

## Citation
- Title (as verified on ePrint): "Vision: A Modular Framework for Anonymous Credential Systems". The brief described it as a
  BBS-to-ECDSA binding paper. That is accurate for the concrete instantiation, but the paper's main contribution is the modular framework.
- Authors: Anja Lehmann, Andrey Sidorenko, Alexandros Zacharakis (Hasso Plattner Institute, Univ. Potsdam). Verified.
- ID: IACR ePrint 2025/1981. Received 2025-10-23, latest of 3 revisions 2026-06-15.
- Status: published. The ePrint page lists it as appearing at the Security Standardisation Research (SSR) Conference 2025. Springer
  DOI 10.1007/978-3-032-19567-8_3, according to the eid-privacy zkp-vault index. I could not open the DOI, which redirects to a Springer login.

## Reading depth
**Abstract + snippets only.** eprint.iacr.org returned HTTP 403 for every PDF URL I tried (2025/1981.pdf, the dated archive copy, and 2026/965.pdf).
web.archive.org could not be reached, and the Springer page is paywalled.
I took technical details from secondary sources by the same group and labeled each one below:
- the HPI EUDI project page (hpi.de/lehmann/eudi.html);
- IETF draft-cllz-cfrg-ecdsa-pop-00 (Celi, Lehmann, Levin, Zacharakis, July 2026), read in full;
- the README of github.com/hpicrypto/age-verification, a demo that "follows the Vision approach";
- the README of github.com/hpicrypto/ecdsa_pops.
**Nothing below comes from the paper body itself.** Re-check these points against the PDF before citing them.

## Core claim (one sentence)
Anonymous credential systems can be built from a fixed core of commitment, signature and NIZK, with plug-in modules that connect only
through a commitment. Because of this, a BBS credential (pairing curve) can be bound to a device key that lives in legacy
ECDSA-only secure hardware without changing the core (abstract, verified).

## Mechanism, as far as I could verify (from the IETF draft and repos, not the paper)
- Holder key: ECDSA P-256 key pair held in the secure element (SE). The private key never leaves the SE.
  **The SE computes only one standard ECDSA signature per presentation.** It signs a fresh message: the
  "JWP-BBS-DB-CHAL" prefix followed by the presentation header (IETF draft).
- The credential stores the device public key Q = (x, y) as four 128-bit limbs in four BBS messages. Q can be committed during
  blind issuance, so the issuer never sees it (IETF draft).
- The presentation exposes Pedersen commitments to these limbs on the credential curve. A cross-group "commitment transfer" proves that each
  limb opens to the same value in a commitment on **Tom-256**. Tom-256 is a curve whose scalar field equals the P-256 base field, which
  makes P-256 coordinate arithmetic native inside the proof. A zero-knowledge proof of possession then shows knowledge of an ECDSA signature
  (r, s) under the committed Q (IETF draft).
- Pairing-friendly curve: **BLS12-381**. The IETF draft gives it only as an example ("e.g."), and the demo README states it explicitly:
  BLS12-381 with BBS+ for issuance and base proof, tom256 for equality across groups, P-256 holder key. The demo also adds Bulletproofs++
  range proofs and revocation.
- Which part needs the SE: only the ECDSA signing step. The holder runs everything else in normal software: the BBS proof of knowledge, the cross-group
  transfer and the ECDSA proof of possession. **For our candidate this means the heavy BLS12-381 and Tom-256 work would run on the
  Cortex-M33 application core, outside Secure Vault.** This is my inference from the protocol structure, not a claim from the paper.
- Security caveat in the IETF draft: revealing the ephemeral value K is sound only if the SE is honest and signs only inputs presented
  to it. The draft also says the Fiat-Shamir binding of the composed protocol is "not yet fixed by this document".

## Platform
- Vision paper: unknown, because I could not read the body. I do not know whether it includes any benchmark.
- Demo (age-verification repo): Android wallet app (Rust compiled for aarch64/x86_64 Android through UniFFI), with an Axum server as
  issuer and verifier. The README gives no timings or sizes.
- **No MCU, IoT or embedded platform appears in any source I could read. All known measurements are on phones or PCs.**
- Compiler flags: not available.

## Threat model (reconstructed from the abstract and the IETF draft; check against the paper)
- Assets: the holder's attributes (selective disclosure), unlinkability across presentations, and non-transferability of the credential
  (it cannot be cloned or shared without the device).
- Adversaries: verifiers or colluding verifiers trying to link or learn attributes, and a holder trying to clone or share a credential.
- Trust in the SE: honest-SE assumption for soundness (IETF draft). The SE privacy model is not stated in the sources I read. Compare
  2025/1995 below, which handles subverted or corrupted SEs.
- Out of scope (as far as seen): side channels on the holder's software computations.

## Numbers that matter
- From Vision 2025/1981 itself: **none verified.**
- Companion paper ePrint 2026/965, "Device Binding for Anonymous Credentials on Legacy Phones" (Celi, Lehmann, Levin, Zacharakis;
  listed as IEEE S&P 2027, received 2026-05-15, last revised 2026-10-26). Abstract numbers only:
  - Most efficient of three constructions: about 1.5 kB binding proof, proving time under about 400 ms, using a "simple arithmetic circuit".
  - Most conservative, circuit-free construction: similar proving time, proof about 122 kB.
  - An earlier version (cronokirby.com summary of the 2026-05 posting) gave "under ~500 ms" with about 1.5 kB, and about 500 ms with about 175 kB for the
    circuit-free construction. **Numbers changed between revisions.**
  - The "700 ms" figure appears only in a fragmentary search snippet of the 2026/965 PDF ("BBS credential with ECDSA device binding in
    700 ms"). I could not see its context or the device it was measured on. **Unverified.**
  - **The "~200 KB" figure requested in the brief could not be verified anywhere.** The verified circuit-free sizes are about 122 kB (current
    abstract) and about 175 kB (earlier version).
  - The HPI URL .../Publications/2026-PoP-ECDSA.pdf returned HTTP 404 on two attempts. Its search-result title is "2026 PoP ECDSA", and I
    assume it is the same work as 2026/965 (ASSUMPTION).
  - Phone models and the CPU or SE used for the 2026/965 measurements: not verified.

## Convenient assumptions (relevant to an IoT transfer)
- Phone-class application processor: about 400-500 ms proving on a phone, likely on a multi-GHz ARMv8 core (not verified).
  On a 78 MHz Cortex-M33 this time would grow by orders of magnitude (CONJECTURE).
- Unlimited RAM and storage: the Rust stack with KZG SRS download, Bulletproofs++, and builds of 10-12 GB of disk (the last is a demo README
  figure, not a runtime figure).
- Free radio: proofs of 1.5 kB to more than 100 kB, before the BBS proof itself. The circuit-free variant is impractical over BLE/802.15.4
  without heavy fragmentation (CONJECTURE).
- Honest SE for soundness.
- Online verifier on IP. No intermittent links or duty cycling are considered.

## Stated limitations / future work
I could not read the paper's own limitations section. The IETF draft says the composed Fiat-Shamir binding is not yet fixed. The draft and
paper 2026/965 both say that the missing ECDSA-compatible device binding is part of why the EUDI Wallet launched with batch-issued
single-use credentials instead of anonymous credentials.

## Load-bearing references (as seen in related sources, not the paper's bibliography)
- ePrint 2026/965, Celi-Lehmann-Levin-Zacharakis: ECDSA PoP constructions for device binding (companion work).
- Frigo and shelat, "Anonymous Credentials from ECDSA" (ePrint 2024/2010, Longfellow), linked from the Vision index entry.
- Paquin, Policharla and Zaverucha, "Crescent" (2024), linked from the Vision index entry.
- ZKAttest / equality-across-groups (Tom-256). Links to zkattest-rs and docknetwork equality_across_groups appear in HPI's
  "Device Binding for BBS from ECDSA-PoPs" slide deck (May 2025). I could not render its text.
- ePrint 2025/619, "Making BBS Anonymous Credentials eIDAS 2.0 Compliant" (pairing-free BBS# with ECDSA holder binding). Appears in the
  same reference list. Not read.

## Related papers (one line each, verified from ePrint landing pages)
- **ePrint 2025/1995**: Friedrichs (HPI), Harding (Brown), Lehmann (HPI), Lysyanskaya (Brown), "Device-Bound Anonymous Credentials
  With(out) Trusted Hardware". The ePrint page lists it as a major revision of a EUROCRYPT 2026 paper. It defines DBACs across SE trust levels (up to subverted or
  corrupted SEs) and gives three round-optimal BBS binding constructions in which the SE computes **one BLS or Schnorr signature per call**.
  That is not ECDSA, so it does not map directly to the P-256-only Secure Vault. It also gives a blind BLS variant for remote SEs.
- **ePrint 2025/824**: Graebnitz, Buchmann, Seiffert, Margraf (Fraunhofer AISEC), "A Specification of an Anonymous Credential System
  Using BBS+ Signatures with Privacy-Preserving Revocation and Device Binding". Preprint (ePrint-only as far as seen). It combines DAA-style
  hardware binding with BBS+ and status credentials for revocation.
- **ePrint 2026/965 / HPI "PoP ECDSA"**: see Numbers above. Verified: about 1.5 kB / under about 400 ms (most efficient) and about 122 kB (circuit-free).
  The ~700 ms figure is unverified snippet text, and ~200 KB is not found.

## Possible seeds (direction-miner moves)
- **Change platform (phone to MCU)**: no source measures the holder-side presentation (BBS proof on BLS12-381, Tom-256 transfer, ECDSA
  PoP) on a Cortex-M class core. A cycle and RAM estimate on Cortex-M33, plus a split between Secure Vault (ECDSA) and the app core, is a
  candidate question. It is open only as far as searched (queries listed below).
- **Change the constraint (radio budget)**: the proof-size and proving-time trade-off (1.5 kB circuit versus about 122 kB circuit-free) looks
  different when every byte costs BLE/802.15.4 airtime and energy. Choose the construction by energy, not by latency.
- **Change the trust model**: the soundness of 2026/965 and the IETF draft rests on an honest SE, while 2025/1995 covers subverted SEs but needs
  BLS or Schnorr. Ask what an ECDSA-only MCU secure element can achieve under 2025/1995-style weaker trust.
- **Add the attacker (phase 2)**: the holder-side software (limb commitments, PoP) runs outside the SE on the MCU. This makes it a natural
  side-channel target for the DRP phase. The ECDSA nonce handling (the revealed K) is especially worth checking.

## Queries run
- "A Modular Framework for Anonymous Credential Systems" Lehmann Sidorenko Zacharakis BBS ECDSA
- HPI Lehmann 2026 ECDSA proof of possession legacy phones anonymous credentials device binding
- Lehmann Sidorenko Zacharakis Vision ... "Tom-256" OR "tom256" benchmark ms proof size
- "Device Binding for Anonymous Credentials on Legacy Phones" 700 ms BBS ECDSA
- eprint 2025/1981 Vision ... ZKAttest equality across groups secp256r1 BLS12-381 implementation timing

## Sources
- https://eprint.iacr.org/2025/1981
- https://eprint.iacr.org/2025/1995
- https://eprint.iacr.org/2025/824
- https://eprint.iacr.org/2026/965
- https://brave.com/research/device-binding/
- https://cronokirby.com/refs/2026-05-device-binding-for-anonymous-credentials-on-legacy-phones.html
- https://www.ietf.org/ietf-ftp/internet-drafts/draft-cllz-cfrg-ecdsa-pop-00.html
- https://hpi.de/lehmann/eudi.html
- https://github.com/hpicrypto/age-verification
- https://github.com/hpicrypto/ecdsa_pops
- https://eid-privacy.github.io/zkp-vault/Resources/papers/LSZ25-Vision/
