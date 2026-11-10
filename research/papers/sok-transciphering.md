# Paper note: SoK — FHE-Friendly Symmetric Ciphers and Transciphering

## Verified identity (KNOWN)
- **Title:** "SoK: FHE-Friendly Symmetric Ciphers and Transciphering"
- **Authors:** Chao Niu, Benqiang Wei, Zhicong Huang, Zhaomin Yang, Cheng Hong, Meiqin Wang, Tao Wei (Ant Group; Shandong University).
- **ePrint ID:** IACR ePrint **2025/669** — CONFIRMED (title, authors, ID all match the ePrint record).
- **Venue:** IACR Transactions on Cryptographic Hardware and Embedded Systems (**TCHES**) **2025, Issue 3, pp. 583–613**. DOI 10.46586/tches.v2025.i3.583-613. ePrint record states "A minor revision of an IACR publication in TCHES 2025." Status: **published / accepted** (not ePrint-only).
- **Companion artifact:** open-source repo `AntCPLab/awesome-transciphering` (GitHub), referenced as the paper's code.
- No mismatch found. Candidate URL and ID are correct.

## Reading depth
**Skimmed / abstract + extensive snippets.** The verified abstract and all bibliographic metadata were read directly from the ePrint record and an OpenAlex metadata record. The full PDF body could NOT be opened: the ePrint PDF, the TCHES PDF, and ResearchGate all returned HTTP 403 to the fetch tool, and the companion repo README is thin. Cipher-family details, benchmark numbers, platform, and the open-problems framing below come from converging web-search snippets of the paper plus downstream papers that cite it (HHEML, arXiv 2510.20243). **Numbers below marked (snippet) were NOT read from the primary PDF and should be re-checked against the PDF before citing in our own write-up.**

## Core claim (1–3 sentences)
FHE suffers large plaintext-to-ciphertext expansion, which is costly when a constrained client must upload encrypted data. Transciphering fixes this: the client sends compact symmetric ciphertext, and the server homomorphically evaluates the symmetric decryption circuit to obtain FHE ciphertext without ever decrypting. This SoK systematizes 20+ FHE-friendly symmetric ciphers and transciphering methods, classifies them, and benchmarks feasible (cipher × transciphering-framework × FHE-scheme) combinations on security, efficiency, and compatibility.

## Platform & what was ACTUALLY measured
This is a SoK, so the contribution is taxonomy + a unified re-benchmark of prior ciphers.

**Cipher families covered (KNOWN that they are in-scope; grouping is partly inference from snippets):**
- **Bit-based / Boolean (target: TFHE, and BGV/BFV over F_2):** LowMC, Kreyvium, Rasta, Dasta, Agrasta, FASTA, FiLIP (and FLIP lineage), **Transistor** (recent TFHE-friendly stream cipher).
- **Word-based / F_p arithmetic (target: BGV/BFV, and CKKS via the RtF/"real-to-finite-field" framework):** Masta, Pasta, HERA, Rubato, Chaghri, and the HERA/Rubato-style "RtF framework" ciphers for approximate (CKKS) data.
- **TFHE-native over Z_q / group-filter designs:** Elisabeth (group filter permutator, extends FLIP/FiLIP), plus patched variants named in snippets: Elisabeth-b, Gabriel, Margrethe.
- AES-CTR is included as the standard-cipher baseline (via a "BtR" framework over CKKS, per the repo).

**FHE schemes targeted:** BGV, BFV/FV, CKKS, and TFHE/CGGI — all four appear. The repo structure confirms three evaluation tracks: RtF framework over FV, BtR framework over CKKS, and a CGGI (TFHE) transciphering re-evaluation.

**Comparative metrics tabulated (as stated; all (snippet) unless noted):**
- Homomorphic throughput measured in KB of message transciphered per second, and latency in ms, single-threaded. (snippet)
- Example figures from snippets (RE-VERIFY against PDF tables): HERA(5,16,16,28-bit) ≈ 1.38×10^5 KB/s throughput, ~25.95 ms latency; Rubato(5,16,12,26-bit) ≈ 0.86×10^5 KB/s, ~28.94 ms. Masta and Pasta reported as less efficient than HERA/Rubato because of many plaintext×ciphertext ops / random-matrix multiplications. Rubato beats HERA by lowering multiplicative depth. Some patched ciphers cost ≥2× Elisabeth's TFHE evaluation. (snippet)
- Downstream citers say the SoK benchmarks Pasta, Masta, Elisabeth under ML workloads such as ResNet-20, comparing depth, latency, and energy. (snippet, via HHEML / arXiv 2510.20243)
- Metrics systematized per cipher: security level, multiplicative depth, expansion ratio, and homomorphic eval cost. (KNOWN from abstract framing; exact table numbers not read.)

