# TS-Inverse — paper note

## Verified identity (KNOWN)
- **arXiv ID**: 2503.20952 (v1, submitted 26 Mar 2025). Confirmed.
- **Exact title**: "TS-Inverse: A Gradient Inversion Attack Tailored for Federated Time Series Forecasting Models".
- **Authors**: Caspar Meijer, Jiyue Huang, Shreshtha Sharma, Elena Lazovik, Lydia Y. Chen.
- **Affiliations**: Delft University of Technology; Dutch Organization for Applied Scientific Research (TNO), The Hague; University of Neuchâtel.
- **Year**: 2025.
- **Venue (CORRECTION to "preprint-only")**: The PDF title page states "This work has been accepted for publication in the **3rd IEEE Conference on Secure and Trustworthy Machine Learning (SaTML 2025)**." So: **accepted, SaTML 2025** (arXiv is the author copy; final version on IEEE Xplore).
- **Code**: https://github.com/Capsar/ts-inverse (CC BY-SA 4.0).

> Note: earlier web-fetch summaries of the raw PDF produced several WRONG details (claimed LSTM/Transformer models, ECG/Traffic datasets, MSE/MAE as the headline metric, "secure aggregation" as a tested defence). Those were hallucinations off a corrupt PDF stream. This note is written from the rendered pages of the actual PDF.

## Core claim (KNOWN)
Gradient inversion attacks (GIA), previously studied for image/text classification, can be adapted to **federated time-series forecasting (TSF) regression** to reconstruct a client's private input series (both the observation window and the forecast target) from the gradients it sends. The authors' tailored attack, TS-Inverse — a learned gradient-inversion model producing quantile bounds, plus an L1 gradient-matching loss with periodicity / trend / quantile-bound regularization — beats existing GIAs by at least 2x–10x in sMAPE on TS data.

