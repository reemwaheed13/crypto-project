# D2 crypto add-on: secure aggregation costed on top of DP, and the search for one applied-crypto contribution (2026-11-11)

## Bottom line (plain words)

1. **Secure aggregation (SecAgg) on top of DP-SGD is affordable for the sensor at realistic group sizes.**
   - Per round, with SecAgg+ (each sensor masks against about 2 log₂N neighbours), masking and key agreement cost
     about 0.4–0.7× the DP-SGD round for N = 10–100 clients.
   - The radio upload (43 mJ at 32-bit encoding) still dominates.
   - Only full pairwise SecAgg at N ≈ 100 approaches the radio cost.
   - Collision verdict on this costing: **partial overlap.** Nobody has measured a masking SecAgg client on a
     microcontroller. The closest is a Raspberry Pi 4 (Hyb-Agg, arXiv 2511.23252), without DP.
2. **No candidate for a super-niche applied-crypto contribution survived as "Open".** Seven were checked. Five are
   partial overlaps in the literature (C1–C5). Two more, C6 and C7, fail on substance once checked: C6 found nothing in the
   literature but falls to a reviewer objection we confirmed; C7 is a near-collision that also fails on our own measurement. Following the rule "drop anything that
   is not Open", **no new crypto contribution is added to D2.**
   - What D2 keeps instead is a cost package with two measured lines that no checked paper reports:
     - the per-phase SecAgg+DP client cost on the M33;
     - the cost of switching from float32 Box-Muller to the constant-time integer noise that SecAgg requires. With a
       naive full-scan CDT this is **68–208 M instructions per step, 12–37× the DP step** (MEASURED). σ-independent
       constant-time samplers (FACCT, Micciancio–Walter convolution) should cost less. They were not measured, so this
       is an upper bound.
   - In this report "Open" always means *open as far as searched*.

Labels: **MEASURED** (QEMU instruction counts, uncalibrated), **KNOWN** (cited, with reading depth), **ESTIMATE**
(arithmetic on measured and known inputs), **ASSUMPTION**.

## 1. SecAgg + DP on an MCU: collision check (`collision-r4-secagg-dp-mcu-cost.md`, 17 queries)

**Already published (KNOWN):**
- MicroSecAgg (PoPETs 2024) was evaluated on an AWS server.
- LightSecAgg (MLSys 2022) was evaluated on AWS m3.medium instances, with N ≤ 200.
- Flamingo (S&P 2023) was simulated on a Xeon.
- None of the three measures a microcontroller client, and none combines masking with DP.
- Hyb-Agg (arXiv 2511.23252, full HTML): Raspberry Pi 4 (Cortex-A72 Linux): 431.8 ms and 787 kB uplink per client per
  round at N = 50 with 8,192 parameters. It has no DP.
- An ESP32 study (MDPI Future Internet 18(6):306, snippets only) encrypts the channel only, not secure aggregation.
- **Cost formulas** (LightSecAgg Table 1, read): Bonawitz per user offline compute O(dN + sN²) and online
  communication O(d + sN); SecAgg+ replaces N by log N.
- **X25519 on the Cortex-M33 (DSP):** 411,061 cycles (Lenngren, ePrint 2025/523, **abstract only**). On the M4 it
  is 441,116.

**Still open (as far as searched):** the measured per-round client cost of SecAgg plus DP-SGD on a Cortex-M (cycles,
RAM, 802.15.4 bytes), and how it scales at small N.

**Risk:** "arithmetic on known formulas", unless the measurement shows a non-obvious crossover.
- Unverified: IEEE Xplore document 11442145 ("low-cost secure aggregation for IoT") could not be opened. Read it
  before relying on this verdict.

## 2. SecAgg cost estimate from our measured numbers (`experiments/d2-sensor-gradinv/secagg/secagg_cost.py`)

Inputs:

| Input | Value | Label |
|---|---|---|
| ChaCha20 | 1,226 instructions per 64-B block | MEASURED |
| DP-SGD step | 5.63 M instructions; the round is 5 steps = 28.1 M = 2.8–8.3 mJ | MEASURED; energy via ASSUMPTION CPI 1–2 and KNOWN datasheet nJ/cycle |
| Masking | Bonawitz double masking: N−1 pairwise masks plus 1 self mask per round, each a PRG expansion over the whole quantised update | KNOWN protocol |
| Key agreement | 2 X25519 key generations + 2 shared secrets per peer | KNOWN protocol |
| X25519 | 411,061 cycles | KNOWN, abstract |
| SecAgg+ degree | k = 2 log₂N | ASSUMPTION; Bell et al. give k = O(log N) |
| Radio | 0.50 µJ per on-air byte at 0 dBm; 102 B payload + 31 B overhead per frame | KNOWN-derived / ASSUMPTION |

**32-bit encoding** (66,432 B, 652 frames, **43.3 mJ** upload):

