# Towards Lightweight CKKS: On Client Cost Efficiency

## Reading depth
**Abstract + metadata + verified snippets.** The eprint abstract page, BibTeX metadata, and
keywords were read in full. The full PDF (`eprint.iacr.org/2025/720.pdf`) returned **HTTP 403**
and could not be opened, so the experimental section (timings, RAM, security level, hardware) was
**not** read. Numbers below are limited to what the abstract states verbatim. Fields that would
require the PDF body are marked "not available from abstract."

## Verified identity (KNOWN)
- **ePrint ID:** IACR Cryptology ePrint Archive, Paper **2025/720** (confirmed via BibTeX).
- **Exact title:** "Towards Lightweight CKKS: On Client Cost Efficiency" (confirmed).
- **Authors:** Jung Hee Cheon (Seoul National University / CryptoLab Inc.), Minsik Kang (Seoul
  National University), Jai Hyun Park (CryptoLab Inc.). (confirmed, matches the candidate's
  "incl. Jung Hee Cheon".)
- **Venue / status:** Published — **ACM ASIACCS 2026** (ePrint says "Published elsewhere. Minor
  revision. ACM ASIACCS 2026"). DOI: 10.1145/3779208.3785268.
- **Year on ePrint:** 2025 (ePrint posting); conference is ASIACCS 2026.
- **Keywords (KNOWN):** CKKS, Homomorphic Encryption, Transmission Key Size, Hierarchical Rotation Key.

Note on the task brief: the brief said "Crypto/Asiacrypt 2025?" — that is **incorrect**. Verified
venue is **ASIACCS 2026** (a different conference from Asiacrypt). Also the brief's harvested range
"285-609 MB" is correct but understates the split: KG+ is **325-609 MB**, BTS+ is **285 MB**.

## Core claim (KNOWN, 1-3 sentences)
The paper reduces the client-side **FHE key transmission size** in CKKS by introducing a "ring
switching technique for keys": the client generates and sends small **transmission keys** with
size-optimal parameters, and the server regenerates **evaluation keys** with computation-optimal
parameters, so computation efficiency is not sacrificed (for KG+). Two systems: **KG+** reduces key
size with no loss in homomorphic-computation efficiency vs. the prior state-of-the-art (Lee-Lee-Kim-No,
Asiacrypt 2023); **BTS+** shrinks keys further at the cost of coarser computation granularity.
The cost reduced is specifically **key transmission / communication**, not key-generation time or
encryption time (those are not headlined in the abstract).

## Platform & what was ACTUALLY measured (KNOWN vs gap)
KNOWN concrete numbers, all from the abstract, all for **CKKS with ring dimension N = 2^16**:

| Task | Metric | KG+ | BTS+ | vs. baseline (Lee-Lee-Kim-No, Asiacrypt 2023) |
|---|---|---|---|---|
| CKKS bootstrapping | transmission key size | **325-609 MB** | **285 MB** | KG+ 3.09-4.37x smaller; BTS+ 3.51-9.30x smaller |
| Secure ResNet-20 inference | transmission key size | **325-609 MB** | **285 MB** | KG+ 3.95-5.73x smaller; BTS+ 4.53-12.25x smaller |

- The headline metric is **key size (communication)**, in MB. The motivation sentence notes the
  baseline problem: a client "should generate and transmit **several gigabytes** of FHE keys."
- **Key-generation time, encryption time, and RAM: not available from the abstract** (PDF 403-blocked).
- **Security level (bits): not available from the abstract.** Not stated on the eprint page.
- **Hardware / embedded measurement (important):** The abstract motivates the work with "mobile and
  IoT devices," but reports **no measurement on any actual IoT or constrained device**. These are
  key-size (communication-cost) evaluations of the *schemes*, computed analytically/on standard FHE
  parameters. (INFERENCE: consistent with a server/PC-style evaluation; the abstract never claims an
  on-device benchmark. The actual benchmark hardware is in the unread PDF.)

## Threat model
**N/A — this is an efficiency / key-management paper.** No adversary model is in the abstract. The
implicit setting is the standard FHE delegation one (semi-honest server performing computation on
client ciphertexts), but security claims are not made in the abstract and the full security analysis
is in the unread PDF. The contribution is communication cost, not a new privacy/security guarantee.

## Convenient assumptions (KNOWN where stated, else INFERENCE)
- **Ring dimension N = 2^16** for all reported numbers (KNOWN — stated in abstract). This is a large
  parameter typical of CKKS bootstrapping.
- **Security level:** not stated in abstract (ASSUMPTION: likely ~128-bit as standard for these
  parameters, but this must be checked in the PDF — do not cite).
- **Client/server split (KNOWN):** the whole design *assumes a powerful server* that regenerates
  computation-optimal evaluation keys from the client's transmission keys. Heavy work (bootstrapping,
  ResNet-20 inference) is server-side; the client's job is key generation + transmission + encryption.
