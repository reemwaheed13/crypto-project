# On-Device Is Not Private: Measuring Membership Leakage in Lightweight Sensor Anomaly Detectors

## Citation
- Title: On-Device Is Not Private: Measuring Membership Leakage in Lightweight Sensor Anomaly Detectors
- Authors: Manpinder Singh Panesar, Aman Kataria, Sita Rani, Aditi Kataria, Vikram Puri
- Venue/status: published, MDPI *Machine Learning and Knowledge Extraction* (MAKE) 2026, 8(10), 320. Received 5 Aug 2026, accepted 15 Sep 2026, published 8 Oct 2026 (p. 1 front matter).
- ID: doi:10.3390/make8100320
- Reading depth: **full text (local PDF)**. Read all 22 pages from the extracted text (`make-08-00320-v2.pdf`); the tables (Tables 1-6) came through readable in the extraction.

## Core claim
A compact reconstruction-autoencoder anomaly detector for multivariate time series leaks training-set membership through its anomaly score (reconstruction error). Per-example clipping plus Gaussian gradient noise brings measured leakage close to chance for about 0.054 detection-AUC loss. INT8 ONNX quantization shrinks the model but does not improve privacy.

## Platform
- **All costs measured on a desktop PC only.** AMD Ryzen 5 5500GT, Ubuntu 24.04.4, ONNX Runtime 1.27.0 CPUExecutionProvider, single thread (intra/inter_op_num_threads=1), PyTorch 2.5.1 CPU, ONNX 1.22.0, Python 3.12.3. Latency is the median of 120 batch-1 inferences after 30 warm-ups (Sec. 4.2, p. 11).
- No MCU, emulator, Raspberry Pi or energy measurement. The authors say plainly that this "is not on-microcontroller execution" (Sec. 4.2; Sec. 3.6; Sec. 6.4 Cost (b)).
- "Resource-envelope screening" (Sec. 3.6) compares desktop numbers with three assumed device classes: MCU-tiny (64 kB flash, 20 kB SRAM, <=10 ms), MCU-mid (512 kB, 128 kB, <=5 ms) and MPU-edge (8 MB, 1 MB, <=2 ms). Flash is checked against the ONNX file size. SRAM is checked against the largest single activation tensor, found with forward hooks, which is only a lower bound on peak memory. Latency comes from the desktop and the authors call it "indicative only".
- No compiler flags (Python/ONNX stack). Quantization is ONNX dynamic INT8. The graph is mixed precision: MatMulInteger, ConvInteger and DynamicQuantizeLSTM run in integer arithmetic, ConvTranspose stays in float (Sec. 3.5).
- Models: four autoencoder families (Dense, Conv, LSTM, TCN), hidden width 64, code dimension 16, window L=64. Parameter counts range from 16,000 to 308,000 (Table 2). Training: Adam at lr 1e-3, 200 epochs, batch 64, n=200 normal member windows (Sec. 4.2, Alg. 2).

## Threat model
- Adversary: black-box and score-only. It sees the published anomaly score (reconstruction error) of chosen windows, with no access to weights or gradients, and runs a loss-threshold membership-inference attack with s(x) = -e(x) (Sec. 3.3). Secondary check: a shadow-model likelihood-ratio attack with 4 shadows, run on a 2-dataset Conv subset (Sec. 5.2, Table 5).
- Protected asset: which training windows were used to train the detector (membership).
- Out of scope, as stated: white-box/gradient attacks and stronger LiRA-style attacks (Sec. 3.3, Sec. 6.4 Attack (b)); a formal DP guarantee (Sec. 3.4); on-MCU SRAM and energy (Sec. 6.4 Cost (b)). Federated learning, gradient inversion and data reconstruction attacks are not studied at all.

