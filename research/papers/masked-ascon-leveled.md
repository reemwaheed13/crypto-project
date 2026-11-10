# Low-Latency Masked Gadgets Robust against Physical Defaults with Application to Ascon

Lane: B

## Citation
- Title: Low-Latency Masked Gadgets Robust against Physical Defaults with Application to Ascon
- Authors: Gaëtan Cassiers, François-Xavier Standaert, Corentin Verhamme (UCLouvain, Crypto Group, ICTEAM). Author list verified on the IACR artifact page, the DIAL repository record and the Zenodo record. The Zenodo record lists the authors in a different order (Verhamme first).
- Venue/status: published in IACR TCHES, Vol. 2024, Issue 3, pp. 603–633. DOI 10.46586/tches.v2024.i3.603-633. Open access (CC BY 4.0).
- Artifact: https://artifacts.iacr.org/tches/2024/a22/ (badge "Results Reproduced").
- ePrint: no ePrint version found (searched, see below).

## Reading depth
**Abstract + snippets + artifact README. NOT full text.**
- The TCHES PDF (tches.iacr.org) is behind an Anubis bot wall and returned "Access Denied".
- The Zenodo copy (records/17453208) downloaded, but as an octet-stream that could not be turned into text.
- The DIAL and ResearchGate copies failed with an error or 403.
- The DOI resolver redirected to an unrelated domain (philosophymindscience.org). That redirect was not followed.
- Everything below comes from the abstract, search-engine snippets of the paper, and the artifact README. Someone must open the PDF by hand before any number from this paper goes into a report.

## Core claim
The paper gives the first single-cycle masked AND gadget (HPC4) that composes trivially and is proven secure against both glitches and transitions (robust probing model, O-PINI). Applied to Ascon in **hardware**, it improves:
- uniformly masked designs;
- leveled designs, where only key derivation (initialization) and tag generation (finalization) are masked;
- "multi-target" designs, which switch between the uniform and leveled modes with no latency overhead.

## Platform
- **Hardware only. There is no software or microcontroller implementation and no Cortex-M number.**
- Verilog designs, synthesized with Cadence Genus 18.10 on the FreePDK45 (45 nm) library. These ASIC synthesis results feed Tables 3, 4, 5, 6, 7, 8 and Figs. 6, 11, 12, per the artifact README.
- Leakage measured on a Sakura-G FPGA board (Xilinx ISE), with a Picoscope 5000D and a CT1 current probe.
- Formal checks: SILVER (PINI of the HPC4 gadget) and PROLEAD (a toy Ascon with one S-box).
- Gadgets: HPC2, HPC4, HPC4G (full resource sharing). The artifact also includes a Trivium PRNG, and Ketje/Keccak for comparison.
- Compiler flags: not applicable (this is HDL work).

## Threat model
- Adversary: a side-channel adversary using power traces, under the robust probing model (glitches and transitions), at a chosen masking order.
- Leveled designs aim at integrity with leakage (CIML2) for encryption and decryption, and at confidentiality with leakage (CCAmL1) for encryption only. Confidentiality for decryption under leakage is not provided by the leveled mode, per the snippet.
- What is protected: the long-term key, through masked init and final. Bulk message processing in leveled mode is unmasked, and its leakage is accepted by the mode-level argument.
- Out of scope (as far as could be seen): fault attacks, and software or microcontroller leakage.

## Numbers that matter
- Not extracted. Full text was unavailable.
- Search snippets show leveled HPC2 / HPC4 multi-target figures (Tables 7/8), but the units and message lengths could not be read. They are **deliberately not recorded here**.
- Qualitative result (snippet, attributed to the paper): for leveled CIML2/CCAmL1 designs, HPC2 gadgets are better for long messages and HPC4 gadgets for short ones, "where amortization does not occur". So the paper does frame cost as a function of message length, in hardware latency and area terms.
- Leakage: the README describes a TVLA run (Appendix B) with 10 million traces by default on the Sakura-G. The result was not read.

## Convenient assumptions
- ASIC synthesis numbers, not a fabricated chip. Leakage tested on an FPGA only.
- Randomness comes from an on-chip PRNG (Trivium), and its cost is assumed to fit the hardware budget.
- Leveled security rests on mode-level leakage-resilience arguments (CIML2/CCAmL1). Unmasked bulk processing is not evaluated as a leakage source of the key.
- No energy per frame, no radio cost, no software context.

## Stated limitations and future work
Not read (full text unavailable). From the abstract, the paper evaluates "several protected implementations of Ascon to map its hardware design space". The scope is hardware.

