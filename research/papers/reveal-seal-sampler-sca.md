# RevEAL: Single-Trace Side-Channel Leakage of the SEAL Homomorphic Encryption Library

## Citation
- Title: RevEAL: Single-Trace Side-Channel Leakage of the SEAL Homomorphic Encryption Library
- Authors: Furkan Aydin, Emre Karabulut, Seetal Potluri, Aydin Aysu (NC State University); Erdem Alkim (Dokuz Eylul University)
- Venue/status: **published** at DATE 2022 (Design, Automation and Test in Europe). Also on Cryptology ePrint Archive as 2022/204 (received 2022-02-20; the ePrint page reads "Preprint. MINOR revision.").
- IDs: ePrint 2022/204. DOI 10.23919/DATE54114.2022.9774724 comes from a search-result snippet and was not checked on the IEEE page. NSF PAR 10403568.
- Page numbers disagree between sources. The page footers of the DATE proceedings PDF read 1531–1536 (ISBN 978-3-9819263-6-1). NSF PAR and the 2025 survey (arXiv 2505.11058) give 1527–1532. Cite the footer pages, or check IEEE Xplore.

## Reading depth
**Full text**: all 6 pages of the DATE proceedings PDF (past.date-conference.com/proceedings-archive/2022/pdf/0341.pdf), including Fig. 2–3, Tables I–IV and the references. The ePrint PDF returned HTTP 403, so the ePrint version was not compared against the DATE version.

## Core claim
In SEAL before v3.6, the BFV encryption Gaussian error sampler branches on the sign of each coefficient and writes the value in a data-dependent way. A single power trace of one encryption therefore reveals enough about the error polynomials e1 and e2 to recover the plaintext: the LWE-with-hints estimate drops from 382.25 bikz (about 2^128) to 12.2 bikz (about 2^4.4).

## Platform (KNOWN, Sec. IV-A)
- Hardware: SAKURA-G board (Xilinx Spartan-6 FPGA) running a **PicoRV32 RISC-V softcore (RV32IM)**. **No commercial MCU was used, and no Cortex-M.**
- Clock: fixed at 1.5 MHz, justified as "the range of RFIDs/MCUs" (refs [32], [33]: STM8 and Maxim secure MCUs).
- Measurement: 1 Ω shunt on the main supply via the board's SMA port; PicoScope 6424E at 1 GS/s.
- Software: **Microsoft SEAL v3.2**, BFV, function `Encryptor::set_poly_coeffs_normal` (Fig. 2) with `ClippedNormalDistribution` (C++ std RNG adapter, double-precision normal, rejection above max deviation, rounding).
- Parameters: SEAL-128, n = 1024, q = 132120577, σ = 3.19 (≈ 8/√(2π)), coefficients clipped to [-41, 41]. Values observed in 220,000 runs fell in [-14, 14].
- Compiler and flags: **not stated.** This matters, because the authors' follow-up (ePrint 2023/1128) found that -O0 and -O3 leak differently.
- Cost measurements: none. This is an attack paper with no timing, memory or energy figures.

## Threat model (KNOWN, Sec. II-B)
- Application framing: a client device encrypts and decrypts, and the cloud evaluates (Fig. 1). The attack targets **device-side encryption**.
- Asset: the plaintext message m being encrypted. The secret key is not targeted.
- Adversary: knows SEAL source code and parameters, has physical access to measure power, and can **profile the same device beforehand** (template attack with configurable samples). Does not know m at attack time. Gets one trace per encryption, because e1, e2 and u are fresh each time.
- How m is recovered: given the public pk and ciphertext, knowing e1 and e2 gives u = (c1 − e2)/p1 and then m (Eq. 2–3). Note that u is sampled uniformly from R2 and is not attacked directly.
- Out of scope: decryption (multi-trace attacks are called "relatively straightforward extensions"), NTT, polynomial multiplication, encoding, key generation, cross-device attacks, SEAL ≥ v3.6.

## Leaks exploited (KNOWN, Sec. III-B)
1. **Control flow**: the if / else-if / else on `noise > 0`, `< 0`, `== 0` reveals sign and zero. Branch identification succeeded 100% of the time (Sec. IV-B, footnote 4).
2. **Data flow**: the assignment `noise = dist(engine)` leaks Hamming weight. Many values share a HW, which causes false positives.
3. **Negation**: `noise = -noise` for negative values. Two's-complement HWs separate values that share a HW (for example -2 and -3), so negative values are recovered much better than positive ones.
- They did not attack `ClippedNormalDistribution` itself, because its 64-bit internals would need 2^64 templates.
- Segmentation: visible peaks mark each coefficient's sampling (Fig. 3a), even though the distribution call does not run in constant time. Branch sub-traces are visually distinct (Fig. 3b).

