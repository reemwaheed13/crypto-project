# On-Device Training of Fully Quantized DNNs on Cortex-M Microcontrollers

## Verified identity
- **arXiv ID:** 2407.10734 (confirmed; v1 HTML read).
- **Exact title:** "On-Device Training of Fully Quantized Deep Neural Networks on Cortex-M Microcontrollers" (confirmed, matches candidate).
- **Authors:** Mark Deutel, Frank Hannig, Christopher Mutschler, Jürgen Teich (FAU Erlangen-Nürnberg / Fraunhofer IIS).
- **Year:** 2024 (submitted 15 Jul 2024; revised 28 Aug 2024).
- **Venue:** Published in IEEE Transactions on Computer-Aided Design of Integrated Circuits and Systems (TCAD), DOI 10.1109/TCAD.2024.3484354. So this is PUBLISHED, not ePrint-only. (Note: some sources summarize it as "IEEE Trans. Computers"; the DOI resolves to TCAD — treat TCAD as the verified venue.)

## Core claim
On-device DNN *training* (not just inference) is feasible on real Cortex-M MCUs by doing fully quantized training (FQT) entirely in 8-bit integer space in place, combined with dynamic partial/sparse gradient updates that only update a fraction of weights per layer based on gradient magnitude. The authors demonstrate both transfer learning (retrain final layers) and full training across vision and time-series datasets on three different-sized Cortex-M boards, and characterize the accuracy/memory/energy/latency tradeoff on hardware.

## Platform & what was ACTUALLY measured
KNOWN (from Table II and text, as stated):
- **Three real MCUs, no Cortex-M33:**
  - **NXP i.MX RT1062 ("IMXRT2062" in extraction)** — Cortex-M7, 600 MHz, 2×512 KB RAM, 16 MB external flash, 108.26 mA idle.
  - **Nordic nRF52840** — Cortex-M4, 64 MHz, 256 KB RAM, 1 MB internal flash, 7.27 mA idle.
  - **Raspberry Pi RP2040** — Cortex-M0+, 133 MHz, 264 KB RAM, 16 MB external flash, 31.24 mA idle.
  - All on a SparkFun MicroMod carrier.
- **Real hardware, not emulator** for the headline results: "All results are averaged over 1000 consecutive training steps" measured on the physical boards; energy derived from measured current draw. CAVEAT: when a model did not fit in MCU RAM, they *simulated* that retraining on a desktop "using the same code and framework as used on the MCU" — so some full-training accuracy points are PC-side, hardware cost points are on-device.
- **Models/datasets:** MbedNet (scaled MobileNetV3-style) and MCUNet-5FPS for comparison. Vision: CIFAR-10, CIFAR-100, Flowers-102, Animals-10, FMNIST, KMNIST, EMNIST-Letters, EMNIST-Digits. Time-series: CWRU (9 cls, 1×512), DaLiAc (13 cls, 1×1024), Speech Commands (36 cls, 1×16000).
- **Per-sample cost (approximate, read off figures — NOT exact table values):**
  - Transfer learning on the M7: ~100–400 ms/sample (Fig. 4b), forward pass dominates.
  - Full training on the M7: ~600–800 ms/sample (Fig. 7b); on the M4/M0+ ~3000+ ms/sample.
  - Energy on the M7: ~50–70 mJ/sample excluding idle (Fig. 5b); M4 least efficient per sample but lowest idle current.
  - (These are my reads of plotted ranges, labelled approximate. The exact per-step numbers live in Figs 4,5,7 — pull from the PDF if we need precise values.)
- **Footprint (read off Fig. 4c/4d):** transfer-learning RAM ~50–150 KB; static weights in flash ~100–400 KB; the nRF52840 (256 KB RAM) could only run ~2 of the datasets.
- **Accuracy vs full-precision:** transfer learning with uint8 FQT ≈ float baseline on easy tasks (CWRU, CIFAR-10), slight drop on hard tasks (Flowers, CIFAR-100); a "mixed" config (float32 classification head + quantized backbone) matched full float. Full training from scratch showed a larger ~2–5% gap below float32 on MNIST variants. On the MCUNet-5FPS comparison (Table IV) their method reached 73.7% avg vs 73.5% for SGD+M+QAS over 8 datasets.

## Threat model
N/A — this is a systems/efficiency paper, not a security paper. There is no adversary, no secrecy/leakage claim. **Its role for us is the ENABLER, not the attack:** it is evidence that genuine training (hence real local gradients) can run on a Cortex-M device, which is the prerequisite realism anchor for any federated-learning / gradient-leakage privacy study on constrained MCUs.

## Convenient assumptions
- **Quantization:** 8-bit uint8, per-tensor linear quantization (vq = ⌊vf/s⌋ + z). Also a "mixed" mode keeping the classification head in float32. Per-channel/alternative schemes explicitly left to future work.
- **Which layers trained:** transfer-learning experiments retrain only the final ~5 layers (20 epochs); a separate "full training" mode trains all layers (mostly on smaller MNIST-scale nets, often needing PC simulation when RAM-bound).
- **Batch size 48**, learning rate 0.001. Gradients standardized per channel before the update step to stabilize FQT convergence.
- No on-device data augmentation for some datasets (they note this increases overfitting).

