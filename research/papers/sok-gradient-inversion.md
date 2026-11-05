# Paper note — SoK: Gradient Inversion Attacks in Federated Learning

## Verified identity (KNOWN)
- **Title (exact):** "SoK: Gradient Inversion Attacks in Federated Learning"
- **Authors:** Vincenzo Carletti, Pasquale Foggia, Carlo Mazzocca, Giuseppe Parrella (corresponding), Mario Vento — all University of Salerno (Dept. of Information and Electrical Engineering and Applied Mathematics), Fisciano, Italy.
- **Venue:** 34th USENIX Security Symposium, August 13–15, 2025, Seattle, WA, USA. Pages 6439–6459 (ACM DL article No. 331). ISBN 978-1-939133-52-6.
- **URL:** https://www.usenix.org/system/files/usenixsecurity25-carletti.pdf ; presentation page https://www.usenix.org/conference/usenixsecurity25/presentation/carletti ; companion repo https://github.com/gparrella12/sok_gias_fl
- **Verification note:** Candidate title, venue, and first author (Carletti) all CONFIRMED against the PDF title page and USENIX page. No mismatch. Abbreviation used below: GIA = Gradient Inversion Attack; SA = Secure Aggregation; DP = Differential Privacy; HE = Homomorphic Encryption.

## Reading depth
**Full-read** of the structure-critical sections. Read as rendered PDF images: title page, abstract + §1 Introduction, §2 System Model, §3 Threat Model (Table 1), §4 attack taxonomy (Table 2), §5 Defenses (Table 3), §6 Metrics (Table 4), §7 Open Challenges, §8 Related Work (Table 5), §9 Conclusion + Open Science. Did not line-read every per-attack subsection in §4; the summary tables were read directly.

## Core claim (KNOWN)
The paper is a *systematization of knowledge* (literature study), not a new attack/defence. It unifies the GIA-in-FL field along four axes: (1) eight threat models ordered by adversary knowledge/capability; (2) a three-family attack taxonomy (optimization-based, generative-model-based, analytic-based); (3) a five-class defence taxonomy with an explicit "which attacks already bypass this" verdict; and (4) a taxonomy of privacy-leakage metrics. Headline conclusions: GIAs are a severe, real privacy threat to FL; passive-server defences (SA, gradient perturbation) are increasingly circumvented by modern and *active/stealthy* attacks; and the field is heavily skewed toward image classification, leaving large gaps.

## Platform & what was ACTUALLY measured (KNOWN)
- **No re-running of attacks.** The "Open Science" statement is explicit: *"This work does not introduce new artifacts, such as source code, as it focuses on a systematic review and analysis of existing literature."* All quantitative figures below are **extracted from the surveyed papers and tabulated**, not independently reproduced. There is **no PC/server/MCU benchmarking of cost** in this paper at all — a point we must respect when citing it for cost claims.
- **Datasets / modality:** Overwhelmingly **image only**. Table 2 lists every surveyed attack by image resolution (32×32, 64×64, 128×128, 224×224, 256×256) and architecture (LeNet, ResNet-18/34/50, VGG-16, ViT, small FC/CNN nets). The illustrative reconstructions (Fig. 1) are images at batch size 8. **No tabular, time-series, or low-dimensional sensor modality appears** in the comparison tables; §7 names "beyond image classification" as an open problem, confirming the absence.
- **Metrics catalogued (Table 4):** image-similarity — PSNR, MSE, SSIM, LPIPS, Jaccard, SemSim, AVD; image-recognizability — IIP, RDLV. Takeaway T-7: traditional pixel metrics *underestimate* privacy risk; LPIPS better tracks human judgment for the passive-server threat models.
- **Batch size (as stated, from Table 2):** attacks in the literature span batch sizes **1 to 1024**; the general trend reported is smaller batches are more vulnerable (batch 1 is the common high-fidelity regime; larger batches, e.g. 256/512×8/1024, appear mostly for stronger analytic or generative attacks). This is a *tabulation of reported settings*, not a controlled sweep.
- **Data dimensionality (as stated):** higher image resolution = harder reconstruction (reported qualitatively; 256×256 cases rely on generative/analytic methods). No quantitative dimension-vs-success curve is produced by the authors.
- **Secure aggregation (as stated, §5.1 + T-5):** SA hides individual contributions and defends **passive** servers, but "does not inherently defend against all types of privacy attacks" and can be **broken by active malicious / Sybil servers** that craft model updates to isolate a single client's contribution (refs [20, 59, 65], [7]); even Distributed DP can fail if the server violates protocol assumptions.
- **DP (as stated, §5.1 + T-5):** DP provides formal, composition-based guarantees that extend to the final model, but **strong-enough noise significantly compromises model utility**, and server-side DP "requires a trusted (passive) server and ideal sampling conditions" (ref [55]); client-side DP (ref [82]) "may be weakened by tailored GIAs" ([17, 30, 99]). No numeric accuracy-vs-epsilon table is given by the authors.

