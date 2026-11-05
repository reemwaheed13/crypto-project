# Collision check, round 2, candidate A2: "Forgetting leaks", the privacy cost of federated unlearning for wearable sensor models

Date: 2026-11-05. Checker: collision subagent. Search tools: WebSearch (standard) and WebFetch. Coverage limits: no direct search of the IACR ePrint site, no Google Scholar, and most papers were read only at abstract or HTML level.

## Candidate (summary)
- Application: federated HAR or health model trained across wearables. A user leaves and invokes the right to be forgotten, and the server runs federated unlearning (FedEraser, gradient ascent, or retrain-from-checkpoint).
- Asset: the departed user's sensor windows and sensitive attributes.
- Adversary: honest-but-curious server, or any party that holds the global model before and after unlearning (difference attack). Passive and classical.
- Claim: on wearable time series, the before/after difference leaks more than the final model alone. Unlearning methods differ in how much they leak. A cheap on-device mitigation exists, and its cost on a Cortex-M33 can be estimated.

## Queries run
1. federated unlearning privacy leakage attack difference between model before and after unlearning (technical)
2. federated unlearning reconstruction attack removed client data inference (plain language)
3. eprint.iacr.org machine unlearning privacy leakage federated (ePrint-focused; returned no ePrint hits)
4. federated unlearning wearable human activity recognition sensor privacy attack (neighbouring community: embedded/HAR)
5. federated unlearning attack time series sensor data leakage 2026 (date-restricted)
6. "unlearning" attribute inference before after model difference health wearable federated 2025 (date-restricted)
7. "Federated Unlearning and Its Privacy Threats" IEEE Network 2024 (coordinator request)
8. "Lightweight Federated Unlearning for IoT Sensing Systems" Ameen Wang MSN 2024 (coordinator request)
9. Ameen federated unlearning IoT sensing lightweight 2024 MSN
10. Sheng "Retaliatory Attacks Against Federated Unlearning via Data Leakage" datasets
11. machine unlearning privacy attack time series ECG accelerometer reconstruct forgotten data (neighbouring community: ML/health)

