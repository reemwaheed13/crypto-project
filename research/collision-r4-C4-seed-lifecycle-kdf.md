# Collision check R4 / C4: seed lifecycle for SecAgg on sensors (KDF per round from one long-term pairwise key)

Date: 2026-11-11. Checker: collision-check subagent.

## Candidate
Do pairwise key agreement (X25519 or ML-KEM) once per deployment, then derive each round's pairwise mask seeds with a KDF (HKDF / KMAC / AES-CMAC over the round number). Quantify the energy and radio rounds saved on an EFR32MG24 over 802.15.4. Subtlety to check: does naive key reuse break SecAgg once a dropped client's seeds are revealed?

## Verdict: PARTIAL OVERLAP (heavy). The crypto idea is taken; only the MCU and 802.15.4 cost measurement is open as far as searched.

## Queries run
1. `multi-round secure aggregation one-time setup pairwise key PRF round number dropout leakage later rounds` (technical)
2. `reuse pairwise masks across rounds secure aggregation federated learning insecure dropped client` (plain language)
3. `eprint.iacr.org secure aggregation multiple rounds setup once key reuse` (IACR ePrint)
4. `secure aggregation IoT 802.15.4 energy key agreement per round lightweight KDF sensor nodes federated learning` (IoT and networking)
5. `secure aggregation microcontroller Cortex-M energy measurement pairwise masking key agreement amortized rounds 2025` (embedded, 2025)
6. `e-SeaFL one-time setup assisting nodes PRF iteration masks; MicroSecAgg multi-round; SASH secure aggregation`
7. `lightweight secure aggregation wireless sensor network federated learning pre-shared key hash chain per-round mask energy Zigbee Thread`
8. `MicroSecAgg PoPETs 2024 multi-round secure aggregation one-time setup dropout`
9. `2026 secure aggregation key reuse forward secrecy per-round key derivation federated learning IoT devices` (2026)
10. `FS2-PQC-FL forward secrecy symmetric ratchet ML-KEM secure aggregation IoMT`

## Papers opened
| ID | Paper | Verified | Reading depth | Overlap |
|---|---|---|---|---|
| arXiv 2308.09883 (v4) | Flamingo, Ma et al., IEEE S&P 2023 | yes, arXiv HTML | first ~100k of 208k characters (protocol, dropout, cost sections) | **Kills the crypto design.** The long-term pairwise secret r_ij gives per-round seeds h_ij,t = PRF(r_ij, t). Clients encrypt the per-round seeds to decryptors, so a dropout reveals only that round's h_ij,t and never r_ij. The paper states the naive reuse attack outright ("all the r_ij are reconstructed in round 1 and are reused in round 2, the server can derive both masks"). It also says the PRF trick cannot be bolted onto BBGLR, because there the server reconstructs r_ij itself. Evaluation is on a Xeon/ABIDES simulator only; no MCU, energy, or radio. |
| ePrint 2023/1936 | LERNA, Li, Lin, Polychroniadou, Tessaro, ASIACRYPT 2023 | yes, ePrint page | abstract and metadata only | Reusable setup across aggregations saves one round per aggregation. Lattice/DCR-based. No IoT evaluation. |
| arXiv 2604.19053 | CHRONOS, H. Dang, Apr 2026 | yes, via emergentmind summary | summary-level | **Closest on the systems side.** X25519 once per epoch, HKDF to PRG keys, an AES-128-CTR mask bound to the round counter inside a TrustZone TEE, run on 20 Rock Pi 4 boards (Linux SoC). Energy is analytical, not measured. It explicitly lists a Cortex-M / TF-M port as an open question. |
| arXiv 2502.08989 v4 | "Setup Once, Secure Always", Sultan et al., 2025 | yes, arXiv HTML | first ~100k of 142k characters | Single setup for all rounds. Criticises Flamingo and e-SeaFL for having no forward or backward secrecy (one leaked long-term seed exposes all rounds). Uses fresh masks rather than a KDF. VM evaluation only. |
| arXiv 2410.09676 | Zhang, Behnia, Yavuz et al., 2024 | search snippets only | snippets | Attack on MicroSecAgg: reusable masks stay constant across iterations, so differencing consecutive masked updates leaks data. This is the reuse pitfall in practice. |
| arXiv 2304.03841 | e-SeaFL, Behnia et al., ACSAC 2024 | search snippets only | snippets | Long-term seeds shared with assisting nodes are expanded by a PRF per iteration. Not read in full. |
| QREXQ 2026 abs 0320 | FS2-PQC-FL (workshop) | search snippets only; PDF would not parse | snippets | ML-KEM plus a symmetric ratchet that gives fresh per-round SecAgg keys (forward secrecy), for IoMT ECG. Device class not confirmed. |
| arXiv 2502.14464 | PQBFL, Gharavi et al., 2025 | abstract | abstract | Ratcheting for forward and post-compromise secrecy in FL. Not SecAgg-specific. No device evaluation. |

Not found: "SASH" (no hit for a SecAgg paper by that name). Chu-ko-nu (arXiv 2402.15111) appeared in results; I did not open it.

## What remains open (as far as searched)
- No paper found that measures the one-time key agreement plus per-round KDF lifecycle on a Cortex-M33 sensor MCU (EFR32MG24 class). That means no measured cycles or energy per round for X25519 or ML-KEM against HKDF/KMAC/CMAC, and no count of 802.15.4 frames or radio round-trips saved. CHRONOS stops at Linux SoCs and names the MCU port as future work.
- What survives is an engineering and measurement contribution. The protocol idea itself is not new.

## Most damning objection
Flamingo (S&P 2023) already does exactly this derivation (PRF of the long-term key and the round number), proves it secure, and spells out the attack the candidate worried about. Its fix needs more than a KDF: clients encrypt per-round seeds to a decryptor committee, so the server never learns r_ij. A plain "BBGLR plus KDF" is insecure for the reason Flamingo gives. So the candidate either adopts Flamingo's decryptor machinery, whose cost on 802.15.4 is the actual open number, or it is insecure. "Energy saved by not re-keying" also comes down mostly to arithmetic: ratio of key-agreement cost to KDF cost, times the number of rounds. A reviewer may call that obvious unless the Flamingo decryptor overhead and dropout recovery are measured too.

## KNOWN / CONJECTURE / ASSUMPTION
- KNOWN: per-round PRF(long-term key, t) seed derivation and its security under dropouts (Flamingo). Reusable setup (LERNA, MicroSecAgg, e-SeaFL). Lack of forward secrecy in static-seed schemes (Sultan et al. 2025). Constant-mask attack (Zhang et al. 2024). Once-per-epoch X25519 plus HKDF on TEE SoCs (CHRONOS).
- CONJECTURE: Flamingo-style seed lifecycle on an EFR32MG24 cuts per-round radio round-trips and energy enough to matter for battery sensors, once decryptor-committee traffic is counted.
- ASSUMPTION: a stable sensor population (few new joins) so one-time setup amortises; a semi-honest server; long-term keys stored without leaking (side channels are out of scope for phase 1).