## Load-bearing references (inferred from snippets and README, not from the bibliography)
- Knichel and Moradi, "Low-Latency Hardware Private Circuits" (HPC3), ePrint 2022/507. This is the prior single-cycle gadget the paper improves on.
- Cassiers, Grégoire, Levi, Standaert, "Hardware Private Circuits" (HPC1/HPC2), TCHES 2021. Inferred, not verified from the bibliography.
- Dobraunig, Eichlseder, Mendel, Schläffer, "Ascon v1.2", Journal of Cryptology 2021.
- Leveled implementation and CIML2/CCAmL1 definitions (Berti/Guo/Pereira/Peters/Standaert line of work). Exact citation not verified.

## Related works noted (verified at the level stated)
- **ePrint 2026/1082**: Mirigaldi, Martina, Masera (Politecnico di Torino), "Compact Yet Fast: An Efficient d-Order Masked Implementation of Ascon". ePrint lists it as a "Preprint". It is a **hardware** design that reconfigures countermeasures during processing: a masking gadget for the critical (key-mixing) phases and parallel datapaths for bulk work, plus a Changing-of-the-Guards variant with no fresh randomness. It reports fixed-vs-random t-tests at first and second order. There are no Cortex-M numbers. Status as a DATE 2026 paper is **not verified**: a search snippet points to a DATE 2026 proceedings PDF (past.date-conference.com .../2026/DATA/878.pdf) saying only init and final need strong protection, but that URL returned 404, so the link between the two is unconfirmed.
- **GMU / NIST LWC protected implementations**: Mohajerani, Beckwith, Abdulgadir, Ferrufino, Kaps, Gaj, "SCA Evaluation and Benchmarking of Finalists in the NIST LWC Standardization Process", ePrint 2023/484 (preprint). The abstract benchmarks protected **hardware** on Xilinx Artix-7. No software platform or Cortex-M figure is named in the abstract. A third-party SJTU report on GMU's site evaluates second-order masked software Ascon on an STM32F303 (Cortex-M4); it was not read.
- **Most relevant to the Lane B idea (found incidentally)**: Gigerl, Mendel, Schläffer, Primas, "Efficient Second-Order Masked Software Implementations of Ascon in Theory and Practice", ePrint 2024/755. It was also presented at the NIST LWC Workshop 2023. According to the abstract, it runs on 32-bit ARM and RISC-V, with TVLA on ARM (an STM32F3 / Cortex-M4 per snippet) and Coco verification on an Ibex netlist. Ascon-128 AEAD runs at about 300 cycles/byte with 2 shares and about 550 with 3 shares, and **about 90 cycles/byte with the leveled technique**. This is the direct prior art for "leveled software Ascon on Cortex-M". It should get its own paper note, with attention to which message lengths those cycles/byte figures assume.

## Possible seeds
- Change of platform (hardware to software): this paper covers hardware only. But ePrint 2024/755 already reports leveled software Ascon on ARM, so a "first leveled SW Ascon" framing is NOT open. Rescope.
- Change of workload (short frames): the paper says HPC4 is better for short messages because cost does not amortize. The same reasoning in software suggests the leveled advantage shrinks for 8–64 byte frames. CONJECTURE until 2024/755's per-length data is checked.
- Change of metric (cycles/byte to energy per frame on a Cortex-M33 including the radio): no source read here reports per-frame energy for short IoT frames. Open as far as searched (queries below).
- Phase-2 fit: our own leveled versus uniform masked software Ascon on the EFR32xG24, followed by TVLA, matches the DRP plan.

## Queries run
- "TCHES 2024 low-latency masked gadgets Ascon leveled implementations Cassiers"
- eprint.iacr.org "Low-Latency Masked Gadgets Robust against Physical Defaults"
- "Masked Gadgets" Ascon Verhamme Cassiers eprint.iacr.org 2023 OR 2024 single-cycle HPC4
- "leveled" Ascon "HPC2" "HPC4" "short messages" latency "uniformly protected" multi-target implementation results
- GMU CERG protected software implementations NIST LWC masked Ascon Cortex-M4 side-channel evaluation
- "Protected Software Implementations" NIST lightweight cryptography Ascon masked ARM Cortex-M4 GMU Kaps "Masked Ascon"
- George Mason University CERG "protected software" LWC finalists ... STM32 ...

## Sources
- https://artifacts.iacr.org/tches/2024/a22/
- https://artifacts.iacr.org/tches/2024/a22/readme.html
- https://zenodo.org/records/17453208
- https://eprint.iacr.org/2026/1082
- https://eprint.iacr.org/2023/484
- https://eprint.iacr.org/2024/755
