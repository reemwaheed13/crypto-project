# Collision check: N4, side-channel leakage in the FHE client's samplers on a Cortex-M

Date: 2026-11-10. Checker: collision-check subagent. Per the caller's instruction, only this file was written (research/collision-log.md was not edited).

## Candidate
- Application: an IoT sensor encrypts its readings directly under BFV or CKKS on a Cortex-M (SEAL-Embedded on an nRF52840 Cortex-M4; Zeppelin BFV on a Cortex-M55, arXiv 2610.08301, not opened here) and uploads the ciphertexts.
- Asset: the plaintext readings. They can be recovered by learning the encryption randomness u, e0, e1: in public-key encryption c0 = pk0*u + e0 + Delta*m, so knowing u and e0 gives m.
- Adversary: physical power/EM access to the sensor (phase 2 on an EFR32MG24 Cortex-M33 kit). This semester, a software leakage assessment with an emulator and an instruction/data-dependent leakage model.
- Claim: the samplers in embedded HE clients (error, ternary secret/u, and encoding) leak in a way that can be exploited, like RevEAL's attack on SEAL's Gaussian sampler, and a constant-time fix has a measurable cost on the MCU.

## Verdict: PARTIAL OVERLAP (close to collision for the sampler part)

## Queries run (all web search, US)
1. `SEAL-Embedded side channel attack`
2. `RevEAL single-trace side-channel leakage SEAL homomorphic encryption library Gaussian sampler`
3. `homomorphic encryption client power analysis microcontroller` (plain phrasing)
4. `eprint.iacr.org side-channel BFV encryption ternary secret sampling attack` (ePrint)
5. `CKKS encoding side channel embedded single trace message recovery`
6. `single-trace attack homomorphic encryption sampler Cortex-M 2025` (date variant)
7. `Aydin Aysu homomorphic encryption side-channel 2024 2025 SEAL-Embedded OpenFHE` (follow-ups)
8. `HOST CHES power side-channel fully homomorphic encryption edge device sensor encryption leakage countermeasure masking` (hardware-security community)
9. `side-channel attack CKKS BFV encryption microcontroller 2026` (extended, date variant)
10. `"SEAL-Embedded" leakage OR "side-channel" OR "constant-time" sampler nRF52840`
11. `federated learning homomorphic encryption IoT client power side-channel leaks plaintext gradients encryption device` (neighbouring ML community)
12. `CKKS encoding decoding randomization side-channel attack TenSEAL SEAL revised protection`
13. `SEAL-Embedded: A Homomorphic Encryption Library for the Internet of Things TCHES 2021 sampling constant-time`
14. `single-trace attack centered binomial sampler Kyber Cortex-M4 message recovery encryption` (embedded PQC community)

## Papers and sources opened
| ID | What | Verified | Depth |
|---|---|---|---|
| ePrint 2022/204 / DATE 2022 (Aydin, Karabulut, Potluri, Alkim, Aysu), "RevEAL" | Single-trace power attack on the Gaussian sampler in SEAL v3.2 BFV encryption (sign-dependent branches), run on a RISC-V FPGA; security drops from 2^128 to 2^4.4 | yes (ePrint/DATE listing) | abstract + secondary summaries (the full text is being read by the paper-reader, research/papers/reveal-seal-sampler-sca.md) |
| ePrint 2023/1128; J. Cryptogr. Eng. 2024, DOI 10.1007/s13389-023-00340-2 (Aydin, Aysu) | Single-trace NN attack on the NTT of ternary values in SEAL CKKS **key generation**, run on an **ARM Cortex-M4F**; 98.3% accuracy at -O0 and 98.6% at -O3; random delay insertion fails | yes | ePrint abstract page (the PDF could not be rendered) |
| ASHES 2022 (Aydin, Aysu), "Exposing side-channel leakage of SEAL" | Two-stage NN profiler for SEAL's Gaussian sampling | seen in search only | snippet |
| arXiv 2505.11058 = ePrint 2025/867 (Ghaleb, Buchanan), survey "Side Channel Analysis in HE" | Lists the HE SCA work. **No SEAL-Embedded, no CBD encryption sampler, no CKKS encoding attack, and no MCU cost for countermeasures** | yes | full HTML read via the fetch summary |
| ePrint 2025/2302 / DATE 2026 (Graz) | CPA on the HERA hybrid-HE cipher for edge devices, recovering the key from 40 traces, plus masking (PINI, TVLA) | seen in search | snippet |
| TCHES 2025 (Peng, Kuhn), "Adaptive Template Attacks on the Kyber Binomial Sampler" | Single-trace template recovery of **CBD sampler outputs on a Cortex-M4** | seen in search | snippet |
| TCHES 2025 (Fouque, Marion, Nguyen, Wallet) | Single-trace CBD leakage in Kyber keygen + lattice reduction | seen in search | snippet |
| ePrint 2026/2137 (Jahandideh) | ML-KEM keygen sampler single trace on pqm4 M4; in-sampler countermeasure costs 937 B of firmware | seen in search | snippet |
| ePrint 2026/071 | HQC side-channel/replay attack (not HE) | yes | abstract; not relevant |
| github microsoft/SEAL-Embedded device/lib/sample.c | **The CBD sampler (get_cbd_val, hamming_weight) is branchless. The ternary mod-3 path is explicitly constant time with a mask-based mapping. Residual branches: the rejection loop (on discarded randomness) and convert_poly_ternary `if (src[i] > 1)`** | yes (source) | full file via the fetch summary |

