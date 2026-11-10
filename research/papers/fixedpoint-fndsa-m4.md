# Flying, Not Floating: Precise Fixed-Point Falcon on Embedded Processors

Lane: A

## Citation
- Title: "Flying, Not Floating: Precise Fixed-Point Falcon on Embedded Processors"
- Authors: Stef Halmans, Amit Kumar, Tim Güneysu (Ruhr University Bochum; Güneysu also DFKI). Verified on the ePrint landing page.
- Venue / status: ePrint-only preprint. Cryptology ePrint Archive, Paper 2026/2431, category Implementation, CC BY. Received 2026-10-09, revised 2026-10-14. No venue acceptance listed.
- Keywords: FN-DSA, Falcon, Fixed-Point Arithmetic, ARM Cortex-M4, Numerical Precision, Post-Quantum Signatures.

## Reading depth
**Abstract only (plus metadata and search snippets).** The PDF (eprint.iacr.org/2026/2431.pdf and the /archive/ path) returned HTTP 403 on every attempt (2026-11-05). Everything below the abstract is marked UNKNOWN, not guessed. Someone needs to open the PDF in a browser to fill these fields before we rely on them.

## Core claim (one sentence)
A Falcon/FN-DSA signer written in constant-time Cortex-M4 assembly with 32.64 fixed-point arithmetic needs about 45% fewer cycles than double-precision emulation. With the authors' precision-restoring techniques it also measures more precise than double precision (it clears the 2^64 queries-per-key target, which double precision does not).

## Platform
- KNOWN (abstract): ARM Cortex-M4, constant-time assembly, Falcon-512 and Falcon-1024. The baseline is double-precision emulation, apparently c-fn-dsa. The abstract also mentions "lifting double-precision c-fn-dsa above 2^64".
- UNKNOWN: board, clock, compiler and flags, and how cycles were measured. CONJECTURE: probably pqm4 on a NUCLEO-L4R5ZI. The first author's earlier TWFalcon artifact (TCHES 2026, ePrint 2025/1991) uses that board with arm-none-eabi gcc 13. This is not verified for 2026/2431.
- **Cortex-M33: no numbers are mentioned in the abstract or metadata.** The keywords and title name only Cortex-M4 and "embedded processors". Whether M33 appears in the body is UNKNOWN. For our EFR32xG24 (M33) phase 2, assume no M33 data until checked.
- The cycle numbers seem to be measured on real M4 hardware, not a PC. The abstract's precision analysis ("measured precision") was likely done on a host. UNKNOWN.

## Threat model
- Asset: the signing key. The precision bound sets how many signatures per key (queries) can be issued before deviations from the ideal Gaussian sampler could leak key information. Target is 2^64.
- Adversary: someone who collects signatures (statistical or key-recovery attacks that exploit imprecise sampling). The abstract also claims constant-time code, which implies timing adversaries are in scope.
- Side channels: **the abstract claims only "constant-time" assembly. It mentions no TVLA, power or EM leakage evaluation, and no masking.** Whether the body reports any leakage test is UNKNOWN. Related work (De Almeida Braga et al., ePrint 2026/1531) argues that fixed-point is easier to make constant-time and to mask than floating point. That is the motivation, not a measured result.
- Out of scope (as far as the abstract shows): power/EM side channels, fault attacks, verification cost.

## Numbers that matter (all from the abstract; table/figure numbers UNKNOWN)
| Quantity | Falcon-512 | Falcon-1024 |
|---|---|---|
| Cycle reduction vs double-precision emulation | ~45% fewer | ~45% fewer |
| Measured precision, fixed-point (queries/key) | 2^99.7 | 2^96.7 |
| Measured precision, double precision (queries/key) | 2^61.5 | 2^58.7 |
| Target | 2^64 | 2^64 |
- Signing RAM: rises from 59n to 83n bytes because of 12-byte words. For n=512 that is about 30.2 kB to 42.5 kB, and for n=1024 about 60.4 kB to 85.0 kB. These are our arithmetic, not paper figures.
- Key generation bounds for Falcon-1024: 72.3% of keys accepted, versus 8.5% if the Falcon-512 bound values are reused.
- **Absolute cycle counts, stack, and code size are not in the abstract** and are UNKNOWN.

## Signing rate vs application message rate
Nothing in the abstract or metadata discusses signing rate, V2X, 10 Hz messaging, sensors, or energy. CONJECTURE: probably not discussed, since the paper is about precision and arithmetic. Verify in the full text.