## Stated limitations & future work
Accuracy is bounded by 8-bit resolution, especially for early layers and subtle features; full-from-scratch training has a wider gap to float than transfer learning does. One quote on the proposed fix:
> "A more fine-grained quantization scheme, such as per-channel quantization, or alternative quantization strategies, such as weighted entropy or stochastic quantization ... could be useful approaches to bridge this gap as long as they can maintain a reasonable computational overhead."

## Load-bearing references (3–5)
- **MCUNet** (Lin et al.) — tiny-DNN inference under ~256 KB; the MCU resource-constraint baseline and the MCUNet-5FPS model they compare against.
- **TinyTL** (Cai et al.) — memory-efficient transfer learning (bias/lite-residual updates), the "retrain few layers" line this paper extends into full quantized training.
- **POET** (Patil et al.) — on-device training via rematerialization + paging; alternative memory-reduction strategy.
- **SGD+M+QAS / quantization-aware scaling** (the MCU on-device training-with-quantization line) — direct accuracy comparison point (Table IV).
- **QAT** (quantization-aware training) — the prior *simulated* float-based quantized training they contrast with their truly-integer in-place approach.

## Relevance to our project
KNOWN from this paper: real backprop and weight updates run on an actual Cortex-M4 (nRF52840) and M0+/M7 with 8-bit arithmetic, within ~50–150 KB RAM for transfer learning. That directly supports the realism of a federated-learning-on-MCU threat model: if a device can compute a local gradient/update at all, that update exists and can leak information — so a gradient-leakage or secure-aggregation/DP study on Cortex-M is not science fiction.

Can we reuse a per-step cost to budget a DP/defence overhead? PARTIALLY, and only as an order-of-magnitude anchor. CONJECTURE/ASSUMPTION: the per-sample latency (~hundreds of ms to seconds) and energy (tens of mJ on the M7) give a baseline we could compare against the added cost of a defence (e.g., per-update Gaussian noise + clipping for DP-SGD, or masking for secure aggregation). But the paper does NOT isolate per-layer gradient-computation cost in a form that cleanly maps to "cost of adding noise to a gradient vector," and the cheapest-to-measure numbers are mostly on the M7, not the M4 that matches the course's EFR32xG24 (Cortex-M33) target. So we should treat its figures as a calibration *reference* (feasibility + ballpark energy/step) and re-measure the defence delta ourselves. HONEST CAVEAT: no M33 here, no radio/communication cost, and the reported headline reductions I saw in a third-party summary (34.8% memory / 49.0% latency / 8.7× speedup) could NOT be confirmed in the v1 abstract/conclusion I read — the directly verifiable speedup figure was "an average speedup of about 6.64 across all datasets." Do not cite the 34.8/49.0/8.7 numbers without checking the final TCAD PDF.

## Possible seeds (direction-miner moves)
- **Port-the-platform:** this is Cortex-M0+/M4/M7; our target is Cortex-M33 (EFR32xG24). Re-estimate on-device training/update cost on M33 (DSP/TrustZone) — fills an unmeasured platform gap.
- **Add-the-adversary:** take their realistic on-device update and ask what it leaks (gradient-inversion on an 8-bit quantized update) — turn the enabler into a privacy study.
- **Cost-the-defence:** use their per-sample energy/latency as the baseline and measure the delta added by DP-SGD (clip+noise) or secure-aggregation masking on an MCU.
- **Quantization-meets-privacy:** does 8-bit FQT itself blunt gradient-leakage (coarser gradients) — a free partial defence, or does it help the attacker? Open as far as searched.

## Reading depth
Skimmed-to-full on the arXiv v1 HTML: abstract, platform/table II, FQT scheme, results text, conclusion read directly via fetch; exact figure values (Figs 4/5/7) were read as plotted ranges, not transcribed from tables. Final TCAD PDF not opened — recommend pulling precise per-step numbers from it before any experiment uses them.

## Full-text check (2026-11-10, local PDF `2407.10734v2.pdf`, v2, 12 pages)
Reading depth is now **full text (v2)**. Corrections and additions:
- **Abstract numbers confirmed in v2.** Versus related work: 34.8% less memory, 49.0% lower latency per training sample, and up to 8.7× speedup from dynamic partial updates (abstract, p. 1). The "could not be confirmed" caveat above applied to v1 only.
- **Time-series anchors (Fig. 5, p. 8; transfer learning; mean of 1000 steps; idle draw excluded):**
  - CWRU (1×512 series, Table I), uint8: M0+ 166.65 ms / 0.59 mJ; **M4 (nRF52840, 64 MHz) 75.79 ms / 2.18 mJ**; M7 3.57 ms / 0.08 mJ per sample.
  - Daliac (1×1024), uint8: M4 379.91 ms / 11.12 mJ per sample.
  - The "~50–70 mJ/sample on M7" above does **not** match Fig. 5, where the M7 is below 3 mJ for both time-series sets. Treat that figure as unverified (possibly a vision dataset from Fig. 4) and do not cite it.
- Table II (p. 7): RP2040 M0+ 133 MHz; nRF52840 M4 64 MHz, 256 KB RAM; i.MX RT1062 M7 600 MHz. Still **no Cortex-M33**, and no privacy, FL or DP experiment. Privacy appears only as motivation (p. 1).
- For D2, the CWRU/M4 row is the per-step baseline that a defence's added cost should be compared against (ASSUMPTION: M33 at 78 MHz is within a small factor of M4 at 64 MHz).