## Threat-model taxonomy (KNOWN, Table 1 + §3)
Three axes: adversary *goal*, *capability*, *knowledge*. Capability splits into **honest-but-curious (passive, follows protocol)** vs **actively malicious (modifies global model, or injects Sybil/colluding clients)**. Eight ordered threat models (A–H), decreasing real-world applicability:
- **A Eavesdropper** (only standard FL info) — most realistic (3★).
- **B Informed Eavesdropper** (aux knowledge, e.g. total variation regularization).
- **C Parameter-Aware Eavesdropper** (knows local LR, epochs, batch size; tied to FedAVG).
- **D Data-Enhanced Eavesdropper** (has a surrogate/public dataset).
- **E Statistical-Informed Eavesdropper** (knows BN/data-distribution stats).
- **F Active Manipulator** (alters model architecture; more effective but more detectable, 1★).
- **G Data-Enhanced Manipulator** (manipulates + surrogate dataset).
- **H Active Client Manipulator** (injects Sybil clients at chosen rounds; most impactful, least realistic).
Also distinguishes **single-client/unaggregated gradients** (most attacks, A–E) vs **aggregated updates / FedAVG** (harder; the weight-difference Δ is attacked). Takeaway T-1: targeted GIAs by malicious/colluding clients against a specific victim are "largely underexplored."

## Key findings for us (KNOWN unless tagged)
- **(a) Modality/dimensionality — yes, the field is image-centric.** §7 opens with "Exploring GIAs in image domain *beyond* image classification" and notes research is "heavily focused on image classification tasks (see Table 2)." Table 2 contains no non-image modality. **KNOWN: low-dimensional / non-image (incl. sensor/time-series) GIA behaviour is not covered by the surveyed literature.** (INFERENCE: this directly supports our "tabular/time-series sensor data is under-studied" premise.)
- **(b) SA + small N still leaks — conditionally yes.** SA defends passive servers but is explicitly *insufficient* against active/Sybil servers that isolate a client; with few clients an active server can more easily single out one contribution (refs [20,59,65,7]). KNOWN: SA alone is not a complete GIA defence. INFERENCE (ours, not quantified by the paper): the small-N regime typical of IoT fleets is exactly where SA's isolation resistance is weakest, and the SoK gives no numbers on the N at which leakage returns — an opening, not a closed result.
- **(c) DP cost/effectiveness.** DP is the only defence credited with formal guarantees covering the final model (T-5), but at a utility cost that the SoK flags as significant and notes can still be bypassed by tailored GIAs. The paper does **not** quantify the accuracy/energy/communication cost of DP on constrained devices — it states the trade-off qualitatively only. That specific cost curve on an MCU is therefore *open as far as this paper goes*.

## Convenient assumptions across the field (KNOWN, as the SoK reports them)
- **Horizontal FL only**, clients share feature/label space (§2).
- **Central server is the attacker** (external adversaries deemed to need intermediary nodes, out of scope).
- Attacks typically assume **known or recoverable labels** (label inference now a separate module; Table 2 marks ○ = "assumes label knowledge").
- Many attacks assume **small batch sizes** and often **batch size 1** for high fidelity.
- Generative/data-enhanced attacks assume access to a **surrogate / public dataset** matching the task (distribution shift between surrogate and real client data is itself flagged as under-measured, T-3).
- Statistical-informed attacks assume access to **BN statistics**.
- Standard **FedSGD / FedAVG**; modified optimizers (SCAFFOLD, FedProx) are noted as unstudied for GIAs.