**Platform — IMPORTANT for us:** benchmarks were run on an **Intel Xeon CPU @ 2.70 GHz, single-threaded** (server-class). (snippet) **No measurement is on a constrained/embedded/IoT MCU.** The paper is a desktop/server re-benchmark of homomorphic evaluation cost. Client-side symmetric-encryption cost is discussed as a *trade-off dimension* (e.g., the remark that Rubato is cheap to transcipher server-side but has *poor client-side performance*), but there is no evidence of actual MCU/embedded cycle or energy measurement.

## Threat model (what the transciphering layer assumes)
- **Setting:** outsourced computation to a single honest-but-curious server (cloud). Client holds the symmetric key and the FHE secret key; server holds the FHE evaluation/public key and the client's FHE-encrypted symmetric key.
- **Asset protected:** confidentiality of the client's raw data (plaintext) and the symmetric key against the server.
- **Mechanism:** data confidentiality reduces to the FHE scheme's IND-CPA security plus the symmetric cipher's security as a keystream/PRF under homomorphic evaluation; the server learns only ciphertexts.
- **Out of scope (inference):** malicious/active server, integrity/verifiability of the computation, side channels on the client device, and the multi-server non-collusion assumptions used by MPC alternatives.

## Convenient assumptions
- Costs that matter are **server-side homomorphic latency/throughput**; client encryption is treated as cheap/compact (the whole motivation) and is not independently measured on constrained hardware. (ASSUMPTION baked into the SoK framing.)
- Honest-but-curious single server.
- Benchmarks are representative of deployment despite being single-threaded on one Xeon part; real deployments would use multi-threading/accelerators.
- Security of recently-proposed ciphers is taken at proposed parameter sets (the SoK flags that several have been attacked / patched — see below).