## Platform & what was ACTUALLY measured (KNOWN unless flagged)
- **Datasets (4)**: Electricity 370 (UCI electricity load diagrams), London Smartmeter, a **Proprietary** energy dataset, and **KDDCup 2018 (Air Quality)** as an extra generalization test. Three are energy load (smart-meter/household) series; one is air quality. (Table III, Sec V-A.)
- **Dimensionality**: Univariate TSF focus (d=1 described in Sec III-A; forecasting individual household/building load). Dataset config in Table III: e.g. Electricity 370 FREQ 15 min, obs length H=96, future F=96, window 192; London Smartmeter 30 min, H=F=48; KDDCup hourly, H=120, F=48. So these are **single-channel load/air-quality series over fixed windows**, not high-dimensional multivariate tensors.
- **Models (5 architectures, NOT LSTM/Transformer)**: FCN (3 FC layers, sigmoid), CNN (LeNet-style 1D conv), TCN (kernel 6, dilation 2), GRU-2-FCN, GRU-2-GRU. Hidden size 64. The paper explicitly excludes transformer TSF as out of scope and focuses on GRU/TCN plus FCN/CNN.
- **Metric**: Symmetrical Mean Absolute Percentage Error (**sMAPE**, lower is better), bounded [0,2], reported separately for Observation and Target reconstruction. (Sec V-A.5.)
- **Baselines compared (5)**: DLG-LBFGS, DLG-Adam [11], InvG (Inverting Gradients) [14], DIA (Dropout Inversion Attack) [19], LTI (Learning to Invert) [20]. (Table II.)
- **Headline "2x–10x" — AS STATED**: "at least a 2x-10x improvement in terms of the sMAPE metric over existing GIA methods on TS data." The main comparison is Table II (batch size B=1, 5000 attack steps, seeds 10/43/28/80/71). Example magnitudes from Table II (sMAPE, lower better): London Smartmeter CNN Observation — TS-Inverse 8.1e-05 vs InvG 0.027, DIA 0.094; CNN Target — TS-Inverse 2.4e-05 vs DLG-Adam 0.006. TCN Electricity 370 Observation — TS-Inverse 0.097 vs LTI 0.158, DLG-LBFGS 0.652. A "TS-Inverse one-shot" variant (analytic target recovery for B=1) often reaches ~1e-07 on targets. (All from Table II.)
- **Where run / on-device?**: Everything is a **PC/server/GPU simulation of FL** (the attack trains an auxiliary inversion model for 75 epochs; optimization attacks run 5000 iterations; LTI 250 epochs). **No MCU/embedded measurement; nothing on-device.** (ASSUMPTION/INFERENCE: the paper's experimental-setup section does not state a specific GPU model — hardware is not reported in the text I read. Flag: no cycle/energy/memory cost on IoT hardware at all; this is purely a PC-side attack-quality study.)

## Threat model (KNOWN — precise)
- **Adversary**: **Honest-but-curious server** ("watching" server, Fig. 1). Follows protocol, tries to infer client data from received gradients. Not malicious (does not modify the model to amplify leakage).
- **Access granularity**: **Single client's gradient**, NOT the aggregate. Each client does one FedSGD step on one batch and sends ∇W_k; the adversary targets that per-client update. (Sec III-A: "The attack considered is a white-box attack … The adversary has access to the model weights (W^r_g) and aggregated gradients (∇W^r_k) at communication time and aims to reconstruct each sample from the batched data.")
- **White-box**: FL strategy, model architecture, optimizer, loss, observation/target shapes, learning rate α, and **batch size B are all known** to the adversary.
- **Batch size**: Main results use **B = 1** (one sequence pair per update); ablations go to B ∈ {1,2,4}. A special **one-shot analytic target recovery is derived only for B=1** (invertible bias gradient of the final FC layer). Larger batches are harder.
- **Defences assumed absent in main setting**: **No differential privacy and no secure aggregation in the baseline attack** — gradients are observed directly. (Defences are added only in a separate robustness section, below.)
- **Auxiliary data**: Adversary has an **auxiliary dataset D_aux** whose distribution overlaps the client's (ideally a subset of the data silos; can also be out-of-distribution, per LTI style), used to train the learned inversion model f_inv. Adversary also has compute to train models outside the FL framework.

## Convenient assumptions (what makes the attack work)
- Per-client gradient is observable in the clear (no secure aggregation, no DP). ASSUMPTION — defeated by either defence.
- Batch size = 1 (and small, ≤4) — the headline numbers and the analytic one-shot target recovery depend on it. ASSUMPTION.
- Full white-box knowledge of architecture, loss (MSE), optimizer, window sizes.
- Access to a distribution-overlapping auxiliary dataset large enough to train a sizeable inversion network (residual blocks, hidden sizes 768/512). This is a strong informed-attacker assumption.
- FedSGD (single gradient step), not multi-step FedAvg — simplifies the gradient→data mapping.
- Data pre-scaled to [0,1] (min-max), bounding the reconstruction target.
- Series have exploitable **periodicity and trend** (energy load) — the regularizers encode this; air-quality (KDDCup) tests how far that generalizes.

## Defences tested (KNOWN — important correction)
The paper tests three defences on Electricity 370 (FCN/CNN), Table IX — these are **NOT secure aggregation**:
- **gauss** (Gaussian / differential-privacy-style noise on gradients): **strongly degrades** reconstruction. e.g. CNN Observation sMAPE jumps from ~1e-05 (none) to ~0.59 for TS-Inverse; best attacker under noise is LTI (~0.22). So DP noise is the most effective defence shown.
- **prune** (gradient pruning / sparsification [38]): relatively weak — attacks still reconstruct well (TS-Inverse Observation ~0.011, Target ~0.0009); pruning does not stop inversion here.
- **sign** (SIGNSGD / sign compression [40]): **strongly degrades** reconstruction (most attacks ~1.0–1.9 sMAPE; LTI best at ~0.32 obs / ~0.18 tar).
- Separately, the authors note **GRU-based forecasting models are inherently more robust** to inversion (many-to-one recurrent mapping loses information), i.e. architecture choice is itself a partial defence (Fig. 11, Table IX discussion).
- Observation: under the stronger defences (gauss, sign) the **learned LTI attack outperforms TS-Inverse**, because the inversion model can be trained on defended gradients; TS-Inverse's edge is largest in the undefended / weak-defence regime.
- **Secure aggregation is discussed as a threat-model assumption (its absence), but is NOT experimentally evaluated. DP is only approximated via Gaussian gradient noise; no formal (ε,δ) budget is reported.**

## Stated limitations & future work (paraphrase; ≤1 quote)
- Scope limited to **univariate** TSF; multivariate (d>1) is out of scope. Transformer TSF architectures also out of scope.
- The one-shot analytic target recovery is proven only for **batch size 1** (requires invertible bias gradient); general batches rely on optimization and degrade.
- Non-IID client data and its convergence effects are acknowledged but **"beyond the scope of this paper"** (direct quote, Sec II-B).
- Effectiveness of the TS regularizers (periodicity λ_P, trend λ_T, quantile bounds λ_Q) is **dataset-dependent and needs per-dataset tuning** — optimal λ values differ across Electricity/London/Proprietary.
- Implied future direction: defences that keep privacy and utility; the GRU robustness observation invites deeper study of architecture-as-defence.

## Load-bearing references (3–5)
1. L. Zhu, Z. Liu, S. Han, "Deep Leakage from Gradients" (**DLG**), NeurIPS 2019 — foundational GIA; L2 gradient matching. [ref 11]
2. B. Zhao, K. Mopuri, H. Bilen, "iDLG: Improved Deep Leakage from Gradients," 2020 — analytic label recovery, B=1. [ref 13]
3. J. Geiping et al., "Inverting Gradients — how easy is it to break privacy in federated learning?" (**InvG**), NeurIPS 2020 — cosine-similarity + total-variation, the key image-domain predecessor. [ref 14]
4. R. Wu, X. Chen, C. Guo, K. Weinberger, "Learning to Invert" (**LTI**), UAI 2023 — learned inversion model from auxiliary data; robust to DP. Direct methodological parent of TS-Inverse's f_inv. [ref 20]
5. B. McMahan et al., "Communication-efficient learning of deep networks from decentralized data" (FedAvg/FedSGD), AISTATS 2017 — FL protocol under attack. [ref 1]
(Also relevant: Yin et al. "GradInversion / See through gradients," CVPR 2021 [ref 16]; Scheliga et al. "DIA," AAAI 2023 [ref 19].)

## Relevance to our project (one honest paragraph)
This is the natural **seed** for moving gradient-inversion from images to low-dimensional IoT sensor time series, and it is directly on our energy-load use case. It already uses **genuinely low-dimensional, univariate IoT-style series** (per-household smart-meter load, air quality) sampled on realistic schedules (15/30 min, hourly) — not high-dim aggregated grids — so the "very low dimensionality" regime is partly covered. What is left open and squarely in our lane: (1) **on-device / IoT-cost side is entirely unmeasured** — the whole paper is PC/GPU attack-quality; nobody has estimated the MCU cost (cycles, RAM, energy, comms) of the *defences* (DP noise, clipping, sign compression) on something like the EFR32xG24, nor the ciphertext/communication cost of adding secure aggregation that this attack assumes away. (2) **Secure aggregation is never evaluated** — only its absence is assumed; measuring whether a lightweight secure-aggregation or DP scheme actually neutralizes TS-Inverse on constrained hardware is open. (3) **Behaviour at the smallest batch (B=1) under realistic defences** is where TS-Inverse's advantage shrinks and LTI wins — quantifying the privacy/accuracy/cost trade-off on real IoT update sizes is open. (4) Their DP is only Gaussian gradient noise with **no reported (ε,δ) budget** — a rigorous DP-vs-reconstruction curve tied to IoT utility is missing. CONJECTURE (ours): the strong per-client-gradient, B=1, white-box, auxiliary-data assumptions may be unrealistic for a real FL deployment with FedAvg multi-step updates and secure aggregation; a worthwhile software question is how much leakage survives once those convenient assumptions are relaxed, and what the cheapest on-device defence is that kills it.

## Possible seeds (direction-miner moves, one line each)
- **Relax-an-assumption**: re-run the attack under FedAvg multi-step + B>1 (not FedSGD/B=1) and measure how fast reconstruction collapses.
- **Measure-the-missing-cost**: estimate on-device (EFR32xG24) cost of DP noise / clipping / sign-SGD / secure aggregation — cycles, RAM, energy, bytes per round — the paper omits all of this.
- **Close-the-defence-gap**: test a real secure-aggregation or calibrated-(ε,δ) DP scheme (never evaluated here) against TS-Inverse and report a privacy/utility/cost frontier.
- **Push-the-regime**: go to even lower dimensionality / shorter windows / single-sensor streams and see if periodicity+trend priors still enable reconstruction.
- **Flip-attacker-to-defender**: use TS-Inverse as the adversary benchmark to certify "leakage per update" for our own federated energy-forecasting prototype.

## Reading depth
**Full-read** of the rendered PDF (all 16 pages: abstract, Sec I–VII, Tables I–X, Figs 1–11, references). Hardware/GPU platform is the one item not stated in the text and is flagged as not reported.

## Full-text check (2026-11-10, local PDF `2503.20952v1.pdf`)
The note above was re-checked against the local PDF and stands. Points that matter for D2's re-scoring:
1. **B = 4 is already tested.** Figs 9–10 (p. 10) use regularised TS-Inverse at B = 4 and still reconstruct. D2's kill experiment ("does reconstruction already fail at batch > 1?") is therefore partly answered for B = 4 under FedSGD. Multi-step FedAvg is still untested.
2. **Table IX (p. 11) states no noise scale (σ or ε) for "gauss".** Noise defeats the optimisation attacks (sMAPE about 1.8–2.0), but the **learned LTI attack keeps sMAPE about 0.22/0.23**, and the authors suggest it "could potentially learn the defense characteristics". A defence study must therefore include a learned, defence-aware attacker, not only optimisation attacks.
3. No device cost, no ε accounting and no secure aggregation, as stated above.
