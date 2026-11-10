# Collision check R4-C6: streaming secure-aggregation masking that fits in sensor RAM

Date: 2026-11-11. Checker: collision-check subagent.

## Candidate
"Secure-aggregation masking that fits in a sensor's RAM: stream the masked update frame by frame."
Setting: smart meter on EFR32MG24 (Cortex-M33, 256 kB RAM, 802.15.4, about 102 B payload per frame).
D2 DP-SGD client already uses about 199 kB static RAM (16,608 float32 params; weights + grad accumulator
+ per-example grad buffer; arm-none-eabi-size). Proposal: per frame, generate each peer's pairwise-mask
bytes with a seekable PRG (ChaCha20 / AES-CTR, block counter = frame offset), add to the quantised chunk,
transmit, discard. RAM is O(k x PRG state) instead of O(d). Claim: non-seekable XOFs (SHAKE, Ascon-XOF)
cannot do this, so RAM forces the primitive choice. Retransmitting a frame means regenerating its masks.

## Threat model (as given)
- Application: FL load or theft model on smart meters.
- Asset: one meter's model update, which leaks its consumption readings.
- Adversary: honest-but-curious aggregator (the standard SecAgg setting), possibly colluding with some meters.
- Measured: peak RAM, cycles per frame, retransmission cost.

## Queries run (all through WebSearch)
1. "secure aggregation memory footprint microcontroller federated learning" (standard)
2. "streaming secure aggregation masking low memory chunked masked update" (standard)
3. "MicroSecAgg secure aggregation PoPETs 2024 memory" (standard)
4. "eprint.iacr.org secure aggregation lightweight IoT smart meter pairwise masks PRG" (standard, ePrint-focused)
5. "federated learning microcontroller secure aggregation RAM constrained Cortex-M implementation" (embedded)
6. "secure aggregation 802.15.4 fragmentation masked model update IoT LoRa 2025" (networking, 2025)
7. "seekable PRG counter mode mask generation secure aggregation on-the-fly chunk memory" (technical)
8. "TinyML federated learning secure aggregation microcontroller 2025 2026 memory overhead masking" (ML/TinyML, 2025-2026)
9. "secure aggregation implementation embedded device ARM Cortex-M pairwise masking ChaCha20 RAM kilobytes" (extended)
10. "MicroSecAgg arxiv OR eprint Guo Polychroniadou Shi Byrd Balch" (standard)
11. "smart meter federated learning secure aggregation microcontroller memory-efficient mask generation 2026" (2026)
12. "eprint.iacr.org 2025 secure aggregation resource-constrained clients low memory lightweight masking" (ePrint, 2025)

## Papers opened
| ID | Title | Verified | Depth | Same claim / device / threat model? |
|---|---|---|---|---|
| ePrint 2022/714; PoPETs 2024-0077 | MicroSecAgg (Guo, Polychroniadou, Shi, Byrd, Balch) | Exists (ePrint page and PoPETs page) | ePrint abstract only. PoPETs PDF downloaded, but no local PDF renderer was available, so the full text was NOT read | Same threat model. No MCU, no client-RAM measurements, no streaming in the abstract. "Micro" refers to round complexity, not microcontrollers |
| arXiv 2511.23252 | Hyb-Agg, one-shot secure aggregation for IoT (Emmaka, Phuong, Nov 2025) | Exists | Abstract | Runs on a Raspberry Pi 4, not an MCU. No RAM figures, no streaming |
| arXiv 2602.16181 | FL energy-theft detection for resource-constrained smart meters (Labate, Thakur, Fortino, Feb 2026) | Exists | Abstract | Same application. Privacy comes from DP noise only; no SecAgg, no MCU RAM figures |
| arXiv 2404.03320 | FL load forecasting on a smart-meter-like MCU (Duttagupta, Zhao, Shreejith, 2024) | Exists | Abstract | Arduino Uno MCU. No SecAgg or masking |
| preprints.org 202605.0029 | ESP32 secure model-update exchange (2026 preprint) | Shown in search results; page returned HTTP 403 | Search snippet only | MCU (ESP32); seems to be cipher selection for update encryption, not pairwise-mask SecAgg. Not verified |

Seen in snippets only, not opened: ACCESS-FL arXiv 2409.01722 (AES-CTR as the mask PRG), PREAMBLE arXiv 2503.11897
(cost of counter-mode PRG versus re-seeding), LightSecAgg arXiv 2109.14236 (chunked masks, server-class devices),
Lighthouse (USENIX Security 2026, resource-constrained clients), arXiv 2502.08989 (key-homomorphic masking, no per-element PRF),
TinyFL (Fraunhofer, MCU FL without SecAgg).

## Verdict
No published collision found for SecAgg on a Cortex-M-class meter with per-frame streamed masks (as far as searched).
But the novelty of the claim mostly fails on technical grounds; there is no paper to cite for that:

1. Most damning objection: generating a counter-mode keystream in chunks is textbook. ChaCha20 and AES-CTR are
   seekable by design, and the SecAgg papers above already use AES-CTR or ChaCha20 as the mask PRG. Generating the
   mask chunk by chunk is just an implementation detail. A reviewer will call it an engineering note.
2. The RAM premise is weak. The masked vector does not need a new 66 kB buffer. The update can be quantised to
   int32 in place in the existing float32 accumulator (same width) and masked in place, or quantised and masked
   frame by frame on the fly. Either way the extra RAM is about one frame. That is true for any PRG.
3. The claim that RAM forces the primitive is false. A non-seekable XOF (SHAKE, Ascon-XOF) also streams in order
   with O(state) RAM when frames go out in order. Only random-access retransmission costs extra. Deriving a
   per-frame seed as KDF(pair_seed, frame_idx), or using Ascon-PRF or KMAC with the frame index as input, makes any
   primitive seekable at the cost of one extra call per frame. So RAM does not pick the primitive.

What may remain open (CONJECTURE): a measured, end-to-end SecAgg client on an MCU-class meter, including RAM,
cycles and energy per 802.15.4 frame, and the cost of retransmission and of a dropped peer. This would be a
measurement or systems contribution, not a crypto one, and it is thin unless it is joined to a real question
such as dropout recovery or side-channel leakage of the mask PRG (DRP phase 2).

## KNOWN / CONJECTURE / ASSUMPTION
- KNOWN: ChaCha20 and AES-CTR allow random access through the block counter (standard). SecAgg papers use AES-CTR
  or ChaCha20 to expand pairwise seeds (ACCESS-FL snippet; arXiv 2409.00974 snippet). MicroSecAgg's abstract
  reports no MCU or client-RAM evaluation.
- CONJECTURE: no published MCU SecAgg client reports per-frame RAM or cycle costs. The search may have missed
  venue-specific work (Sensys, EWSN, IoTDI). The MicroSecAgg full text was not read.
- ASSUMPTION: the 199 kB figure includes a 66 kB accumulator that can be reused after local training (needs checking
  against the D2 code). There are k pairwise peers, each with one 64 B ChaCha state (or about 200 B for AES-CTR
  with round keys).
