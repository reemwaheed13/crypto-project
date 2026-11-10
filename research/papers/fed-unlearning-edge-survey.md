# Federated Unlearning in Edge Networks: A Survey (Ng et al., 2026)

Lane: A

## Citation
- Title: "Federated Unlearning in Edge Networks: A Survey of Fundamentals, Challenges, Practical Applications and Future Directions"
- Authors: Jer Shyuan Ng, Wathsara Daluwatta, Shehan Edirimannage, Charitha Elvitigala, Asitha Kottahachchi Kankanamge Don, Ibrahim Khalil, Heng Zhang, Dusit Niyato
- ID: arXiv:2601.09978 (cs.DC), v1 submitted 15 Jan 2026. DOI 10.48550/arXiv.2601.09978.
- Status: arXiv-only as verified (no venue/comments field on the abstract page, Nov 2026).

## Reading depth
Full text via arXiv HTML (v1), read in chunks through an extraction tool. The section bodies and reference list were covered.
Caveat: I read the paper through a summarising fetch tool, not line by line. Quotes below are what the tool returned as verbatim. Re-check them against the PDF before citing.

## Core claim
Federated unlearning (FU, which the paper abbreviates "FUL") removes a client's, class's or sample's influence from an FL model. This survey sorts FU methods for edge networks along three challenges (communication efficiency, resource allocation/edge constraints, and security and privacy), then lists applications and open problems.

## Platform
- None. This is a survey with no experiments of its own. No hardware, emulator, parameter sets or compiler flags.
- The cost numbers it quotes from other papers are wall-clock or round counts, not device measurements (see below).

## Threat model (as framed by the survey)
- Assets: client data whose influence must be erased (right to be forgotten), plus intermediate model states and deletion metadata.
- Adversaries discussed: poisoning/backdoor clients (Sec. IV-A), and inference adversaries (membership, attribute, re-identification; Sec. IV-C). The survey also covers a server that must prove deletion (certified/verifiable unlearning, Sec. IV-B).
- The survey's general statement on leakage (Sec. IV intro), as returned by the tool: "deletion requests, correction signals, and intermediate model states can all expose sensitive details". No specific attack paper is attached to this statement.
- Out of scope / absent: the survey does not formalise any one adversary. It also does not treat a server or observer that compares models before and after unlearning (a "difference attack") as a named threat.

## What it says about privacy leakage CAUSED by unlearning (Lane A focus)
- No named difference attack, no before/after membership inference, and no reconstruction-from-model-difference attack is described.
- Chen et al., CCS 2021, "When Machine Unlearning Jeopardizes Privacy": NOT cited. I checked reference titles [1]-[168] for "Jeopardizes", "membership inference" and "inference attack". The only membership-inference title found is [152].
- The closest items:
  - [30] Wang, Li, Li, "Federated Unlearning and Its Privacy Threats", IEEE Network 38(2):294-300, 2024. The survey describes it as analysing FU's "susceptibility to membership inference attacks" (Sec. I-A). This is the obvious next paper to read for Lane A.
  - [152] Carlini, Chien, Nasr, Song, Terzis, Tramer, "Membership Inference Attacks from First Principles" (IEEE S&P 2022). Cited only as a post-deletion empirical check (Sec. VII, "Privacy beyond deletion").
  - Sec. VII, "Privacy beyond deletion" (tool-returned verbatim): "Even after removal, traces of membership or attributes may persist in outputs, activations, or gradients." The fixes it proposes are Renyi DP [150], DP-SGD [151], secure aggregation [143, Bonawitz et al. CCS 2017] and MIA tests [152].
  - Defences in Table VII / Sec. IV-C:
    - Aegis [95] (attribute inference, federated recommender, WWW 2025)
    - ConFUSE [96] (ISVLSI 2024)
    - CUFRU [97] (cross-user recommendation leakage, ACM TIST 2025)
    - Ontology-guided FU [98] (Future Internet 2025)
    - Plausible deniability of participation [99] (Varshney and Torra, Machine Learning 2025)
    - Get Rid of Your Trail [86] (metadata leakage)
    - FedRecovery [41] (DP unlearning, IEEE TIFS 18, 2023)
    - Dynamic participation + SecAgg+ [82] (Liu et al., IEEE TDSC 2024)
    - CerFeaUn [92] (certified feature unlearning, VFL, IEEE TMC 2025)
  - Sec. III-E notes that finer rollback logs increase "storage and privacy risk". In other words, storing history for cheap unlearning is itself a leakage surface. This is a lesson the survey draws, not a measured result.

