# Evaluation of DP-SGD Impact on TinyML-Optimized Models (SBSeg 2025)

## Citation
- Title (original, Portuguese): "Avaliação do Impacto de DP-SGD em Modelos Otimizados com TinyML". English title on the SBC page: "Evaluation of DP-SGD Impact on TinyML-Optimized Models".
- Authors (verified on the SBC landing page and the PDF title block): Davi Bezerra Yada da Silva (UFC Quixadá), Aldri Luiz dos Santos (UFMG), Jeandro de M. Bezerra (UFC / UFMG).
- Venue: Anais do XXV Simpósio Brasileiro de Cibersegurança (SBSeg 2025), Foz do Iguaçu/PR, short-paper track (Artigos Curtos), pp. 979–986. Published 2025-09-01. **Published, peer-reviewed short paper.**
- DOI: 10.5753/sbseg.2025.11481
- URL: https://sol.sbc.org.br/index.php/sbseg/article/view/36671 ; PDF: https://sol.sbc.org.br/index.php/sbseg/article/download/36671/36458

## Reading depth
**Abstract, title block, start of the introduction, and full reference list only.** I downloaded the PDF, but the body (methods, tables, results, conclusion) is in compressed streams the fetch tool could not decode, and no local PDF renderer was available. Everything below about methods marked INFERENCE comes from the abstract or the reference list, not the body. Before anyone cites specifics, the body has to be read (for example by running `pdftotext` on the PDF).

## Core claim (one sentence)
A feed-forward classifier and an autoencoder for IoT intrusion/anomaly detection, trained with DP-SGD on IoT-23 and then shrunk with "TinyML" optimisation, still work on a Raspberry Pi 4. The classifier keeps about 87% accuracy at epsilon = 0.5, and optimisation cuts model size by up to 91%, RAM by up to 82%, and execution time by up to 80%.

## Platform
- KNOWN (abstract): the models were deployed on a **Raspberry Pi 4**. That is a Cortex-A72 Linux single-board computer, **not a microcontroller**. The abstract and visible text name no MCU (no Cortex-M, no ESP32).
- KNOWN (reference list): cites TensorFlow Privacy's `DPKerasAdamOptimizer` (TensorFlow Privacy Team, 2024). INFERENCE: DP training used TF Privacy / Keras with DP-Adam, probably on a PC or workstation. Where training ran is not confirmed from the body.
- INFERENCE: the "TinyML optimisation" is probably TFLite conversion with post-training quantisation, since Hubara et al. 2021 (post-training quantisation) is cited. Not confirmed. Parameter sets, quantisation bit-width and compiler flags are unknown.
- **Flag:** as far as can be seen, DP-SGD training and noise generation did **not** run on the edge device. The Pi 4 is used for inference after optimisation. Costs are measured on the Pi 4, a high-end edge device, not on a constrained MCU.

## Threat model
- KNOWN (abstract/intro): the application is attack and anomaly detection in IoT networks. The motivation is that on-device models handle sensitive data and are open to adversarial attacks (citing Huckelberry et al. 2024).
- INFERENCE: the asset is the training records (network-flow data in IoT-23), and DP-SGD protects against membership or reconstruction inference from the released model. The body was not read, so the paper's exact adversary statement is unknown.
- Out of scope (INFERENCE from the abstract): the on-device noise sampler, RNG quality, floating-point attacks on the Gaussian mechanism, side channels, and federated aggregation.

## Numbers that matter (abstract only; table/figure numbers unknown)
| Item | Value | Source |
|---|---|---|
| FFN accuracy under DP | ~87% at epsilon = 0.5 | abstract |
| Model size reduction from optimisation | up to 91% | abstract |
| RAM use reduction | up to 82% | abstract |
| Execution time reduction | up to 80% | abstract |
| Delta, noise multiplier, clip norm, accountant | not visible | — |
| Energy | not mentioned in abstract | — |

Search snippets say there is a summary table with reconstruction error, precision, accuracy, recall and F1. Its number was not verified.

## DP accountant, sampler, RNG
- Accountant: not visible. INFERENCE: TF Privacy's default RDP accountant (Abadi et al. 2016 moments accountant lineage, which is cited).
- Noise sampler / RNG: not visible. INFERENCE: TF Privacy's `tf.random.normal` in float32 on the training host. Nothing in the abstract or references suggests the paper examines sampler security (floating-point Gaussian issues, randomness source).

## Convenient assumptions (INFERENCE unless noted)
- Centralised training on a capable host. The device only runs inference.
- "Edge device" means a Raspberry Pi 4 (GB-class RAM, Linux), far from an EFR32-class MCU.
- A single public dataset (IoT-23, network flows).
- The DP guarantee is assumed to hold under an ideal Gaussian sampler, with no attention to the implementation.

## Stated limitations and future work
Not readable (the body was not decoded). Do not attribute any limitations to the paper until the body is read.

## Load-bearing references (from the verified reference list)
1. Abadi et al., "Deep learning with differential privacy", ACM CCS 2016: DP-SGD and the moments accountant.
2. TensorFlow Privacy Team (2024), DPKerasAdamOptimizer: the training implementation.
3. Hubara et al., "Accurate post training quantization with small calibration sets", ICML 2021: the likely optimisation method.
4. Chua et al. (2024), "How private is DP-SGD?", arXiv:2403.17673: the sampling-assumption gap in DP-SGD accounting.
5. Huckelberry et al. (2024), "TinyML security", arXiv:2411.07114: the threat motivation.

## Relevance to our question (on-MCU DP noise generation and sampler security)
- As far as can be seen, this paper does **not** run DP-SGD noise generation on a microcontroller, does **not** cost it, and does **not** examine the noise sampler or RNG. DP is applied at training time on a host. The edge measurements (size, RAM, time) are for inference of the optimised model on a Pi 4.
- It does not collide with a direction on "on-device DP noise sampling cost and security on Cortex-M". The caveat is reading depth: the body must be checked to confirm that no on-device training or noise generation is described.

## Possible seeds
- Move the boundary: shift DP noise generation from the host to the MCU (EFR32xG24, Cortex-M33). Cost the Gaussian sampler (cycles, RAM, energy), comparing float and integer/discrete-Gaussian versions with the TRNG as the source.
- Attack the assumption: check whether a float32 Gaussian sampler on a Cortex-M33 FPU shows Mironov-style floating-point leakage, and whether it degrades after quantisation.
- Composition gap: check whether post-training int8 quantisation of DP-trained models (as here) interacts with the DP guarantee or with membership-inference resistance. This is post-processing, so the formal DP guarantee holds, but empirical leakage may change.
- Platform downgrade: repeat the Pi 4 measurements on a real MCU class, where the 82% RAM figure matters.

## Search log (for "open as far as searched")
- WebSearch: `"DP-SGD" TinyML IoT-23 Raspberry Pi 4 autoencoder SBSeg 2025 Silva Santos Bezerra`
- WebSearch: `"Avaliação do Impacto de DP-SGD em Modelos Otimizados com TinyML"`
- Related, not read: SBRC 2025, "Aplicando Privacidade Diferencial contra Ataques de Associação em Internet das Coisas" (https://sol.sbc.org.br/index.php/sbrc/article/view/35188), which also uses DP-SGD with an FFN on IoT-23 and is likely the same group.
