# When Machine Unlearning Jeopardizes Privacy

Lane: A

## Citation
- Title: When Machine Unlearning Jeopardizes Privacy
- Authors: Min Chen, Zhikun Zhang, Tianhao Wang, Michael Backes, Mathias Humbert, Yang Zhang
- Venue: ACM CCS 2021 (published). DOI 10.1145/3460120.3484756 is linked from the arXiv page. arXiv 2005.02205: v1 5 May 2020, v2 14 Sep 2021. Code on GitHub (per abstract).
- Reading depth: full text via ar5iv HTML (about 100k of 108k characters read; the unread tail is references/appendix). The arXiv PDF could not be text-extracted in this session.

## Core claim
If an attacker can query a model both before and after a sample is deleted, even by full retraining, comparing the two outputs reveals whether that sample was in the training data. On well-generalized models this works better than classical membership inference (MI).

## Platform
- Server only: Python 3.7, Ubuntu "19.10 LTS" (as written), Intel Xeon E7-8867 v3 @ 2.50 GHz, 1.5 TB RAM. DP-SGD uses Opacus.
- No MCU, embedded or on-device measurement. Compiler flags: not applicable / not reported.
- Costs were measured only on a server, and attack cost is not reported as a metric.

## Threat model
- Adversary: black-box query access to the original model M_o and the unlearned model M_u, plus a local shadow dataset (same or different distribution). It can detect that an update happened by watching outputs change on repeated queries.
- Goal: membership only. Decide whether target x was "in" M_o's training set and "out" of M_u's. The paper does not attempt attribute inference or reconstruction, and contrasts itself with Updates-Leak (Salem et al.), which reconstructs data and infers labels from model updates.
- Protected asset: membership of the deleted record (motivating example: a patient's record in a disease model).
- Out of scope: white-box parameter access, federated settings (FL appears only in related work), and label-only attacks with posterior recovery (left as future work).

## Method (plain words)
Query both models on x to get two probability vectors. Combine them into one feature vector (DirectConcat, SortedConcat, DirectDiff, SortedDiff, or EucDist) and give it to a binary attack classifier (LR, DT, RF or MLP). The classifier is trained on shadow original/unlearned model pairs. Defaults: 20 shadow original models, 5,000 samples each, and 100 one-sample-deleted unlearned models per original. Rule of thumb (Sec. 5.3): use concatenation features for overfitted models, difference features for well-generalized models, and always sort the posteriors.

## Unlearning methods attacked
- Retraining from scratch (main results, Fig. 3–4).
- SISA (Bourtoule et al.) with 5 shards and posterior averaging (Sec. 5.7, Fig. 7). The attack is weaker here because aggregation dilutes each sample's influence.

## Data and models
- Tabular/categorical: Adult (14 features, 2 classes), Accident (30, 3), Insta-NY and Insta-LA (169, 9). Images: MNIST, CIFAR10, STL10 (Table 1). Insta-LA and STL10 are used only for transfer experiments.
- Target models: LR, DT, RF, 5-layer MLP, SimpleCNN (MNIST), DenseNet and ResNet50 (CIFAR10).
- No time-series, sensor, wearable, HAR or ECG data.

## Numbers that matter
Metrics: AUC; DegCount (share of targets where the attack is more confident than classical MI); DegRate (average confidence gain).
- Table 2 (attack AUC vs classical MI AUC):
  - Adult DT: 0.882 vs 0.497
  - Insta-NY LR: 0.983 vs 0.490
  - Accident DT: 0.929 vs 0.501
  - Insta-NY RF: 0.685 vs 0.551
  - CIFAR10 DenseNet: 0.881 vs 0.630
  - CIFAR10 ResNet50: 0.719 vs 0.548
  - MNIST SimpleCNN: 0.511 vs 0.496
- Categorical data (Fig. 3): AUC gain of up to 0.48 over classical MI. Best DegCount 0.94, best DegRate 0.40. DT is most vulnerable.
- Adult DT (intro): DegCount 0.85, DegRate 0.28.
- Ablation (Fig. 6, Adult DT): AUC plateaus after about 5 shadow models and about 1,000 samples per shadow model. The number of unlearned shadow models (Su) barely matters.
- Transfer (Table 3):
  - Insta-NY to Insta-LA: about a 1% AUC drop (DT 0.944 to 0.931).
  - LR to DT shadow model: 0.927.
  - CIFAR10 to STL10 (DenseNet): 0.813.
- Harder scenarios (Sec. 6, Fig. 8–9):
  - With fewer than 10 intermediate model versions (LR), AUC stays at or above 0.84.
  - Group deletion of 0.2%: 0.842 vs 0.972 for single deletion (LR).
  - Online add+delete: 0.972 to 0.940.
  - Membership of the retained samples: AUC below 0.6.
- Defences (Sec. 7, Table 4):
  - Top-k posteriors (k = 1–3): ineffective, AUC about 0.90–0.97.
  - Label-only output: AUC about 0.45–0.55.
  - Temperature scaling: AUC about 0.61–0.65 (Insta-NY LR).
  - DP-SGD on LR only: AUC about 0.52 at eps = 4.64 and about 0.48–0.50 at eps = 0.7, with an accuracy loss.
- Inconsistencies noted in the source: DenseNet overfitting is 0.439 in the text but 0.465 in Table 2. The conclusion says "five datasets" while Table 1 lists seven.

## Convenient assumptions
- The adversary sees both model versions and can tell which update was the deletion.
- Most experiments delete a single sample, and the attack weakens with group deletion or many intermediate versions.
- The attacker has a shadow dataset and the compute to train about 20 x 100 shadow models.
- Centralized training only. No clients, aggregation noise or non-IID data.
- Tabular and image data only.
- Accuracy is the only cost of the defences. No device, energy or communication costs are considered.

## Stated limitations / future work (paraphrased)
- Label-only attacks that recover posteriors by sampling could break the label-only defence. Left open.
- The attack degrades with SISA, group deletion and many intermediate models.
- DP needs budget tuning to balance utility. Stronger defences are future work.
- The authors plan to study other attacks under unlearning, such as adversarial examples and model stealing.

## Load-bearing references
1. Cao and Yang, "Towards Making Systems Forget with Machine Unlearning" (S&P 2015): the original machine unlearning paper.
2. Bourtoule et al., "Machine Unlearning" (SISA, S&P 2021).
3. Shokri et al., membership inference attacks (S&P 2017), and Salem et al., ML-Leaks.
4. Salem et al., "Updates-Leak" (USENIX Security 2020): reconstruction and label inference from model updates, the closest precursor to a reconstruction variant.
5. Abadi et al., DP-SGD (CCS 2016).

## Forward search (2024-2026, IDs verified on arXiv abstract pages unless marked)
- arXiv 2404.03233: Hu, Wang, Dong, Xue, "Learn What You Want to Unlearn: Unlearning Inversion Attacks against Machine Unlearning", IEEE S&P 2024. Centralized; recovers features and labels of deleted data from the original/unlearned pair.
- arXiv 2405.20272: Bertran et al., "Reconstruction Attacks on Machine Unlearning: Simple Models are Vulnerable", arXiv-only. Exact reconstruction of a deleted point in linear models. Relevant because small HAR models are also simple.
- arXiv 2502.14558: Zhou and Zhu, "Model Inversion Attack against Federated Unlearning" (FUIA), arXiv-only. Honest-but-curious server compares before/after parameters under sample, client and class unlearning. CIFAR-10/100 with ResNet-18/44; metrics MSE/PSNR. Defences: gradient pruning (p = 0.2) and Gaussian perturbation (std 0.003). Image data only. Effect weakens as more clients are forgotten.
- arXiv 2506.01777: Lamri, Alam, Jiang, Maniatakos, "DRAUN: An Algorithm-Agnostic Data Reconstruction Attack on Federated Unlearning Systems", arXiv-only. Malicious server; 5 FU methods, 4 datasets and 4 architectures (datasets not named on the abstract page).
- arXiv 2507.09602: Ju, Fan, Liu, Chang, "DRAGD: A Federated Unlearning Data Reconstruction Attack Based on Gradient Differences", arXiv-only. Uses before/after gradient differences; the DRAGDP variant adds public priors for face images.
- arXiv 2505.11097: Zhang et al., "Verifiably Forgotten? Gradient Differences Still Enable Data Reconstruction in Federated Unlearning", arXiv-only. An honest-but-curious auditor of proof-of-unlearning reconstructs samples (SVD plus a pixel-level inversion model). Proposes orthogonal obfuscation as a defence. Image-oriented.
- arXiv 2508.06789: Wang et al., "Label Inference Attacks against Federated Unlearning" (ULIA), KSEM 2025. 100% attack success rate (ASR) for class- and client-level unlearning under IID data; 93% to 62.3% ASR when 1% of a user's data is forgotten.
- arXiv 2605.01129: Fu et al., "Revisiting Privacy Leakage in Machine Unlearning: Membership Inference Beyond the Forgotten Set" (TC-UMIA), accepted to Euro S&P 2026. Centralized; shows that unlearning raises membership risk for the retained data.
- arXiv 2404.03659: K. Chen et al., "Federated Unlearning for Human Activity Recognition", arXiv-only. Proposes an FU method for HAR and uses MI only to verify unlearning, not as a before/after attack. No on-device cost figures in the abstract.
- arXiv 2603.24213: Taleb, Gazeau, Laurent, LBRM membership inference and attribute leakage on time-series imputation models, arXiv-only. Time-series MI and attribute inference, but no unlearning.
- UNVERIFIED (no ID; the AAAI OJS page blocked access): "Retaliatory Attacks Against Federated Unlearning via Data Leakage" (AAAI; search snippet says Sheng et al.). Uses the before/after global-model difference for MI and then reconstruction. Check before citing.
- Gap, open as far as searched: no paper found that applies before/after (federated) unlearning attacks to time-series, sensor, wearable, HAR or ECG data, or that reports on-device cost of a defence. Queries used:
  - "federated unlearning privacy leakage attack reconstruct forgotten client data before after unlearning global model"
  - "federated unlearning membership inference human activity recognition OR ECG OR wearable OR time-series privacy attack"
  - "machine unlearning privacy attack time series sensor data OR ECG OR "activity recognition" membership inference unlearned model"
  - "membership inference attack against federated unlearning before after unlearning client-level arXiv"
  - "machine unlearning reconstruction attack difference original unlearned model arXiv 2024"

## Possible seeds
- Change the data modality (KNOWN gap as far as searched): rerun the FUIA/DRAUN-style before/after-unlearning reconstruction on HAR/ECG windows. CONJECTURE: low-dimensional 1-D windows are easier to invert than CIFAR images.
- Raise the attack goal: move from membership (this paper) to attribute inference of the departed client (for example sex, age band or health condition). This is not covered by FUIA or ULIA, which reconstruct features or labels.
- Swap in the IoT constraint: a defence evaluated on-device. Measure the MCU cost (memory, cycles, energy, extra radio bytes) of client-side DP noise or obfuscation during the unlearning round, which none of the listed papers report. Costs would be emulator estimates.
- Change the observer: a later model consumer with only black-box access to the before/after global models (this paper's model) versus the white-box server (FUIA). Compare how much each leaks in FL with small sensor models.
- Count the leftover signal: client-level deletion in FL is "group deletion". This paper reports that group deletion weakens the attack (0.972 to 0.842), so measure how leakage scales with the departing user's number of windows and with how many clients leave together.