## Stated open problems & future work (key for us)
Paraphrased gaps the SoK names (CONJECTURE that these are the main ones, assembled from snippets — confirm in the PDF's conclusion):
- **Security is fragile and moving.** Several FHE-friendly ciphers have been broken or weakened and then patched (Elisabeth → Elisabeth-b/Gabriel/Margrethe). Designing ciphers that are simultaneously secure, standardizable, and cheap to evaluate homomorphically remains open.
- **No single winner.** Best cipher depends on the FHE scheme and data type (bit vs. word vs. real/CKKS); cross-scheme, apples-to-apples comparison is still hard — part of why the SoK builds a unified benchmark.
- **Server-side cost still high** relative to native symmetric crypto; closing the gap (lower multiplicative depth, better keystream evaluation) is ongoing.
- **Client-side / deployment cost is under-studied** (the SoK notes client-side vs. server-side trade-offs, e.g. Rubato, but does not quantify them on constrained devices).
One short quote (from the abstract, verified): transciphering lets one "first encrypt data with a space-efficient symmetric cipher, then convert symmetric ciphertext to FHE ciphertext without decryption."

## Load-bearing references (seminal ciphers/schemes the SoK builds on)
1. **LowMC** (Albrecht et al., EUROCRYPT 2015) — first MPC/FHE-friendly low-multiplicative-complexity block cipher.
2. **Rasta / Dasta** (Dobraunig et al., CRYPTO 2018; Hebborn–Leander) — low-depth stream-cipher paradigm, parent of the "…asta" family.
3. **Pasta** (Dobraunig et al., TCHES 2023) — F_p HHE cipher and the RtF-style integration with BGV/BFV.
4. **HERA / Rubato** (RtF framework, EUROCRYPT 2021 / 2022) — real-number / CKKS transciphering.
5. **Elisabeth** (Cosseron et al., ASIACRYPT 2022) — group filter permutator, flagship TFHE-friendly stream cipher (and its subsequent cryptanalysis/patches).

## Relevance to our project (one honest paragraph)
This SoK is directly on-topic for the PQC/FHE strand but has a **server-side blind spot that is exactly our opening**. It systematizes which symmetric cipher pairs best with which FHE scheme and re-benchmarks homomorphic *evaluation* cost — but every number is server-side on an Intel Xeon, single-threaded, with **no constrained/MCU measurement** of the *client* symmetric-encryption step, even though the whole point of transciphering is to make the client's upload cheap. The paper even acknowledges client-vs-server trade-offs (Rubato cheap to transcipher but expensive client-side) without quantifying them on embedded hardware. For our EFR32xG24 phase-2 plan, the clean, defensible question is: **on a real Cortex-M33 IoT MCU, what is the actual cost (cycles, RAM, energy, radio bytes) of the client-side keystream generation for the SoK's main cipher families (e.g., Pasta/Masta vs. HERA/Rubato vs. Elisabeth/Transistor), and does the server-side ranking invert on the client?** The SoK gives us the taxonomy, parameter sets, and server-side baselines to anchor that comparison. On "cheapest on the client," the honest answer from this paper alone is unresolved: it flags Rubato as client-side poor, suggests bit-based TFHE ciphers (Elisabeth/Transistor) as lightweight for keystream, and treats compactness as the selling point — but it does not measure client cost, so the cheapest-on-client cipher is an open empirical question the SoK leaves for us.

## Possible seeds (direction-miner moves)
- **Measure-what-they-assumed:** SoK measured server HE cost; we measure the *client* MCU cost it assumed away. One line → novel, software-only, continues to DRP.
- **Invert-the-metric:** re-rank the SoK's cipher families by client-side cycles/energy on Cortex-M33; test whether the server-side winner loses on the client.
- **Stress the threat model:** add the radio/communication cost (expansion ratio × upload energy) that server benchmarks ignore for a battery IoT node.
- **Follow the patch trail:** Elisabeth-b/Gabriel/Margrethe security churn → quantify the client cost of the *secure* parameter sets, not the broken originals.

---
*Sources consulted:*
- https://eprint.iacr.org/2025/669 (abstract, authors, status — verified)
- https://tches.iacr.org/index.php/TCHES/article/view/12228 (TCHES 2025 Issue 3, pp. 583-613)
- https://api.openalex.org/works/doi:10.46586%2FTCHES.V2025.I3.583-613 (DOI / volume / pages)
- https://github.com/AntCPLab/awesome-transciphering (companion code; RtF/FV, BtR/CKKS, CGGI/TFHE tracks)
- https://arxiv.org/html/2510.20243v1 (HHEML — cites SoK's ResNet-20 benchmarking; context only)
- Benchmark figures (HERA/Rubato throughput, Xeon 2.70 GHz, single-thread) from web-search snippets of the paper — NOT read from primary PDF; re-verify.

## Full-text check (2026-11-10, local PDF `2025-669.pdf`, 32 pages)
Reading depth is now **full text**. The full text corrects two statements above:
1. **The SoK does measure client-side cost**, in Table 4 (§5.4, p. 20): latency in CPU cycles and throughput in cycles/byte for 23 cipher settings. The settings include Trivium, Kreyvium, FiLIP-DSM/TX, Margrethe, LowMC, Rasta, Dasta, FRAST, Masta, Pasta, Pastav2, HERA, Rubato and AES-SW (kokke/tiny-AES-c). These are measured on the same Xeon as the server benchmarks, not on an MCU. Lines 43 and 53 above ("client encryption … not independently measured", "does not quantify them") are therefore wrong for a PC client. They hold only for constrained hardware.
2. **Latency and throughput were swapped** in the snippet figures above. Table 2 (p. 17) gives HERA(5,16,16,28-bit) **latency 1.38×10⁵ ms, throughput 25.95 KB/s**, and Rubato(5,16,12,26-bit) **0.86×10⁵ ms, 28.94 KB/s**.

Client-side ordering in Table 4 (cycles for one block, lower is better):
- **Cheapest:** AES-SW 1.86×10³; HERA-5 2.86×10⁴; Trivium-12 4.89×10⁴; Kreyvium-13 5.27×10⁴.
- **Middle:** FRAST 1.47×10⁵; Pastav2-4 1.92×10⁵; Masta-6 2.25×10⁵; Pasta-4 4.70×10⁵ … Dasta-6 7.76×10⁵.
- **Most expensive:** Rubato-5/3/2 1.22–1.48×10⁷; FiLIP-DSM 2.56×10⁷; Rasta-5 1.81×10⁸; LowMC 1.73×10⁸.

Section 6 observation (2) says outright that Rubato's server-side advantage comes with poor client-side cost, and that "a client with constrained communication resources is often also limited in computational capabilities, such as an IoT device". **Consequence for D3:** "the server-optimal cipher is not the client-optimal one" is KNOWN, on a Xeon. D3's remaining question is whether the client ranking changes between a Xeon and a Cortex-M33 (a 32-bit core with no 64-bit multiply/divide, Keccak-based PRNG cost, a few hundred kB of RAM). Platform detail: the paper names an Intel Xeon but does not state its model in §5. The IAIK framework that several of these ciphers come from builds Keccak with AVX2 by default (its `util/keccak/CMakeLists.txt`). Table 4 note: Pasta-3 is listed as (3,128,64), while Table 2 lists it as (3,256,128). The paper is internally inconsistent here.
