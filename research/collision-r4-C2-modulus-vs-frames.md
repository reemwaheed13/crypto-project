# Collision check, round 4, candidate C2: SecAgg modulus 2^k chosen jointly with DP noise to cut 802.15.4 frames

Date: 2026-11-11. Checker: collision-check subagent.

## Candidate (as given)
Smart meters (EFR32MG24, 802.15.4, 250 kbit/s, 127-byte frames, about 102 B payload) upload a 16,608-parameter DP-SGD update.
The float32 update is quantised to integers mod 2^k and masked; the server sums N masked vectors mod 2^k.
Claim: the smallest safe k (sum of N clipped, noised, quantised updates does not wrap, with failure probability below a target) follows from clip norm, DP sigma, N and quantisation step. This roughly halves the 66 kB upload or better, and the radio energy saved per round outweighs every other crypto cost.

## Verdict: PARTIAL OVERLAP, close to a collision on the core claim

## Queries run (all `standard` unless marked)
1. `secure aggregation modulus bit-width differential privacy wraparound modular clipping distributed discrete Gaussian` (technical)
2. `federated learning secure aggregation IEEE 802.15.4 frames communication cost smart meter quantization` (networking/IoT)
3. `eprint.iacr.org secure aggregation bit width quantization differential privacy communication modulus` (IACR ePrint)
4. `how many bits per coordinate secure aggregation smallest modulus no overflow sum of clients federated learning` (plain language)
5. `secure aggregation LoRa OR 802.15.4 OR BLE low-power wireless packets federated learning energy masking` (networking/embedded)
6. `2025 2026 differentially private secure aggregation bit-width selection modular wraparound probability bound clients noise` (extended, date-targeted)
7. `Skellam mechanism federated learning bit-width modular clipping Agarwal Kairouz Liu communication` (targeted prior work)
8. `quantized secure aggregation differential privacy IoT microcontroller energy per round radio transmission bits per parameter 2025` (extended, embedded/ML, date-targeted)

## Papers opened
| ID | Paper | Verified | Reading depth | Finding |
|---|---|---|---|---|
| arXiv 2102.06387 (v4 HTML) | Kairouz, Liu, Steinke, Distributed Discrete Gaussian, ICML 2021 | yes (abstract page + HTML full text) | Full text, both halves fetched via the extraction tool; Thm 2, Sec 4.3-4.4, Sec 5 checked | Sec 5 heuristic: pick gamma so that 2k*sigma_hat <= 2^B, with sigma_hat^2 = c^2 n^2/d + (gamma^2/4 + sigma^2) n. That gives the needed bits from clip c, noise sigma, N=n, quantisation step gamma and dimension d, which is the candidate's core formula. Thm 2 gives the asymptotic m >= O~(n + sqrt(eps^2 n^3/d) + sqrt(d)/eps). Prop 35 bounds the expected modular-clipping error; no explicit tail probability for wraparound. Experiments use B about 12-18 with k = 2, 3, 4. No IoT, radio, packet or energy content. |
| arXiv 2110.04995 (ar5iv HTML) | Agarwal, Kairouz, Liu, Skellam mechanism, NeurIPS 2021 | yes | Full text via the extraction tool (first 100k of 103k characters) | Same rule: 2^b = 2k*sigma_tilde, sigma_tilde = sqrt(c^2 n^2/d + n/(4 s^2) + mu). Wraparound is controlled by the k multiplier; no explicit probability. No IoT, radio or energy content. |
| arXiv 2610.00695 (HTML v1) | Progressive-Resolution Secure Aggregation for FL, 2026 | yes | First 100k of 118k characters via the extraction tool | Overflow-margin rules with DP lattice-Gaussian noise. Worst case: m about log K. Concentrated, with an explicit failure probability delta_w: m about (1/2) log K + O(1). This is an explicit tail-probability margin rule, so it removes the "explicit failure probability" angle. Communication counted in bits per dimension only; no packets, frames or energy. |
| arXiv 1912.00131 | Bonawitz et al., Autotuned communication-efficient SecAgg | snippet only | search snippet | Modulus = n*kappa (about log2 n headroom bits) prevents overflow; tunes SecAgg parameters for quantised inputs. |
| arXiv 2511.23252 | One-Shot Secure Aggregation for IoT, 2025 | title/refs only (PDF did not extract) | metadata + search snippet | Argues that radio-on time and round count dominate IoT energy; no modulus-vs-DP sizing seen. |
| PMLR v162 chen22c | Chen, Kairouz, Ozgur, Fundamental price of SecAgg in DP FL, ICML 2022 | snippet only | search snippet | Lower bound on bits per client under SecAgg+DP; theory, not device-level. |

Not opened (snippet only): SecAgg+ (Bell et al. CCS 2020), cpSGD, ScionFL 2210.07376, DDP-SA 2026, PRIME ePrint 2024/1771, WBAN quantum aggregation (PMC13125524).

## Most damning objection
DDG (Sec 5) and Skellam already compute the bit-width from c, sigma, n, gamma, d using 2k*sigma_hat <= 2^B. Going from that to "frames" is just ceil(d*B/8 / 102). 2610.00695 already gives an explicit-delta overflow margin that includes DP noise. "Radio dominates" is a well-known IoT result. As stated, the candidate is a re-application of known formulas.

## What remains open (as far as searched)
- No hit studies SecAgg+DP bit-width on 802.15.4/6LoWPAN, BLE or LoRa with frame-level accounting. That covers header and fragmentation overhead, the MAC retry/ACK cost per frame, and the fact that wraparound failure costs a whole round, which makes a "retransmit vs. extra bit" trade-off. The candidate only survives if it is reframed as a device-level systems measurement, not a new formula.
- Whether radio energy actually dominates once the mask PRG expansion for N peers and the key agreement run on the EFR32MG24 is unmeasured. It could also fail: with SecAgg's pairwise masks, the CPU cost grows with N.

## KNOWN / CONJECTURE / ASSUMPTION
- KNOWN: the bit-width rule from c, sigma, n, gamma (DDG Sec 5; Skellam Alg 1). DDG reports DP-SGD reaching central-DP accuracy at about 16 bits or fewer per coordinate. 2610.00695 gives an explicit-delta margin of about (1/2) log K. Bonawitz 1912.00131 gives worst-case headroom of log2 n bits.
- CONJECTURE (ours): with the frame/MAC overhead and failure-retransmit trade-off included, the energy-optimal k on 802.15.4 differs from the accuracy-optimal k of DDG/Skellam. Radio energy dominates the per-round crypto cost on the EFR32MG24.
- ASSUMPTION: about 102 B usable payload per frame (ignores 6LoWPAN/security headers, which reduce it further). 16,608 x 4 B = 66.4 kB, about 652 frames at 32 bits and about 326 at 16 bits (arithmetic, not measured).
