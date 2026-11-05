# HHEML: Hybrid Homomorphic Encryption for Privacy-Preserving Machine Learning on Edge

## Verified identity (KNOWN)
- **arXiv ID:** 2510.20243 (confirmed; matches the candidate).
- **Exact title:** "HHEML: Hybrid Homomorphic Encryption for Privacy-Preserving Machine Learning on Edge".
- **Authors:** Yu Hin Chan, Hao Yang, Shiyu Shen, Xingyu Fan, Shengzhe Lyu, Patrick S. Y. Hung, Ray C. C. Cheung.
- **Year / status:** 2025 (submitted 23 Oct 2025). Category cs.CR. **arXiv-only preprint** — no venue/journal stated.
- Sources read: abstract page, full HTML (arxiv.org/html/2510.20243v1).

## Core claim (KNOWN)
HHEML is an FPGA hardware/software co-design that accelerates the *client-side* symmetric-encryption step of a hybrid homomorphic encryption (HHE) PPML pipeline, so a resource-constrained client can hand off ML inference to an untrusted server cheaply. Over an existing FPGA baseline it claims >50x lower client-side encryption latency and ~2x higher hardware throughput.

## Platform & what was ACTUALLY measured (KNOWN unless noted)
- **HE-friendly cipher:** Pasta — specifically the **Pasta-4** core (SHAKE128-based XOF + permutation masking engine). Plain modulus not stated in-text; rounds r=4.
- **FHE scheme:** HHEML reuses the **GuardML** server stack and does *not* name its own FHE scheme in-text. The paper lists BGV/BFV/CKKS/FHEW/TFHE only as background. (INFERENCE, from GuardML paper, Frimpong et al. 2024: GuardML's HHE pairs **Pasta + BFV**, so HHEML's server side is almost certainly **BFV**. Not asserted by HHEML itself.)
- **"Edge" device actually used:** a **Xilinx PYNQ-Z2 FPGA** board with a dual-core **ARM Cortex-A9** processing system. Baseline "Pasta on Edge" and HHEML accelerator run on the PL fabric (client accelerator ~100 MHz; measurements in Table I at 75 MHz; fabric capability cited 650 MHz). **This is an FPGA SoC, NOT a Cortex-M microcontroller and NOT a plain PC.** No Cortex-M / EFR32 / Raspberry Pi / phone anywhere.
- **Client encryption cost (Table I):** MNIST encryption **1,553.4 μs** (HHEML) vs **3,106.7 μs** (Pasta on Edge baseline); 1-round enc **34.3 μs** vs **66.1 μs**. Power **1.505 W** vs **1.2 W**. Resource cost rises: LUTs 34,419 vs 23,736; FFs 18,372 vs 11,132; DSP 64 both; BRAM 2 vs 0.
- **End-to-end client latency (Table III):** HHEML total **6.9 ms** (6.5 ms client-internal comm + 0.4 ms client→server) vs GuardML **356 ms**. The headline ">50x" comes from 356 ms / 6.9 ms (~55x).
- **Throughput (Table II):** two-XOF design = **1.95x** relative to single-XOF (normalized 1.00x). This is the "~2x hardware throughput gain".
- **Bandwidth saved by transciphering:** **not found.** No ciphertext expansion factor or byte-level HHE-vs-raw-FHE size comparison is given; only the 0.4 ms Ethernet time.
- **Server-side homomorphic cost:** "Decomposition" **9,397.5 s**; homomorphic MNIST evaluation **54.9 s**. These are inherited unchanged from the GuardML software baseline — HHEML accelerates only the client. (Note: the paper does not formally define "Decomposition"; inferred to be the server transciphering step converting symmetric ciphertext to FHE form.)
- **Accuracy / ML model:** **not found.** MNIST (28x28) is used as benchmark input, but no model architecture and no inference accuracy are reported.

## Threat model (KNOWN, partial)
- **Asset:** the client's sensitive input data (and its ML inference result).
- **Adversary:** an **untrusted computation server**; the client encrypts locally before sending ciphertexts. Phrasing is consistent with **honest-but-curious** (server computes correctly but may try to learn plaintext). The paper does not formalize collusion, malicious-server, or side-channel adversaries.
- **Out of scope / not addressed:** model confidentiality, integrity of results, side channels (the course's phase-2 interest).

## Convenient assumptions (INFERENCE from what is / isn't stated)
- Server-side FHE cost (thousands of seconds) is treated as an accepted/fixed baseline; the contribution is narrowly the client encrypt step.
- MNIST only; no accuracy reported, so utility is assumed rather than demonstrated.
- "Edge" = an FPGA SoC with programmable logic available — i.e., the client already has a hardware accelerator and ~1.5 W power budget. Not a bare MCU with only an MCU core.
- Communication cost reduced to a single Ethernet latency figure; no radio/wireless energy, no bandwidth/size accounting.

## Stated limitations & future work (KNOWN)
The motivation line for HHE over plain FHE: classical FHE "introduces significant communication and computation overhead on the client side, making it impractical for edge devices." Future work is directed at **multi-core pipelining** and **tighter integration with advanced FHE backends** for more scalability/efficiency. (No explicit discussion of the ~9,400 s server decomposition cost as a limitation.)

## Load-bearing references (KNOWN)
1. Dobraunig et al., 2023 — "Pasta: A case for hybrid homomorphic encryption" (the cipher used).
2. Frimpong et al., 2024 — "GuardML: Efficient Privacy-Preserving ML Services through HHE" (server pipeline reused; Pasta+BFV).
3. Aikata et al., 2025 — "Pasta on Edge: Cryptoprocessor for hybrid homomorphic encryption" (direct FPGA baseline being beaten).
4. Grassi et al., 2024 — "Masta: A new family of symmetric ciphers tailored for HHE".
5. Bouillaguet et al., 2024 — "Elisabeth: Efficient lightweight symmetric cipher for bootstrappable HE".

## Relevance to our project (one honest paragraph)
For our CS 5803 framing, "edge" here is **overstated relative to a genuinely constrained MCU**. The client is a PYNQ-Z2 FPGA SoC (Cortex-A9 + programmable logic at ~1.5 W), and the entire speedup lives in *custom FPGA hardware* for the Pasta XOF/permutation. It is NOT a Cortex-M33 / EFR32xG24-class target, and nothing in the paper is a pure-software MCU cost. That said, the paper is a useful HHE anchor: it confirms the PASTA/HHE → BFV line (via GuardML), gives a concrete client-encryption workload (Pasta-4 encrypting a 28x28 MNIST input), and shows the server cost is dominated by transciphering ("Decomposition", ~9,400 s) — a red flag worth re-examining. The directly re-costable quantity for us is **Pasta-4 symmetric encryption of one MNIST input in software**: we could implement/benchmark Pasta-4 on a Cortex-M33 (cycle counts, flash/RAM, key-expansion cost) and compare against this FPGA number to quantify what a non-FPGA edge client actually pays — i.e., whether HHE is still "practical for edge" without an FPGA. Their missing accuracy, missing bandwidth/expansion numbers, and honest-but-curious-only threat model are all gaps we could fill in software.

## Reading depth
**Full-read** of arXiv HTML v1, including Tables I–III; cross-checked GuardML reference for the FHE scheme (inference, flagged). Underlying numbers not independently reproduced.