## Convenient assumptions
- A Cortex-M4 with DSP (UMULL/UMLAL-class 32x32 to 64 multiplies). The fixed-point design depends on these. M0+/M23 parts would behave differently.
- RAM budget of 83n bytes for signing (about 42 kB for Falcon-512). This is large next to Pornin's 11 kB design. The two are not combined.
- Precision is "measured", which suggests empirical error bounds. The related 2026/1531 work states its main theorem is conditional on empirically derived error bounds.
- The modified keygen rejects keys that break intermediate-value bounds (acceptance 72.3% for Falcon-1024 with their bounds). The bounds assume keygen happens on a capable device.
- No radio or energy cost and no application context.

## Stated limitations / future work
From the abstract only: the authors "provide recommendations for the fixed-point FN-DSA that NIST plans". This suggests the arithmetic is not final while NIST settles the standard's fixed-point variant. The full future-work section is UNKNOWN.

## Related papers (glanced, for positioning only)
- **ePrint 2026/1915**, Thomas Pornin (NCC Group), "A RAM-Efficient Implementation of Falcon" (preprint). Falcon-512 RAM drops from about 31 kB to about 11 kB. Average signing on Cortex-M4 is 13.45M cycles. More than half of the floating-point operations are replaced with modular integer and NTT work, plus a new FFT variant. Relation: 2026/2431 goes the opposite way on RAM (59n to 83n bytes) to gain precision. According to a search snippet (not verified), Pornin lists merging with fixed-point work as future work. This tension is the obvious gap.
- **ePrint 2025/123**, Thomas Pornin, "Falcon on ARM Cortex-M4: an Update" (preprint). Runtime about halved compared with the 2019 implementation. A NIST pqc-forum post by Pornin reports Falcon-512 on Cortex-M4F at keygen 72M, sign 22M, verify 255k cycles. That figure comes from a search snippet, not checked against the PDF. This is the likely FPU-using baseline. It is not the emulated-double baseline that 2026/2431's 45% compares against.
- Neither PDF could be opened (403). Both are abstract-level only.

## Load-bearing references (as inferable from the abstract)
1. De Almeida Braga, Fouque, Lachguel, Prest, "Toward a Secure Fixed-Point Implementation of the Falcon Signature Scheme", ePrint 2026/1531 (CRYPTO 2026). This is the 64.64 fixed-point signer on 64-bit hosts.
2. Pornin, c-fn-dsa / Falcon on Cortex-M4 (ePrint 2025/123). Baseline for double-precision emulation.
3. Halmans et al., "TWFalcon: Triple-Word Arithmetic for Falcon", TCHES 2026 (ePrint 2025/1991). Same first author, earlier precision work on M4.
4. NIST FN-DSA draft (FIPS 206). The standard the recommendations target.

## Possible seeds (direction-miner moves)
- **Port to a new platform:** we found no Cortex-M33 (EFR32xG24) numbers, as far as we searched. Port or estimate 32.64 fixed-point signing on M33. M33 has DSP and FPv5 single precision only, and an M4-to-M33 cycle mapping needs calibration.
- **Combine two orthogonal results:** Pornin's 11 kB RAM layout (2026/1915) plus Halmans et al.'s fixed-point precision (2026/2431). What are the RAM, cycle and precision trade-offs for a merged signer?
- **Add the missing measurement:** the abstract shows a constant-time claim but no TVLA/EM evaluation. A fixed-vs-random TVLA on the fixed-point signer would be the phase-2 side-channel work.
- **Put it in an application context:** signing rate and energy against a 10 Hz V2X/BSM or sensor-attestation budget, including the 2^64 query-per-key cap. Example: how long a key lasts at 10 Hz. Double precision's 2^61.5 for Falcon-512 is still far beyond any device's signing lifetime. CONJECTURE: so the precision gap matters more for security proofs than for deployment.
- **Masking:** fixed-point removes the floating-point obstacle to masking. Measure the cost of a first-order masked fixed-point FFT sampler on M4/M33.

## Queries used
- eprint.iacr.org/2026/2431 (landing page OK, PDF 403)
- eprint.iacr.org/2026/1915, eprint.iacr.org/2025/123 (landing pages OK, PDF 403)
- "Flying, Not Floating" Falcon fixed-point Cortex-M4 Halmans
- "Precise Fixed-Point Falcon on Embedded Processors"
- iacr.org/news/item/29117 (2026/1531 abstract)