## Stated open problems & future work (KNOWN, §7)
Named gaps (paraphrased):
1. **GIAs beyond image classification** — segmentation, instance detection, and other modalities (explicitly medical/privacy-sensitive); whether current attacks even transfer is unknown.
2. **Alternative FL optimizers** (SCAFFOLD, FedProx and other heterogeneity-aware aggregators) — their modified aggregation could create new attack vectors or inherent resistance; unstudied.
3. **Distribution-shift impact** between attacker surrogate data and real client data, especially in domains with scarce public data (healthcare); needs quantification.
4. **Metrics that fail to capture real privacy risk** — need semantic/privacy-specific metrics, not just pixel similarity.
5. **Client-side defences against active/stealthy servers (Threat Models F–H)** — an "urgent need for specialized detection systems" (T-6); current defences overwhelmingly target passive servers A–E.

One short quote (future work): *"Research on GIAs remains heavily focused on image classification tasks ... overlooking other learning tasks such as semantic segmentation and instance detection, widely used in privacy-sensitive medical domains."*

## Load-bearing references (as cited within this SoK)
- **[109] Zhu et al. — Deep Leakage from Gradients (DLG)**: founding optimization-based GIA; first entry in Table 2, basis for the whole field.
- **[21] Geiping et al. — "Inverting Gradients"**: high-resolution reconstruction; one of the Fig. 1 exemplars; anchor optimization attack.
- **[99] Yue et al.**: modern generative/semantic attack that bypasses several gradient-perturbation defences and motivates semantic metrics (Fig. 1 exemplar).
- **[14] Du et al.**: the closest prior survey; the SoK positions itself against it (Table 5) and extends its partial systematization.
- **[55] / [82] DP defences** and **[56] / [9] cryptographic (SA / HE) defences**: the load-bearing defence references behind the DP and SA conclusions in §5.

## Relevance to our project (one honest paragraph)
This SoK is strong supporting evidence for our candidate directions, and importantly it does **not** close them. On **direction 2 (low-dimensional / sensor / time-series data)**: the SoK confirms (KNOWN) that the entire surveyed GIA literature is image-classification-centric — Table 2 has no non-image modality, and "beyond image" is its first-listed open problem. That means the *question* "how much does a gradient leak for low-dimensional IoT sensor/time-series updates, and do image-era attacks even transfer?" is genuinely under-studied, not already answered. On **direction 6 (secure aggregation with small N)**: the SoK confirms (KNOWN) that SA defends only passive servers and is breakable by active/Sybil servers that isolate a client, and that targeted attacks in small/colluding-client settings are "largely underexplored"; it provides **no** quantification of the client-count N at which SA leakage reappears, nor any constrained-device cost. Caveats for honesty: (i) this paper measures nothing itself — all numbers are tabulated from others, so it cannot be cited as a cost source; (ii) "image-centric literature" is a statement about *coverage*, not proof that sensor GIAs are easy or hard — that is exactly our empirical question (CONJECTURE); (iii) our threat-model write-ups should adopt this SoK's A–H taxonomy so reviewers can place our adversary precisely (e.g., small-N IoT with an active server ≈ Threat Model F/H). Net: the SoK supports the *novelty* of directions 2 and 6 and hands us a ready-made taxonomy, but the actual leakage/cost measurements remain for us to produce.

## Possible seeds (direction-miner "moves", one line each — CONJECTURE)
- **Transplant-to-new-domain:** run DLG/Geiping-style GIA on low-dimensional sensor/time-series FL updates instead of images; measure leakage with non-pixel metrics (the gap §7 names first).
- **Measure-the-untested-cost:** produce the DP accuracy-vs-epsilon and SA communication/memory cost curve *on an MCU emulator* (EFR32xG24) that this SoK explicitly does not provide.
- **Stress-test-an-assumption:** drop the "batch size 1 / known labels / matching surrogate" assumptions and quantify how fast leakage decays for realistic IoT batch/label conditions.
- **Close-a-named-gap (small N + active server):** find the client-count N at which SA stops hiding a single IoT client against a Threat-Model-F/H server — a number the SoK says is missing.
- **Defence-reality-check:** test whether a cheap client-side perturbation that survives on an MCU actually blocks a modern (Yue-style) attack, feeding the "client-side defence for F–H" open problem.
