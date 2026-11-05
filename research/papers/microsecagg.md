# MicroSecAgg: Streamlined Single-Server Secure Aggregation

## Verified identity
- **Title (CORRECTED):** "MicroSecAgg: Streamlined Single-Server Secure Aggregation."
  Note: the candidate task used the subtitle "...for Federated Learning" — that is **not** the actual
  title. The paper covers three aggregation scenarios, FL being only the third.
- **Authors:** Yue Guo (J.P. Morgan AI Research & AlgoCRYPT CoE), Antigoni Polychroniadou (J.P. Morgan
  AI Research & AlgoCRYPT CoE), Elaine Shi (Carnegie Mellon University), David Byrd (Bowdoin College),
  Tucker Balch (J.P. Morgan AI Research).
- **Venue (CORRECTED):** Proceedings on Privacy Enhancing Technologies (PoPETs) **2024, Issue 3**,
  pages **246–275** — not Issue 4 as the candidate text suggested. Paper no. popets-2024-0077.
- **DOI:** https://doi.org/10.56553/popets-2024-0077
- **ePrint:** IACR ePrint 2022/714 (https://eprint.iacr.org/2022/714). Confirmed same paper.
- **Year:** 2024 (© 2024, owner/author).
- Status: **Published** (PoPETs 2024.3) with an earlier ePrint version.

## Reading depth
**Full-read** of the PDF (PoPETs camera-ready). Read in detail: abstract, intro (§1), results (§1.1),
approach (§2), preliminaries/primitives (§3.1), protocol figures (§setup/aggregation diagrams),
comparison Tables 1–2, performance (§6 + Figures 1–5), conclusion (§7), and appendices A (primitives,
class groups, 2ODH), C/D (security proofs, threshold conditions), E (performance analysis). Security
proof algebra (hybrids) skimmed, not line-checked.

## Core claim (KNOWN)
MicroSecAgg is a single-server secure-aggregation framework for **multiple successive aggregation rounds**
over a dynamic (drop-out-prone) client pool. Its headline streamlining vs Bonawitz et al. SecAgg
(BIK+17) and successors: a **one-time Setup phase** whose secret-sharing material is **reused across all
later aggregation iterations**, which (a) cuts the per-aggregation round complexity to **3 rounds**
(semi-honest) vs 5–6 in prior single-server work, (b) reduces per-user computation to **logarithmic in n**
(vs linearithmic/quadratic in BIK+17 / Ma et al. / Bell et al.) and per-user communication by a factor
of n vs Ma et al., while server computation is **linear in n** (vs quadratic in BIK+17). Three protocol
variants: MicroSecAgg_DL (small input domain, DDH cyclic group), MicroSecAgg_gDL (neighbourhood/grouping
version, cheaper comms), MicroSecAgg_CL (large input domain, class groups of unknown order to avoid
discrete-log recovery).

## Platform & what was ACTUALLY measured
KNOWN (from §6 and Figures):
- **Implementation language:** Python. Built on top of **ABIDES** [15], a discrete-event simulation
  framework modified to simulate multi-iteration aggregation protocols.
- **Hardware:** All experiments run on an **AWS EC2 r5.xlarge** instance, 4 × 3.1 GHz Intel Xeon Platinum
  8000-series CPUs, **32 GB** memory. **Single-threaded** for both users and server. Users and server are
  all simulated on this one machine.
- **Scale tested:** up to **1024 users** (also 500, 1000 reported at various points);
  group/neighbourhood size 100 (and 64/128 in some figures).
- **Parameters:** aggregation-result bit-length ℓ = 20 bits (also 16/20/24 in Fig. 3); input/vector
  length 1 for most comparisons, up to 100 for class-group comparison; class group library [13] (BICYCL)
  instantiated with **256-bit primes**; works over a **32-bit range** for class-group discrete log.
- **Rounds:** Setup phase 5 rounds (MicroSecAgg_gDL, Fig. 7); Aggregation phase **3 rounds** semi-honest
  (Fig. 8). Table 1 gives per-round, per-user comms in bits; Table 2 gives asymptotic total cost per
  aggregation (malicious security).
- **Concrete sample numbers (KNOWN):** with 500 participants, MicroSecAgg_DL is ~100× faster in
  computation than BBG+20; MicroSecAgg_gDL >5× faster than BBG+20 (connectivity 100, 500 users);
  MicroSecAgg_CL at 1024 users / neighbourhood 64 / 32-bit input is ~6× faster than BBG+20 and ~42×
  faster than Flamingo (decryptor side) on user side. Brute-force 20-bit discrete log takes **1.14 s**;
  with class groups the same recovery takes **673 ns**. WAN experiment (Fig. 5) assumes server in NYC,
  farthest user in Sydney, ~200 ms ping, 100 Mbps bandwidth.
- **FL accuracy check:** logistic regression on the UCI **Census/Adult income** dataset [27]; secure
  version and plaintext FL both reach accuracy **0.81** (i.e., secure aggregation does not hurt accuracy).

CRITICAL HONESTY POINT — **no constrained device / MCU was used.** Every cost was measured on the AWS
server-class machine above. The authors themselves flag the gap: they note that in practice "the device
on the user side is usually with more limited resource while the server usually runs on more powerful
machines than the AWS machine we use in the experiments." So the single AWS node stands in for BOTH the
client and the server; client-side cost on an actual IoT/MCU is **not measured and not estimated** here.

## Threat model (KNOWN)
- **Single server** holding all inputs; clients talk only to the server (no direct client-to-client
  channels), communication relayed through the server. This is the defining single-server setting.
- **Two security levels provided:**
  - *Semi-honest (honest-but-curious) server:* the 3-round aggregation / lighter protocol.
  - *Malicious server:* extra rounds + online-set signing (the "blue/underlined" parts in Table 1 and
    Algorithm 3), using signatures so the server cannot feed different client subsets inconsistent
    "online sets."
- **Collusion / corruption threshold (KNOWN, from Theorems 1.1–1.2):**
  - MicroSecAgg_DL guarantees privacy if the semi-honest (resp. malicious) adversary corrupts less than
    **1/2 (resp. 1/3)** fraction of users, and correctness if fewer than **1/2 (resp. 1/3)** drop out.
  - MicroSecAgg_gDL / _CL use parameters γ (corruption fraction) and δ (dropout fraction) with
    **γ + 2δ < 1**; privacy if corruption < **1/2 (resp. 1/3)**, correctness if dropout < δ.
    The γ+2δ<1 condition is needed because users are split into small groups and each group must not have
    too many offline/corrupt users.
- **Static adversary** assumed (same as Flamingo); neighbourhoods are fixed in the Setup phase.
- **What is protected:** each individual client input x_i is hidden from the server (and from corrupt
  clients); the server learns only the sum Σ x_i of online users per iteration — and crucially not any
  single client's input in any iteration k, even though the setup is reused.
- **Dropout model:** clients may drop out / be slow in any iteration and **come back online later**
  (dynamic pool); Shamir t-out-of-n sharing lets remaining users help reconstruct masks of dropped users.
- **Out of scope:** Sybil attacks — explicitly acknowledged as unaddressed ("Only the recent work of
  David Byrd et al. [16] ... provides an aggregation solution to limit such attacks"); input validation /
  poisoning (defended by ACORN [9], Armadillo, not here); robustness against a server that aborts.

## Convenient assumptions (ASSUMPTION / KNOWN)
- **PKI:** clients hold signing keys and all users' verification keys (public-key infrastructure) in the
  malicious variant — Algorithm 3 input explicitly lists "public key infrastructure."
- **One-time trusted/honest setup material:** a class-group tuple (B, N, p, s, g, f, G, F) is "honestly
  generated" by (cl.gen, cl.solve) and shared; the reusable Setup phase is assumed to run once correctly.
- **Cryptographic assumptions:** DDH in Z*_p (safe prime p = 2q+1) for the DL/gDL versions; DDH group
  with an easy-DL subgroup (class groups of unknown order, Castagnos–Laguillaumie framework) for the CL
  version; IND-CCA2 authenticated encryption; 2ODH (Two-Oracle Diffie–Hellman) for the Setup channel;
  Shamir t-out-of-n secret sharing (including an integer-interval variant for the CL version).
- Honest-majority-style thresholds (above). Number of clients assumed moderate-to-large (hundreds–~1024
  in experiments); grouping assumes n large enough for the hypergeometric tail bounds to hold.
- All users assumed online during most benchmark runs ("We assume all users are online during the whole
  process") except the dedicated dropout-rate experiments.

## Stated limitations & future work (KNOWN, paraphrased)
- Client-side cost scales with input/vector length L because the protocol exponentiates / reconstructs per
  vector index; the authors note it "shine[s]" most for moderately sized models with fewer weights, and
  gets heavier as weight vectors grow.
- CL (class-group) version avoids discrete-log recovery but needs secret sharing over large integers
  (containing n!), which current hardware supports inefficiently — so it suits the group version.
- Sybil resistance and input validation are not addressed.
- Future work (one short quote): "we are interested in using our secure aggregation protocol as a building
  tool for the more complex setting of vertical federated learning" and in further compressing sparse
  model updates.

## Load-bearing references (3–5)
1. **BIK+17 — Bonawitz et al.**, "Practical Secure Aggregation for Privacy-Preserving Machine Learning,"
   CCS 2017 [ref 11]. The original single-server SecAgg baseline; MicroSecAgg's masking idea revisits it
   (§2.1) and all comparisons benchmark against it.
2. **BBG+20 — Bell, Gascón, Lepoint, et al.**, "Secure single-server aggregation with (poly)logarithmic
   overhead," CCS 2020 [ref 10]. Introduces neighbourhood/grouping; MicroSecAgg_gDL borrows the grouping
   and the γ+2δ-style threshold analysis.
3. **Flamingo — Ma, Woods, Shi, Polychroniadou, Rabin**, S&P 2023 [ref 37]. Multi-round single-server
   SecAgg with reusable setup; direct competitor; MicroSecAgg claims lower round count and improved comms.
4. **Castagnos, Laguillaumie, Tucker — class-group / DDH-with-easy-DL framework** [refs 19, 20, 21]
   (+ BICYCL implementation [13]). The algebraic engine behind MicroSecAgg_CL for large input domains.
5. **Shamir**, "How to Share a Secret," CACM 1979 [ref 43]. The t-out-of-n sharing that provides dropout
   tolerance and mask reconstruction throughout.

## Relevance to our project (one honest paragraph)
This is exactly the **classical single-server secure-aggregation baseline** a "post-quantum secure
aggregation on MCUs" study would start from. The classical, quantum-breakable piece is the
**Diffie–Hellman key agreement in the Setup phase**: §3.1 defines a textbook DH key exchange
(KA.setup / KA.gen / KA.agree, with s_{u,v} = H((g^{x_v})^{x_u})) over the DDH group Z*_p, and this
pairwise DH-derived symmetric key ek_{i,j} is what every pair of clients uses to encrypt the Shamir
shares they exchange (via the server) in Setup Rounds 1–3. The per-pair mask seeds mk_{i,j} and the
masking group elements H(k)^{X_i} also live in the same DDH group, so the whole privacy argument rests
on DDH / discrete log — both broken by a quantum adversary. The natural swap for our DRP is to replace
the DH key-agreement step with a **lattice KEM (e.g., ML-KEM/Kyber)** to establish ek_{i,j}, then
re-cost: KEM keygen/encaps/decaps cycles and the larger public-key/ciphertext sizes per client pair, on
the EFR32xG24. Honest caveat: this is NOT a drop-in. The MicroSecAgg masking (H(k)^{Σ r_i}, discrete-log
recovery of the sum, class-group arithmetic) is itself DDH/DL-structured and is what makes the sum
homomorphically recoverable — a KEM only gives you the symmetric channel for share exchange, not the
additively-homomorphic masking. So "PQ MicroSecAgg" cleanly covers the key-establishment channel; making
the *mask* post-quantum (e.g., LWE/LWR-based masking as in LERNA [31] or Liu et al.) is a separate, harder
question. For our software-first phase, the clean, well-scoped deliverable is: re-instantiate the Setup
channel with a lattice KEM and measure the client-side cost delta (cycles, RAM, bytes) — precisely the
cost MicroSecAgg itself never measured on a constrained device.

## Possible seeds (direction-miner moves)
- **Swap-the-primitive:** replace Setup-phase DH key agreement with ML-KEM/Kyber; re-cost client-side on
  EFR32xG24 (cycles, flash/RAM, handshake bytes). One line: the cleanest PQ-migration experiment here.
- **Re-cost-on-MCU:** every number in this paper is AWS-only; port the client role to the xG24 emulator
  and report the gap between server-class and MCU client cost.
- **Stress-the-assumption:** MicroSecAgg assumes moderate n and honest setup; measure how group size and
  dropout rate δ trade off against MCU memory when n is small (realistic IoT fleet) rather than ~1024.
- **Measure-the-leakage:** pair with the FL gradient-leakage angle — quantify what one masked update
  leaks under this threat model vs adding DP noise, on sensor data.
- **Hybrid-mask question:** flag (not promise) the harder open piece — making the additively-homomorphic
  *mask* itself post-quantum (LWR-based), since KEM-only migration leaves the DL-structured masking.
