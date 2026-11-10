# A4 kill experiment: PQ-signed windows of CAN FD frames vs a 10 ms deadline (2026-11-10)

**Verdict: the claim as worded is dead.** No window size N meets the 10 ms authentication deadline, for any scheme,
clock, bit rate, background load or frame period in the model: **0 of 96 configurations**. Batching does not help,
because signing one signature already takes longer than 10 ms. Labels: **KNOWN** (cited), **CONJECTURE** (ours),
**ASSUMPTION** (a modelling choice). This is a spreadsheet model. No crypto code was run.

## Inputs

| Input | Value | Source |
|---|---|---|
| ML-DSA-44 (m4f), Cortex-M4 | sign 3.94 M cycles on average (17.0 M worst seen in 1,000 runs); verify 1.42 M | KNOWN: pqm4 `benchmarks.md`, commit `0dd10ee` |
| FN-DSA-512 (m4f), Cortex-M4 | sign 22.5 M; verify 0.40 M | KNOWN: pqm4, same commit (`fndsa_provisional-512`) |
| FN-DSA-512 low-RAM | sign 13.45 M on average (verify taken as the m4f value) | KNOWN, **ABSTRACT ONLY**: ePrint 2026/1915 |
| SLH-DSA-SHA2-128f (clean) | sign 369 M; verify 21.9 M | KNOWN: pqm4, same commit |
| Signature sizes | 2,420 B / 666 B / 17,088 B | KNOWN: FIPS 204; Falcon spec (padded format); FIPS 205 |
| ECU clock | 80 MHz and 240 MHz. Cortex-M4 cycle counts are used unchanged | ASSUMPTION |
| CAN FD | 500 kbit/s arbitration phase, 2 or 5 Mbit/s data phase. One 64-B frame takes 407 or 204 µs with worst-case bit stuffing | ASSUMPTION: ISO 11898-1 field widths |
| Background bus load | 40% and 80% | KNOWN: range used by app16083705 |
| Frame period / deadline | 1, 10, 100 ms / 10 ms (the claim), plus 50 and 100 ms | KNOWN: A4 card / ASSUMPTION |

**Model**, chosen to favour A4: the receiver keeps all N frames and recomputes the hash, so frames carry no Merkle
paths, and hashing cost is ignored. Worst-case latency of the first frame in a window is:

> L(N) = (N − 1)·P + t_sign + (signature frames × frame time) / (1 − background load) + t_verify

The background-load term is a crude queueing inflation (ASSUMPTION). The signer must also keep up: t_sign ≤ N·P.
The script is `experiments/a4-canfd-pq-auth/model.py`. All 288 rows are in `model.csv` and the tables in `tables.md`.

## Results

**Fastest possible authentication (N = 1, best case: 5 Mbit/s, 40% background load)**

| Scheme | 80 MHz: sign / verify / on bus / total (ms) | 240 MHz: total (ms) | Meets 10 ms? |
|---|---|---|---|
| ML-DSA-44 | 49.3 / 17.8 / 12.9 / **80.0** | **35.3** (70.9 ms signing in the worst run seen) | no |
| FN-DSA-512 | 280.9 / 5.0 / 3.7 / **289.6** | **99.0** | no |
| FN-DSA-512 low-RAM* | 168.1 / 5.0 / 3.7 / **176.8** | **61.4** | no |
| SLH-DSA-SHA2-128f | 4,607 / 274 / 91 / **4,972** | **1,718** | no |
| **CAIBA** (symmetric, EuroS&P 2025) | "no communication overhead or verification delays" (**ABSTRACT ONLY**) | same | yes, per its abstract. But it needs a trusted authenticator node on the bus, gives no non-repudiation, and is not a signature |

**Where batching does work (relaxed deadlines, 240 MHz, 5 Mbit/s, 40% background load)**

| Scheme | Frame period 1 ms | 10 ms | 100 ms |
|---|---|---|---|
| ML-DSA-44, 50 ms deadline | none | N* = 2 (+38.7% bus load) | N* = 1 (+7.7%) |
| ML-DSA-44, 100 ms deadline | **N* = 65 (+11.9%)** | N* = 7 (+11.1%) | N* = 1 (+7.7%) |
| FN-DSA-512 (either), 100 ms deadline | none | none | N* = 1 (+2.2%) |
| Any scheme at 80 MHz | none, except ML-DSA-44 at 100 ms period / 100 ms deadline (N* = 1) | | |

## What this means, in plain words

A signature has to be computed before anyone can check it. On an ECU-class chip, computing one post-quantum signature
takes 16 ms at the very least (ML-DSA-44 at 240 MHz, on average), and Falcon takes 56 ms or more. So no batching trick can
meet a 10 ms deadline. Batching shares the signature's *bus cost* across frames, but it can only *add* waiting time.
Batching does become useful for slower signals that can wait about 100 ms. There, ML-DSA-44 beats Falcon, because its
faster signing matters more than Falcon's smaller signature once that signature is shared. That matches the card's
ranking-flip guess (CONJECTURE now supported by the model, not by measurement). Meanwhile CAIBA already gives source
authentication against a compromised ECU with no delay at all, according to its abstract. So the safety-critical,
low-latency case that motivated A4 is better served by CAIBA, and A4 is left with slower, non-safety signals.

## What survives

- **For slow signals only:** PQ source authentication of periodic CAN FD signals with deadlines of about 100 ms (body and
  comfort domain, logging). Here ML-DSA-44 windows of up to 65 frames work at 240 MHz with about 12% extra bus load,
  and FN-DSA does not fit. The selling points that remain over CAIBA are non-repudiation for forensic logs and no
  trusted node. Neither is a latency argument.
- **Caveats that could change numbers, but not the 10 ms verdict:**
  - M33/M7 cycles were not calibrated against the M4 numbers.
  - A real CAN schedule analysis would replace the 1/(1−load) inflation.
  - ML-DSA's worst-case signing (17 M cycles) breaks the 100 ms rows at 80 MHz.
  - The FN-DSA low-RAM row is from an abstract.

## Effect on the A4 score

Falsifiability did its job: the stated claim is false. Changes: novelty 4 → 3 (only a relaxed-deadline niche remains),
threat model 4 → 3 (the safety-critical motivation now goes to CAIBA), feasibility 4, DRP 3, group split 5,
falsifiability 5. **Total 23** (was 25).