| N | Scheme | Masks | Mask compute | Mask energy (mJ) | × DP round | X25519 ops | X25519 energy (mJ) |
|---|---|---|---|---|---|---|---|
| 10 | Full pairwise | 10 | 13.2 M | 1.3–3.9 | 0.47 | 20 | 0.8–1.2 |
| 10 | SecAgg+ | 8 | 10.6 M | 1.1–3.1 | 0.38 | 16 | 0.7–1.0 |
| 40 | Full pairwise | 40 | 52.9 M | 5.3–15.6 | 1.88 | 80 | 3.3–4.8 |
| 40 | SecAgg+ | 12 | 15.9 M | 1.6–4.7 | 0.56 | 24 | 1.0–1.5 |
| 100 | Full pairwise | 100 | 132 M | 13.2–38.9 | 4.70 | 200 | 8.2–12.1 |
| 100 | SecAgg+ | 15 | 19.8 M | 2.0–5.8 | 0.70 | 30 | 1.2–1.8 |

With **16-bit encoding** (33,216 B, 326 frames) the upload is 21.7 mJ and every mask cost halves. The full table is in
`experiments/d2-sensor-gradinv/results/2026-11-10/secagg_cost.txt`.

**Plain words.**
- **SecAgg+ is cheap.** Its compute adds less than one DP round at every N we tried. Key agreement is small next
  to masking.
- **Full pairwise SecAgg is not.** Its cost grows linearly with N and at N = 100 rivals the radio.
- **The radio stays the biggest line in every case,** so shrinking the encoding (16 bits instead of 32) saves more
  energy than any crypto optimisation. That is DDG's known bit-width rule, not a new result (see C2).

## 3. Applied-crypto candidates for D2

Each candidate got one collision checker running at least 4 distinct searches (technical, plain-language, ePrint,
embedded/networking). Logs are `research/collision-r4-C*.md`, and each lists every query and reading depth.

| # | Candidate | Searches | Closest papers and depth | Verdict | Killer / most damning objection |
|---|---|---|---|---|---|
| C1 | Mask-expansion PRG choice on the EFR32MG24: SW ChaCha20 vs SE hardware AES-CTR vs Keccak/Ascon | 17 | CHRONOS arXiv 2604.19053 (full HTML except the last part: HW AES-CTR masks on Cortex-A); MicroSecAgg (landing page); gimli perf table (snippet) | **Partial overlap** | Radio dominates: one mask is about 31 ms on the SE, or about 17–34 ms in software ChaCha (ESTIMATE), against at least 2.1 s of airtime for the upload. "MCU advice differs from desktop" is folk knowledge. The SE (474 µs/kB, KNOWN datasheet) is not faster than software ChaCha |
| C2 | Choose the modulus 2^k jointly with the DP noise to cut 802.15.4 frames | 8 | DDG arXiv 2102.06387 (full); Skellam arXiv 2110.04995 (nearly full); arXiv 2610.00695 (most) | **Partial overlap, close to a collision** | DDG/Skellam already give the bit-width from clip, σ, N and the quantisation step, with explicit overflow margins. Frames are just ⌈bytes/102⌉ |
| C3 | Constant-time Shamir dropout recovery on the M33 | 14 | CVE-2023-25000 (table-based GF(256) leak); SLIP-39 constant-time Shamir in production; CARDIS 2012, TCHES 2018 | **Partial overlap** | In Bonawitz/SecAgg+ the *server* reconstructs, and the sensor only shares once. Constant-time GF(256) Shamir already ships, and the cost is milliseconds |
| C4 | Per-round mask seeds from one long-term pairwise key via a KDF | 10 | Flamingo arXiv 2308.09883 (about half, including protocol and dropouts); CHRONOS (summary); LERNA ePrint 2023/1936 (abstract) | **Partial overlap** | Flamingo already does PRF(long-term key, round) and shows that naive reuse inside Bonawitz breaks under dropouts. The secure version needs Flamingo's decryptor committee |
| C5 | One hardware-AES stream for both masks and constant-time integer DP noise | 9 | CHRONOS (abstract); arXiv 2604.07125 (abstract); ePrint 2026/2287 (abstract) | **Partial overlap**; main claim fails on our numbers | The sampler cost is the table scan, and randomness is only 5–10% at DP-scale σ. The accelerator is no faster than ChaCha. Drawing noise from the mask stream would let the server regenerate and remove it |
| C6 | Stream the masking frame by frame to fit 256 kB RAM (seekable PRG) | 12 | MicroSecAgg (abstract); Hyb-Agg (HTML); smart-meter FL arXiv 2602.16181, 2404.03320 | **Open in the literature, but fails on substance** | "Chunked counter-mode keystream is textbook." The RAM premise is wrong: you can quantise to int32 *in place* in the float32 accumulator and mask in place, with no extra 66 kB buffer and any PRG. A per-frame KDF makes SHAKE/Ascon seekable too |
| C7 | Constant-time integer noise becomes affordable under SecAgg, because each client needs only σ/√N | 10 | DDG arXiv 2102.06387 (full: σ/√n per client, dropout degradation §5.3); Dordis arXiv 2209.12528 (snippets: dropout noise deficit fixed); FACCT ePrint 2018/1234 and Micciancio–Walter ePrint 2017/259 (abstracts: constant-time cost independent of σ) | **Partial overlap, close to a collision** | σ/√N and dropout handling are in DDG and Dordis. Cost growing with σ is an artefact of the naive full-scan CDT, which σ-independent constant-time samplers replace. With a proper sampler there is no N-dependence to report. It also **fails on our measurement** (§4): no crossover with the float sampler exists for the full-scan CDT |