## Numbers that matter
- Profiling: 220,000 sampling runs for templates. Attack: 25,000 traces, each attacked as a single trace to estimate the success rate (Sec. IV-B). Points of interest selected by SOSD; Chari-style templates.
- Table I (predicted value in rows, true value in columns, %): zero 100; -1 95.7; -2 92.5; -4 91; -3 60.7; -5 54.9; -6 54.2; -7 41.2. **Positive values are weak**: +1 31.8, +2 27.7, +3 23.5, +4 20.6, +5 18.1, +6 17.2, +7 16.
- Table III: SEAL-128 primal attack cost is 382.25 bikz without hints and **12.2 bikz** with hints (about 2^4.4, described as a complete break). Hints come from the Dachman-Soled et al. CRYPTO 2020 framework (perfect and approximate hints).
- Table IV, sign/zero (branch) leak only: 253.29 bikz (about 2^84.34). With 1 guess, 252.83 bikz and 20% success probability. **The sign alone does not break the scheme**; the value leak is required.
- Caveat (inference): Table II shows posterior probabilities near 1 for positive coefficients, even though Table I gives only about 30% top-1 accuracy for them. The authors say the two tables are computed differently: Table I uses the argmax template, Table II the full template probabilities. The 12.2 bikz result therefore depends on how the hint framework turns template scores into hints. This should be checked before reusing the number.

## Countermeasure and its cost (KNOWN)
- **No countermeasure was implemented or measured, so its cost is unknown.** Sec. V-A and VI only recommend shuffling or other randomization, **not masking** (masking is known to fail against single-trace attacks, citing Primas et al. CHES 2017), and "better software coding practices to eliminate conditional executions on sensitive values."
- Vendor fix: SEAL v3.6 replaced the if-else with an iterator function (ref [35]). The authors say v3.6+ "may have a different vulnerability", which is left for future work.
- Inconsistency: the NC State press release (as summarized by search) says the vulnerability was confirmed through at least v3.6. The paper itself says v3.6 changed the sampler and was not evaluated. Trust the paper.

## Were SEAL-Embedded or other embedded HE clients examined?
**No.** SEAL-Embedded is not mentioned anywhere in the paper. The only target is desktop SEAL v3.2 C++ cross-compiled for a RISC-V softcore. No Cortex-M device and no embedded-specific HE library was examined.

## Convenient assumptions
- Profiling happens on the same device with full control of the sampled values; the paper notes that cross-device attacks may need ML profiling.
- A 1.5 MHz clock on an FPGA softcore with a low-noise shunt is far quieter than a real MCU at 64–800 MHz. The authors themselves note that higher clocks need better equipment.
- The double-precision normal sampler on RV32IM, without an FPU, is probably emulated in software, which gives a long and distinctive trace (inference, not stated).
- Single parameter set (n = 1024). The claim that the attack applies to all n is not tested. The paper also says higher security levels would be harder.
- No radio, energy or cost model, which is expected for an attack paper.

## Stated limitations and future work (paraphrased)
- Template attacks need many profiling traces and control over the sampled values, and face the curse of dimensionality.
- Single device only.
- Low clock frequency only.
- Higher security levels were not shown and are expected to be harder.
- SEAL v3.6+ left for future work.
- Combining defenses is suggested as the best option, but none were evaluated.

## Load-bearing references
- [31] Dachman-Soled, Ducas, Gong, Rossi, "LWE with side information", CRYPTO 2020: the hint-to-bikz framework behind the 2^4.4 figure.
- [28] Chari, Rao, Rohatgi, "Template attacks", CHES 2002: the profiling method.
- [15] Primas, Pessl, Mangard, "Single-trace side-channel attacks on masked lattice-based encryption", CHES 2017: the reason given for not recommending masking.
- [11] Espitau et al., BLISS branch-tracing and EM attacks on microcontrollers, CCS 2017, and [10] Kim et al., CDT sampler single-trace analysis, 2018: prior sampler side-channel work that RevEAL distinguishes itself from.
- [27]/[35] Microsoft SEAL releases 3.2 and 3.6: the vulnerable and patched sampler code.