## Papers opened or checked
| ID | Title / authors | Verified | Depth | Same claim? | Same device/data? | Same threat model? |
|---|---|---|---|---|---|---|
| arXiv 2502.14558 (v3) | FUIA: Model Inversion Attack against Federated Unlearning, Zhou and Zhu | Yes, arXiv | HTML full text via fetch summary | Yes: before/after difference, reconstruct features and infer labels | No: CIFAR-10/100 images only | Yes: honest-but-curious server. Tests gradient pruning and Gaussian noise defences (accuracy cost only, no device cost) |
| arXiv 2506.01777 (AISTATS 2026, PMLR v300) | DRAUN, Lamri, Alam, Jiang, Maniatakos | Yes, arXiv and PMLR | HTML full text via fetch summary | Yes: reconstructs unlearned data from unlearning updates, 5 FU methods compared | No: CIFAR-10/100, MNIST, FEMNIST | Yes: honest-but-curious server. Defences are noise and pruning, measured as utility trade-off only |
| arXiv 2507.09602 | DRAGD/DRAGDP, Ju, Fan, Liu, Chang | Yes, arXiv abstract | Abstract only (PDF unreadable, HTML 404) | Yes: gradient differences before/after unlearning used to reconstruct | Images, including faces. Sensor data not mentioned in abstract | Probably server (not stated in abstract) |
| arXiv 2505.11097 | Verifiably Forgotten? Gradient Differences Still Enable Data Reconstruction in Federated Unlearning, Zhang et al. | Yes, arXiv abstract | Abstract only | Yes: reconstructs from gradient differences | Pixel-level inversion, so likely images | Honest-but-curious auditor |
| arXiv 2508.06789 (KSEM 2025) | ULIA: Label Inference Attacks against Federated Unlearning, Wang et al. | Yes, arXiv abstract | Abstract only | Partly: infers labels from the parameter difference at sample, class, and client level | Datasets not named in abstract | Server-side, uses the parameter difference |
| AAAI 2026, DOI 10.1609/aaai.v40i30.39725 | Retaliatory Attacks Against Federated Unlearning via Data Leakage, Sheng et al. | Partly: the AAAI listing exists in search results, but the page fetch was blocked | Search snippet only | Partly: unlearning-induced MIA plus reconstruction, done by a client | Datasets unknown | Adversarial client, not the server |
| arXiv 2404.03659 | Federated Unlearning for Human Activity Recognition, Chen et al. | Yes, arXiv abstract | Abstract only | No: proposes a KL-based unlearning method and uses MIA only to verify forgetting | Yes: wearable HAR | No attack on the before/after difference |
| arXiv 2405.10979 | Private Data Leakage in Federated HAR for Wearable Healthcare Devices, Chen et al. | Search snippet | Snippet only | No: MIA on normal federated HAR, no unlearning | Yes: wearable HAR | Curious server |
| arXiv 2603.24213 | Uncovering Memorization in Timeseries Imputation models: LBRM MIA and attribute leakage, Taleb, Gazeau, Laurent | Yes, arXiv abstract | Abstract only | No: MIA and attribute inference on time-series imputation; no FL or unlearning | Time series (health, IoT) | Black-box, plus a fine-tuning (initial weights) setting |
| IEEE Network 38(2):294-300, DOI 10.1109/MNET.004.2300056 | Federated Unlearning and Its Privacy Threats, F. Wang, B. Li, B. Li | Yes, via search, AMiner, and the survey 2601.09978 ref [30] | Abstract only | Survey or position paper: taxonomy, inference-attack vulnerability, defence summary. No experimental difference attack visible in the abstract | Not sensor-specific as far as seen | Discusses inference attacks in general |
| MSN 2024, pp. 130-137 | Lightweight Federated Unlearning for IoT Sensing Systems, M. Ameen, P. Wang | Partly: the DBLP listing appears in search results, but the abstract and full text were not obtained | Title and venue only | Probably an efficiency/unlearning method (title suggests it). Not confirmed | IoT sensing | Unknown. Not confirmed to contain any attack |
| arXiv 2601.09978 | Federated unlearning survey | Yes | HTML characters 0-200k of 311k | Cites [30]. Mentions FedRollback for IoT sensing (lightweight recovery). No sensor-data difference attack in the part read | n/a | n/a |
| arXiv 2512.18035 | Towards Benchmarking Privacy Vulnerabilities in Selective Forgetting with LLMs, Qian et al. | Yes, arXiv abstract | Abstract only | Benchmark of unlearning privacy attacks, centralized or unspecified | Data not named | n/a |
| arXiv 2405.20272 (NeurIPS 2024) | Reconstruction Attacks on Machine Unlearning: Simple Models are Vulnerable, Bertran et al. | Search snippet | Snippet only | Centralized deletion reconstruction | Tabular and image | Before/after access, centralized |
| arXiv 2005.02205 (CCS 2021) | When Machine Unlearning Jeopardizes Privacy, Chen et al. | Known neighbour from brief, not re-opened | Not re-opened | Centralized MIA using the before/after difference | Images and tabular | Black-box before/after |

## Verdict: partial overlap (strong)
- KNOWN: the core mechanism is already published. The before/after federated-unlearning model difference lets an honest-but-curious server reconstruct forgotten data or infer labels and membership: FUIA 2502.14558, DRAUN 2506.01777 (AISTATS 2026), DRAGD 2507.09602, 2505.11097, ULIA 2508.06789, and Sheng et al. AAAI 2026. DRAUN already compares 5 FU methods. FUIA and DRAUN already test noise and pruning defences. So "unlearning methods differ in leakage" and "noise/pruning mitigates it" are not new as generic claims.
- Open as far as searched: no paper found that runs these difference attacks on wearable or HAR time-series data. Every attack paper checked uses images (CIFAR, MNIST, FEMNIST, faces), or its datasets were unverified. Also not found: sensitive-attribute inference (as opposed to label or class inference) on the departed client from the difference, or an on-device (MCU) cost estimate for the mitigation.
- Caveats: the datasets for DRAGD, 2505.11097, ULIA, and Sheng et al. were not verified at full-text depth, and the Ameen and Wang MSN 2024 content was not obtained. Any of them could use sensor data. Read those four experiment sections and obtain the MSN paper before committing.
- Relation to the excluded directions: A2 is close to "gradient inversion of normal training updates on sensor FL plus DP cost". The FU difference is effectively an aggregated pseudo-gradient of the forgotten data, and the defences are the same noise and pruning. The novelty is a domain transfer (images to wearable time series) plus attribute inference and MCU cost. That is a minor variant of both the published FU attacks and the excluded gradient-leakage direction, unless the group finds a time-series-specific effect.