## Numbers that matter
All figures below are means over 6 datasets (Table 4) unless noted.
- Base FP32: utility AUC 0.878 ± 0.104; attack AUC 0.688 ± 0.040; TPR@1%FPR 0.074 ± 0.030; size 329 ± 91 KB; latency 0.090 ± 0.005 ms (Table 4).
- INT8: utility 0.875; attack AUC 0.721; TPR@1%FPR 0.110; size 102 ± 26 KB; latency 0.218 ± 0.011 ms (Table 4).
- Gradient noise (C=1, σ=1): utility 0.824 ± 0.122; attack AUC 0.508 ± 0.011; TPR@1%FPR 0.016 ± 0.003; size and latency the same as base (Table 4).
- INT8 + gradient noise: utility 0.824; attack AUC 0.513; TPR 0.017; 102 KB; 0.217 ms (Table 4).
- Size of the defence effect: attack-AUC drop 0.181, 95% CI [0.145, 0.210], Cohen's d = 4.06. TPR drop 0.059, CI [0.039, 0.082], d = 1.96. Wilcoxon p = 0.0156 for both. Utility cost 0.054, CI [0.025, 0.083], p = 0.031 unadjusted, which is not significant after Bonferroni correction (Sec. 5, p. 12). Oriented away from chance, the defended attack AUC is 0.517 (p. 12).
- Across all 84 cells: base attack AUC ranges 0.49-0.93, defended ranges 0.47-0.56 (Fig. 5, p. 12). Worst case: CreditCard, TPR@1%FPR 0.13, and one Dense cell reaches AUC 0.93 (p. 12).
- Per architecture: base attack AUC 0.61-0.77. Utility retained under the defence: LSTM 96%, Conv 94%, Dense 94%, TCN 91%. INT8 sizes: Dense ~196 KB, Conv ~65 KB, LSTM ~71 KB, TCN ~77 KB (Fig. 6, p. 13-14).
- INT8 compression is 3.2x on average (Dense 3.8x, LSTM 3.4x, TCN 3.1x, Conv 1.5x). INT8 attack AUC rises by +0.033, CI [0.003, 0.082] (one-sided improvement test p = 0.95, worsening test p = 0.078). On Exathlon it rises from 0.62 to 0.77, and the jump depends on the seed (Sec. 5, p. 14; Fig. 8).
- Envelope screening: all 84 INT8 artifacts pass MCU-mid and MPU-edge. Only 12/84 pass MCU-tiny 64 kB, all of them Conv (57% of Conv cells) (Sec. 5.1, Fig. 9).
- σ sweep (CreditCard/PSM/Exathlon × Conv/Dense, seed 0): leakage reaches near chance by σ = 0.25-0.5 (CreditCard Dense 0.93 → 0.51; PSM Dense 0.73 → 0.49). Clip-only (σ=0) leaves leakage unchanged (CreditCard Dense 0.93 → 0.94). σ=2 cuts PSM Conv utility from 0.86 to 0.70 (Sec. 5.2, Fig. 10).
- Shadow attack agrees with the loss-threshold attack: CreditCard base 0.698 vs 0.689, defended 0.497 vs 0.498; PSM base 0.597 vs 0.598, defended 0.491 vs 0.504 (Table 5).

## Convenient assumptions
- Windows are treated as i.i.d. and assigned to member or non-member sets uniformly at random (Sec. 3.2). An exchangeability guard (|z| > 3) rejects splits that are distinguishable before training; all 21 splits passed (Sec. 4.2).
- Very small training set: n=200 windows per model (Sec. 4.2). This favours memorisation, so leakage may be higher than with realistic data volumes.
- Training happens offline and centrally on a PC. On-device training cost is never considered, even though the defence acts at training time. The paper calls the defence "low-cost" because inference is unchanged (Sec. 6.1).
- No radio or communication cost, no energy, and no real MCU memory measurement.
- Datasets are admitted only if a probe detector reaches AUC >= 0.65 (Sec. 4.1). The gate is partly selected on the seed-0 evaluation set (p. 10).
- The defence has no privacy accountant and uses shuffling instead of Poisson subsampling, so no (ε, δ) is reported (Sec. 3.4).

## Stated limitations and future work (paraphrased)
- Only the loss-threshold attack and a small 4-shadow attack were evaluated. Stronger shadow-model and white-box attacks could reveal leakage this evaluation misses (Sec. 6.4).
- The defence is not formal DP. Future work would replace it with an accountant-backed (ε, δ) mechanism (Sec. 7).
- Costs are desktop envelope screening. The authors want to confirm them with on-target MCU profiling of peak memory and energy (Sec. 7): "on-target profiling of true peak SRAM and energy is future work" (Sec. 3.6).
- Six datasets, all from a gated benchmark, so the results make no claim of universality (Sec. 6.4).

## Load-bearing references
- [4] Sun et al., EPC-TinyAD, *Electronics* 2026, 15, 2879. A compressed TinyML anomaly-detection framework that calls itself privacy-aware because of on-device inference or "noisy federated updates". This paper is a direct response to it. For D2 it may be the closer paper (FL plus TinyML); worth reading.
- [23] Abadi et al., DP-SGD, CCS 2016. The defence mechanism.
- [6] Carlini et al., MIA from first principles, S&P 2022. The TPR@low-FPR metric.
- [12] Zhang et al., Bits for Privacy (PTQ vs MIA), TrustCom 2025. Prior work on compression vs privacy (image models).
- [28] Liu & Paparrizos, TSB-AD benchmark, NeurIPS 2024 D&B. The data source.

## Possible seeds
- Change the attack: move from membership inference to gradient inversion and reconstruction of sensor windows. This paper deliberately excludes white-box and gradient access, which is exactly what an FL server has.
- Change the setting: centralised training becomes FL with update upload. The defence then has to run on the client, so its training-time cost is paid on the MCU. This paper never measures that cost.
- Change the platform: their own future work asks for real-MCU SRAM and energy. Cortex-M33 measurement (EFR32xG24) would fill that gap, but the authors could do it next.
- Measure what was assumed: their envelope screening uses an activation lower bound. Measuring peak SRAM and cycles of per-example clipping and noise on the device is a concrete gap.
- Recheck a negative result in a new setting: "INT8 does not help privacy" could be tested against reconstruction attacks on quantised FL updates.
- Formalise: add a privacy accountant to report ε for the σ values they found to be near-chance (σ ≥ 0.25-0.5).