- Workloads evaluated are those of Lee-Lee-Kim-No: **CKKS bootstrapping** and **secure ResNet-20
  inference** (image-classification CNN). So the data model is effectively image/CNN inference, not
  arbitrary IoT sensor streams.

## Stated limitations & future work
Not available from the abstract (the limitations/conclusion are in the unread PDF). The one explicitly
stated trade-off, quoted from the abstract:

> "The BTS⁺ system further reduces the key size at the expense of granularity in homomorphic computation."

(So BTS+'s smaller keys come with coarser computation granularity — a design limitation acknowledged
up front. Other limitations/future work: not verified.)

## Load-bearing references (KNOWN / INFERENCE)
1. **Lee, Lee, Kim, No — Asiacrypt 2023** — the state-of-the-art key-management baseline this paper
   beats; both headline comparisons are against it (KNOWN, named in abstract). This is the single most
   load-bearing reference.
2. **CKKS** (Cheon-Kim-Kim-Song, Asiacrypt 2017) — the approximate-arithmetic FHE scheme the entire
   work optimizes (INFERENCE: not seen in abstract text but necessarily cited; scheme named throughout).
3. **CKKS bootstrapping** (Cheon-Han-Kim-Kim-Song, Eurocrypt 2018 line) — the bootstrapping workload
   is one of the two evaluated tasks (INFERENCE from "CKKS bootstrapping").
4. **Secure ResNet-20 inference on CKKS** (e.g., Lee et al. 2022 line) — the second evaluated task;
   rotation-key-heavy CNN inference under FHE (INFERENCE; "Rotation Key Reduction for Client-Server
   Systems of DNN on FHE," eprint 2022/532, appeared adjacent in search and is plausibly cited).
5. **Hierarchical Rotation Key** techniques — called out as a keyword, central to the key-size
   reduction (KNOWN keyword; specific citation in unread PDF).

NOTE: SEAL-Embedded was NOT confirmed as cited. The brief asked to flag it "if cited" — I could not
verify it from the abstract/metadata, so treat it as **not confirmed**.

## Relevance to our project (honest, order-of-magnitude)
This paper is the clearest possible confirmation of the **CKKS-on-MCU gap that motivates
transciphering**, not a refutation of it. Even the best "lightweight" result here is a **285 MB**
transmission key (BTS+), with KG+ at **325-609 MB**. Our target device, the **EFR32xG24**, has on
the order of **256 KB of RAM** (and roughly ~1.5 MB flash). That is an order-of-magnitude gap of
about **10^3** on key *size* alone: 285 MB is roughly **1,100x** the entire RAM of the MCU and still
~190x its flash — the client could not even store one transmission key, let alone generate it.
"Lightweight CKKS" here means "gigabytes down to hundreds of megabytes on a server/mobile-class
client," which is a real and useful result for phones, but it does **not** bring CKKS key handling
into the hundreds-of-KB regime of a Cortex-M microcontroller. (INFERENCE: this is exactly the
argument for **transciphering / symmetric-cipher-to-FHE** on IoT: the constrained client runs a
tiny symmetric encryption, and the server does the costly CKKS key generation and homomorphic
transformation. This paper reduces client cost but keeps it firmly server/mobile-class.)

For our direction-mining: this paper is a strong **"baseline / motivation" citation** that
quantifies why native FHE client keys are infeasible on a Class-1/2 IoT MCU, sharpening the case for
measuring transciphering cost on the EFR32xG24. Caveat for our own write-ups: the MCU RAM/flash
figures above should be re-verified against the EFR32xG24 datasheet before citing (ASSUMPTION), and
the comparison is key-*size* vs device-*memory*, not a measured on-device benchmark from this paper.

## Possible seeds (direction-miner moves, one line each)
- **Push-to-the-edge / role-split check:** measure whether a transciphering client (AES/ASCON on the
  MCU) beats even BTS+'s 285 MB client key — quantify the client-side delta on the EFR32xG24.
- **Baseline-gap framing:** use this paper's 285-609 MB numbers as the concrete "native CKKS client"
  cost to contrast against transciphering key/ciphertext sizes in our threat-model table.
- **Metric-swap:** the paper optimizes key *transmission bytes*; our IoT framing should add *energy
  and radio cost per byte* on the MCU — a metric this paper omits (no radio cost assumed).
- **Parameter-regime probe:** all numbers are N=2^16; ask what happens to the client/server split at
  smaller N (lower depth) that an IoT inference task might actually need.