## KNOWN (cited)
- Branch-based single-trace sampler leakage in HE encryption: RevEAL (2022/204), but only on SEAL v3.2 and a RISC-V FPGA.
- Single-trace ternary leakage through the NTT on a Cortex-M4F: 2023/1128. That paper attacks keygen, but SEAL-Embedded runs the ternary u in encryption through the same kind of NTT.
- Single-trace template recovery of CBD samples on a Cortex-M4: Peng and Kuhn (TCHES 2025), Fouque et al. (TCHES 2025), ePrint 2026/2137 (with the countermeasure cost).
- SEAL-Embedded's CBD sampler and its ternary mod-3 sampler are written branch-free (source code).
- The survey 2505.11058 lists no SCA on SEAL-Embedded or any other embedded HE client, and no attack on CKKS encoding.

## CONJECTURE (ours)
- The RevEAL-style branch leak probably does not exist in SEAL-Embedded. Leakage would instead be Hamming-weight and value dependent in the CBD and ternary outputs and in the NTT of u. That is a port of Kyber CBD template attacks and 2023/1128, not a new kind of leak.
- The CKKS encoding (IFFT, rounding of the scaled message) and the convert_poly_ternary branch may leak m or u directly. As far as searched, no paper attacks these on an MCU.
- Zeppelin (Cortex-M55, Helium vector) has not been checked by anyone; vector units may change the leakage.

## ASSUMPTION
- Emulator leakage models (for example, an HW/HD model in the style of ELMO/Rosita on M0/M4) stand in well enough for the M33/M4 for a semester result. They are not calibrated against an EFR32MG24 until phase 2.
- arXiv 2610.08301 (Zeppelin) is as the caller described it. This checker did not open it.

## What remains open (as far as searched)
1. A leakage assessment of an actual embedded HE client library (SEAL-Embedded and/or Zeppelin) covering the full encrypt path: sampling, encoding, the NTT of u, and pk*u. No paper found.
2. Leakage in the CKKS encoding/IFFT stage on an MCU, as a direct route to m.
3. The measured cost on an MCU of a protected HE client encryption (masking or shuffling the NTT of u, plus a protected encoder). There are no HE numbers. The only nearby number is the ML-KEM sampler countermeasure in 2026/2137.

## Most damning reviewer objection
"SEAL-Embedded's samplers are already branch-free, so RevEAL does not carry over. What remains is the CBD/ternary Hamming-weight leak and the NTT leak, already shown single-trace on a Cortex-M4 for Kyber (Peng and Kuhn, TCHES 2025; Fouque et al., TCHES 2025) and for SEAL's ternary NTT (Aydin and Aysu, JCE 2024). Without a physical trace, an emulator leakage-model result is a TVLA exercise, not an attack." To get past this, the paper has to aim at the encoding/IFFT path or at the Zeppelin vector path, and show real message recovery rather than just detected leakage.