## Sensor / time-series / wearable data
- IoT FU methods cited:
  - FedBT [83] (IEEE IoT-J 12(15), 2025, DOI 10.1109/JIOT.2025.3571432)
  - Ameen and Wang [109], "Lightweight Federated Unlearning for IoT Sensing Systems", MSN 2024
  - Yuan et al. [85], "Toward Efficient and Robust Federated Unlearning in IoT Networks", IEEE IoT-J 11(12), 2024
  - FMU [118] (VFL in IoT, IEEE TMC 2025)
- Wearables and biometric patterns are mentioned only as personalisation targets (Sec. VI-C), with no study cited. Vehicular sensor segments and time windows appear in Sec. VI-D (DT-FU [88], UaaS-SFL [89], FedEditor [105]).
- No privacy attack on unlearning is tied to low-dimensional sensor or time-series data, and no time-series dataset is named.

## Numbers that matter (all quoted from other papers; none on-device)
- FedEraser [34] (IWQoS 2021): up to 4x speedup vs naive retraining (Sec. II-B2).
- KNOT: up to 85% faster retraining than baselines in the asynchronous setting (Sec. III-C).
- Compression/distillation methods: "comparable recovery in 20-40% fewer rounds". Selective rollback cuts communication "by up to an order of magnitude" vs retraining (Sec. III-E). The underlying setups are not given.
- On-device cost: NO quantitative energy, memory, latency or cycle figures. Only qualitative claims:
  - "minimal energy" and "battery usage remains acceptable" (Sec. V, Table VIII)
  - HE-based methods (RevFRF [39]) carry "heavy computational overhead" on constrained devices (Sec. IV-D)

## Convenient assumptions (of the surveyed field, as presented)
- Image and recommender benchmarks dominate. There are no sensor-specific evaluations of leakage.
- Server-side history storage (rollback logs) is assumed available, and its storage and privacy cost is noted but not quantified.
- Device cost is reported as rounds or speedup, with no MCU or radio cost.

## Stated limitations / future work (paraphrased)
- Fairness and heterogeneity remain open (Sec. III-A).
- No single method dominates across the dimensions (Sec. III-E).
- Sec. VII open challenges:
  - availability, asynchrony and dropouts
  - privacy beyond deletion (residual membership or attribute traces)
  - verifiability and audit
  - cryptography and TEEs for constrained devices
  - communication-compute co-design
  - concept drift
  - benchmarks combining deletion streams with non-IID data and intermittent participation
  - scheduling and incentives
  - governance

## Load-bearing references
- [30] Wang, Li, Li, "Federated Unlearning and Its Privacy Threats", IEEE Network 38(2), 2024.
- [34] Liu et al., "FedEraser", IWQoS 2021, DOI 10.1109/IWQOS52092.2021.9521274.
- [41] Zhang et al., "FedRecovery: Differentially Private Machine Unlearning for Federated Learning Frameworks", IEEE TIFS 18, 2023.
- [152] Carlini et al., "Membership Inference Attacks from First Principles", IEEE S&P 2022.
- [109] Ameen and Wang, "Lightweight Federated Unlearning for IoT Sensing Systems", MSN 2024.

## Possible seeds
- Transplant (move an attack to a new setting): run a Chen et al. CCS 2021-style before/after difference attack on FU for wearable/IMU or other low-dimensional time-series data. The survey cites no such attack. This is open as far as this survey covers it, not a claim that the gap is open overall; check [30] and Chen et al.'s citers next.
- Measure the unmeasured: the on-device cost of FU (and of the DP or secure-aggregation defences it proposes) is only qualitative here. Cycle and memory estimates on a Cortex-M33-class MCU would fill that gap.
- Attack the defence: rollback logs and stored history are flagged as a privacy risk without quantification. Measure how much stored per-round updates leak about a forgotten client.
- Trade-off curve: DP noise (FedRecovery-style) vs success of the difference attack vs accuracy on sensor data, under MCU memory limits.
- Threat-model shift: the survey's MIA check [152] targets the post-deletion model alone. Model an observer who holds both models (the server, or a client that sees two consecutive global models).
