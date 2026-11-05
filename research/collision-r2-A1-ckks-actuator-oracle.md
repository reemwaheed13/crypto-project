# Collision check, round 2, A1: Actuators as decryption oracles (CKKS IND-CPA-D in closed-loop IoT)

Date: 2026-11-05. Checker: collision subagent.

## Candidate
Cloud computes on CKKS-encrypted sensor data and returns an encrypted control decision. The IoT device decrypts it and acts. The adversary is a curious or malicious cloud that sees only the device's thresholded, 1-bit action. Claims: (1) Li–Micciancio-style key recovery still works with 1-bit feedback and a practical query count; (2) noise flooding at decryption on a Cortex-M33 is a cheap fix.

## Queries run
1. "CKKS IND-CPA-D key recovery one-bit decryption feedback threshold oracle attack" (extended). FAILED with a too_many_requests error, so no results. Treat this query as not run.
2. "encrypted control homomorphic encryption actuator decryption leaks secret key attack" (standard). Found control-community papers on covert, zero-dynamics, and malleability attacks. None was a key-recovery paper.
3. "eprint.iacr.org reaction attack homomorphic encryption decryption failure oracle key recovery 2025" (standard). Found ePrint 2026/2490, the closest hit.
4. "IoT device decrypts FHE result acts on it attack smart thermostat cloud learns key" (standard, plain language). No paper matched.
5. "CPA-D attack encrypted control system CKKS decrypted control input observed by adversary" (standard, neighbouring community: control/CPS). Found arXiv 2607.12742.
6. "CKKS key recovery comparison sign decryption oracle bit leakage attack 2025 2026" (standard, date-restricted). Found 2026/2490 again, plus Li–Micciancio and the noise-flooding line.

Search-coverage gaps: query 1 failed. No query searched ePrint for "IND-CPA-D threshold decryption bit". 2024/424, 2024/853, and 2024/127 were not opened in this round.

## Papers opened
| ID | Title | Status | Depth |
|---|---|---|---|
| IACR ePrint 2026/2490 | Bian, Choe, Kim, Lee, "Reaction Attack on CKKS Deployments with Asymptotically Optimal Query Complexity" (posted 2026-10-15; the ePrint page says it was accepted to IEEE S&P 2027) | Verified: ePrint page fetched | Abstract page only |
| arXiv 2607.12742 | "Stability Buys Time: A Re-Keying Game for Encrypted Multi-Agent Control" | Verified: arXiv abstract fetched. Authors not recorded. | Abstract only |

Seen only in search snippets and not opened: Li–Micciancio (EUROCRYPT 2021, ePrint 2020/1533); Checri et al. CPAD on threshold BFV/BGV/CKKS (iacr.org news 22392); Nabokov et al. noise-flooding attack on OpenFHE; ePrint 2023/561 (vr2FHE); covert and zero-dynamics attacks on encrypted controllers.

## Findings
- KNOWN (2026/2490): CKKS key recovery from a client's **1-bit validity reaction** with O(H) queries. For a uniform ternary secret this is O(N), within a factor of 2 of the entropy bound. The attack runs end to end in Lattigo and does not break IND-CPA. It **needs public coefficient-to-slot (C2S, bootstrapping) evaluation keys** and a validity check. The abstract mentions no IoT, control, or countermeasure costs. This takes claim (1) in its general form.
- KNOWN (2607.12742): in encrypted control, the loop must decrypt to actuate, so CKKS decryptions leak the key, and re-keying is the defence. This takes the "actuator decryption leaks the key" framing for control loops. The abstract describes passive leakage from decrypted values. It does not describe a thresholded, 1-bit actuation channel.
- No collapse into the round-1 exclusions (transciphering, PQ handshake energy, gradient inversion, DP/secure aggregation).

## Verdict: Partial overlap (borderline collision)
What remains, as far as searched:
(a) Key recovery when the only feedback is a **thresholded actuation** (relay on/off), in a **leveled, no-bootstrapping-key** deployment like those common in encrypted control. 2026/2490's C2S gadget would be unavailable there.
(b) Measured cost of decryption-side noise flooding (DecodePublic-style) on a Cortex-M33 / EFR32xG24. This part alone is a cost measurement and is thin as a paper.
Before committing, read 2026/2490 in full: it may already cover leveled or no-C2S variants in its discussion section. Also read the 2607.12742 leakage model.