## Follow-up work found (2022–2026) on HE client side channels
| Work | ID / venue (as found) | Target | Depth checked |
|---|---|---|---|
| Aydin, Aysu, "Exposing Side-Channel Leakage of SEAL Homomorphic Encryption Library" | ASHES 2022 (CCS workshop), pp. 95–100, DOI 10.1145/3560834.3563833 | SEAL **key generation**: ternary value assignment in the NTT. Single trace, power/EM, **ARM Cortex-M4F**, 98.3% coefficient accuracy; random delay insertion does not stop it | abstract via search snippets only (ACM returned 403) |
| Aydin, Aysu, "Leaking Secrets in Homomorphic Encryption with Side-Channel Attacks" | ePrint 2023/1128; J. Cryptographic Engineering vol. 14 (2024) pp. 241–251, DOI 10.1007/s13389-023-00340-2 | Journal extension: SEAL (v4.1 per survey and snippets, CKKS per survey) keygen NTT; -O0 attack on add/sub, -O3 attack on guard and mul_root; ML profilers; Cortex-M4F; 98.3% (-O0) and 98.6% (-O3) | abstract only (ePrint PDF 403). One search snippet suggested SEAL-Embedded is mentioned, which is **unverified** |
| Cheng et al., "Cache-timing attack on the SEAL homomorphic encryption library" | PROOFS 2022 (per survey; no ID verified) | Barrett reduction conditional subtraction; remote cache timing, not an MCU client | survey summary only |
| Ghaleb, Buchanan, "Side Channel Analysis in Homomorphic Encryption" | arXiv 2505.11058 (also ePrint 2025/867 per search) | Survey. **Does not mention SEAL-Embedded**; no MCU-client threat model | HTML read via fetch summary |
| "Zeppelin: Client-Side BFV Encryption and Decryption for Helium-Powered Microcontrollers" | arXiv 2610.08301 (search snippet; not opened by me) | Cortex-M55 BFV client. Per the snippet, it claims constant time against timing channels with branch-free, fixed-keystream ternary and Gaussian samplers, and leaves **power/EM evaluation as future work** | snippet only |

Searches run (Nov 2026): "side-channel attack SEAL-Embedded microcontroller homomorphic encryption client"; "single-trace power attack homomorphic encryption error sampling ternary SEAL v3.6 later 2023 2024 2025"; "side-channel attack homomorphic encryption encryption noise sampling CKKS OpenFHE Cortex-M 2024 OR 2025 ePrint"; "SEAL-Embedded OR Zeppelin BFV Cortex-M side-channel power analysis sampler constant-time"; "power side-channel attack CKKS encryption client microcontroller error sampling single trace 2025 2026".
**Result: no power/EM attack on the SEAL-Embedded samplers (ternary u, discrete Gaussian / CBD error) or on Zeppelin's samplers was found. This is open as far as searched, not proven open.** The searches did not cover IEEE Xplore, Google Scholar citations of RevEAL, or the TCHES/CHES 2023–2026 tables of contents. Do those before claiming novelty.

## Possible seeds (CONJECTURE; generic direction-miner moves)
- **Re-target to the modern embedded client**: run the RevEAL pipeline (segment, then branch/sign leak, then value templates, then LWE-with-hints) on SEAL-Embedded's encryption samplers on a real Cortex-M4 or the EFR32xG24 (M33). Neither the paper nor the follow-ups touch encrypt-time sampling on an embedded HE library.
- **Constant-time is not leak-free**: test whether branch-free samplers (Zeppelin's claim; SEAL v3.6 iterator) still leak HW single-trace, as the GALACTICS BLISS CDT attack showed for lattice signatures.
- **Make the realism gap concrete**: RevEAL ran at 1.5 MHz on an FPGA softcore. Measuring how template accuracy degrades at real MCU clocks and with cross-device profiling is a stated limitation nobody has closed (as far as searched).
- **Cost the countermeasure**: shuffling was recommended but never costed. Cycle and RAM overhead of shuffled or branch-free sampling on a Cortex-M client is measurable in emulation in phase 1 and on hardware in phase 2.
- **Leakage-to-security accounting**: reuse the Dachman-Soled framework, which needs software only, to ask how much partial leakage (sign only, HW only) of the ternary u versus the error e costs in BFV/CKKS client encryption. This fits the software-first phase 1. Table IV's sign-only result (2^84) is one data point.
- Caution: the plaintext-recovery route depends on attacking e1 and e2 for the public-key encryption. For symmetric/secret-key encryption modes used by embedded clients (SEAL-Embedded supports both, per its own paper, not verified here), the algebra changes. Check before porting.

Sources: https://past.date-conference.com/proceedings-archive/2022/pdf/0341.pdf ; https://eprint.iacr.org/2022/204 ; https://eprint.iacr.org/2023/1128 ; https://arxiv.org/html/2505.11058v1 ; https://arxiv.org/html/2610.08301 (snippet) ; https://par.nsf.gov/biblio/10403568