## Collision questions for D2

**1. Is any FL involved? NO.**
Training is centralised and offline. Each detector is trained by Adam on 200 normal windows in a single process (Sec. 3.1, Sec. 4.2, Alg. 1-2). There are no clients, no server aggregation and no update upload. FL appears only as motivation: the introduction criticises frameworks that call themselves privacy-aware because of "noisy federated updates" [4] (Sec. 1, p. 2; abstract), and [19] (Nasr, FL white-box MIA) is cited but not used.

**2. Gradient inversion or reconstruction attacks? NO (membership inference only).**
The only attacks are a loss-threshold MIA and a shadow-model likelihood-ratio MIA (Sec. 3.3, Sec. 5.2, Table 5). White-box and gradient attacks are explicitly out of scope (Sec. 3.3, Sec. 6.4). There is no attribute inference, model inversion or data reconstruction. "Reconstruction" in the paper always means the autoencoder's own reconstruction of its input, not an attack.

**3. Data? PARTLY (multivariate sensor and other time series, not smart meter or air quality).**
Six TSB-AD-M datasets (Table 3):
- CATSv2: 17 channels
- CreditCard: 29 channels, finance
- Exathlon: 19 channels, facility
- GECCO: 9 channels, water quality sensor
- PSM: 25 channels, facility
- SWaT: 37 channels, water treatment sensor

Window length L=64 and 9-37 channels after dropping constant ones, so each window holds 64 × C values (Sec. 3.1, Table 3). That is moderate-dimensional multivariate data, not univariate or low-dimensional smart-meter load. The task is unsupervised anomaly detection, not forecasting or classification.

**4. Defences, ε, privacy/utility? PARTLY.**
- Defence: DP-SGD-style per-example clipping (C=1) plus Gaussian noise (σ=1 in the main grid; sweep over σ ∈ {0, 0.25, 0.5, 1, 2}). INT8 dynamic quantization is also tested as a possible privacy measure (Sec. 3.4, 3.5, 5.2).
- **No ε values.** There is no accountant, so no (ε, δ) is reported (Sec. 3.4).
- No sparsification and no pruning experiments. Pruning is mentioned only as motivation.
- Privacy/utility at σ=1: attack AUC 0.688 → 0.508, TPR@1%FPR 0.074 → 0.016, utility AUC 0.878 → 0.824 (Table 4).
- Clip-only gives no protection (Sec. 5.2).
- Note: privacy is measured against MIA, not against reconstruction.

**5. Where were costs measured? NO for MCU, NO for defence cost.**
- Desktop AMD Ryzen 5 5500GT with ONNX Runtime only (Sec. 4.2).
- Metrics: ONNX file size, batch-1 inference latency, and an activation-tensor lower bound for SRAM.
- No cycles, no energy, no true peak RAM.
- The **on-device cost of the defence itself was not measured**. The defence acts only at training time, which happens on the PC, and inference cost is identical with and without it (Table 4; Sec. 6.1).

**6. Does future work propose FL, reconstruction attacks or MCU deployment? PARTLY.**
Future work (Sec. 7, Sec. 3.6, Sec. 6.4) proposes:
- stronger shadow-model and white-box MIAs;
- accountant-backed DP;
- on-target MCU profiling of peak memory and energy.

It does **not** propose FL or reconstruction/gradient-inversion attacks. The MCU profiling would be of inference, not of the defence running during training. As a near-term competitor the risk is low to moderate: a follow-up could add MCU numbers, but probably not FL plus reconstruction.

**7. Bottom line.**
- Already covered by this paper (KNOWN): on multivariate sensor time series, clipping plus Gaussian gradient noise (C=1, σ ≥ 0.25-0.5) pushes membership leakage close to chance for about 0.05 AUC loss, measured on a PC. INT8 does not help privacy.
- Still open as far as this paper goes:
  - an FL setting with update upload;
  - gradient-inversion or reconstruction attacks (TS-Inverse-style) by an honest-but-curious server, and whether the same σ defeats them;
  - sparsification as a defence;
  - ε accounting;
  - any real MCU or Cortex-M33 measurement, in particular the cycles, RAM and energy of per-example clipping and noise during on-device training;
  - smart-meter or air-quality data.
- No collision with D2 in its current form. Cite this paper as KNOWN prior evidence that this kind of defence works against MIA on sensor time series. Follow EPC-TinyAD [4], which claims FL-style noisy updates on TinyML, as a possible closer collision.