**Survivors under "drop anything not Open": none.** C6 is the only literature-Open candidate (C7 turned out to be a partial overlap), and its own checker,
plus the in-place argument we verified against D2's buffers, removes its technical premise. Scoring survivors with the
pessimistic rubric therefore produces no entries. For the record, the "Open" sliver every checker converged on is a
*measured* SecAgg+DP client cost breakdown on an MCU, i.e. §2 done on hardware. That is the work package itself, not a
super-niche contribution.

## 4. Preliminary measurements (QEMU M33, about 30 minutes in total)

**C6 streaming masking** (`experiments/d2-sensor-gradinv/secagg/stream_mask.c`, `build.sh`). All three variants
produce bit-identical masked output (MEASURED):

| K peers (+1 self) | Contiguous mask over the whole vector | Stream, 64-B leftover per peer | Stream, stateless seek per frame |
|---|---|---|---|
| 8 | 12.6 M instructions | 14.1 M (+12%) | 21.7 M (+72%: partial ChaCha blocks at 100-B frame boundaries) |
| 15 | 22.3 M (the cost model estimated 21.1 M) | 24.9 M (+12%) | 38.4 M (+72%) |

- **Static RAM of the masking module:** 134.6 kB with a separate 66 kB masked buffer, against 68.2 kB streaming at
  K = 15 (MEASURED, `arm-none-eabi-size`; both include the 66 kB update).
- **Context:** D2's DP-SGD step module is already 202.5 kB (MEASURED). So a *separate* masked buffer would push the
  sensor past 256 kB.
- **But masking in place** in the accumulator after training needs no extra buffer either. This is why C6 was dropped.

**C7 constant-time integer noise under SecAgg** (`experiments/n1-mcu-dp-sampler/sampler_c7.c`). This uses a full-scan
CDT, constant-time, at the per-client σ_int = 461/√N (grid C·2⁻¹⁰, ε = 8, ASSUMPTION). Variances match their targets.

| N | Per-client σ_int | Instructions per sample (MEASURED, constant: 1 distinct count) | Per DP-SGD step, 16,608 samples + randomness (ESTIMATE) | × float32 Box-Muller noise (3.70 M/step) | × DP step (5.63 M) |
|---|---|---|---|---|---|
| 100 | 46 | 3,890 | 68 M | 18× | 12× |
| 40 | 73 | 6,158 | 106 M | 29× | 19× |
| 10 | 146 | 12,290 | 208 M | 56× | 37× |

The cost fits about **84 instructions per unit of σ_int**. Even at σ → 0 it exceeds Box-Muller's ~223 per sample,
including ChaCha. **No crossover exists for the full-scan CDT**, so C7's claim fails. The checker adds that
σ-independent constant-time samplers exist (FACCT ePrint 2018/1234, Micciancio–Walter ePrint 2017/259, abstracts
only). Under those, per-client cost is roughly flat in N, so the table above is an *upper bound* for a sound sampler,
not its cost. Measuring FACCT or a convolution sampler on the M33 is the obvious next step. CONJECTURE: it would still
sit above the float sampler, which is consistent with N1's 46–98 M per step convolution estimate.

## 5. What goes into D2

- **Secure aggregation enters D2 as a costed work package.**
  - Threat model: honest-but-curious server, and SecAgg hides individual updates from the server.
  - DP still protects against what the aggregate reveals and against colluding or dropout-exploiting servers.
  - SecAgg is not compared against DP.
- **The new measured cost lines are:**
  - the masking/key-agreement table in §2;
  - the integer-noise switch in §4: up to 12–37× the DP step at N = 10–100 with a naive constant-time CDT. This is an
    upper bound; σ-independent samplers should be cheaper, but they are unmeasured. Even so, the switch is plausibly the
    largest crypto cost SecAgg imposes on the sensor, larger than the masking itself (CONJECTURE).
- **No standalone applied-crypto contribution is claimed** (none survived).
- **Phase 2 is unchanged:** power analysis of the PRNG and noise routine on the EFR32 kit. No TrustZone, no verifiable
  DP.
