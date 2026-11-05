# Paper note: Low-Stack HAETAE for Memory-Constrained Microcontrollers

## Verified identity (KNOWN)
- **VERIFIED.** The candidate arXiv ID resolves to a real paper that matches the candidate title exactly.
- **Title:** "Low-Stack HAETAE for Memory-Constrained Microcontrollers"
- **arXiv ID:** arXiv:2604.15868v1 (cs.CR)
- **Authors:** Gustavo Banegas (LIX, CNRS, Inria, École Polytechnique, Institut Polytechnique de Paris, France); YoungBeom Kim and Seog Chung Seo (Kookmin University, Seoul, Republic of Korea); Christine van Vredendaal (NXP Semiconductors, The Netherlands). Author list is in alphabetical order (stated in a footnote).
- **Venue/status:** arXiv preprint, dated 17 Apr 2026 (document date 2026-04-20). Not yet formally published / peer-reviewed as of this note. Funded by the HYPERFORM consortium (France 2030, ANR-22-PETQ-0008 PQ-TLS).
- Verified via the arXiv abstract page, the arXiv HTML render, and a direct page-by-page read of the arXiv PDF (Tables 3/4/5 and the References page).
- **Caution — do not confuse with a sibling paper:** a separate, independent work exists — MinGi Kim, YoungBeom Kim, DongHyun Shin, Seog Chung Seo, "HAETAE on a Diet: Less Stack, Less Time on Low-End Microcontrollers," Cryptology ePrint 2026/2578 (Oct 2026, Kookmin University). Overlapping author (YoungBeom Kim, Seog Chung Seo) and theme but a different paper. This note is about arXiv:2604.15868 only.

## Reading depth (KNOWN)
- **Full read** of the load-bearing parts: title/abstract/intro (pp. 1-2), sampler + verification streaming design (pp. 13-16), the full results section with Tables 3, 4, 5 (pp. 17-20), the complete References list (p. 21), and the KeyGen appendix (p. 22). Read directly from the arXiv PDF, so the numbers below are transcribed from the paper's own tables, not from a summarizer.

## Core claim (KNOWN)
A pure-C (optionally asm-accelerated) low-stack implementation of the HAETAE module-lattice signature scheme that fits key generation, signing, AND verification for all three security levels (HAETAE-2/-3/-5) within the 8 kB-16 kB SRAM budget of low-end Cortex-M / RISC-V microcontrollers, by (i) rejection-aware pass decomposition, (ii) component-level early rejection, and (iii) reverse-order streaming rANS entropy coding — reducing signing stack from 71-141 kB (reference) to ~5.8-6.1 kB, while making verification 2.34-3.34x faster than ML-DSA m4fstack.

## Platform & what was ACTUALLY measured (KNOWN)
**Primary benchmark board — REAL HARDWARE (not emulator, not PC):**
- **NUCLEO-L4R5ZI** board: STM32L4R5ZI, ARM Cortex-M4F, 2 MB Flash, 640 kB RAM. This is the default pqm4 target.
- Benchmarked under the **pqm4** framework ([KPR+]).
- Clock config: cycle-accurate speed measured at **CLOCK_BENCHMARK = 20 MHz, 0 wait-state flash**; stack measured at CLOCK_FAST = 120 MHz. Median of 100 runs, k = 10^3 (i.e., counts reported in kilocycles). Compiler: **arm-none-eabi-gcc 11.3.1**, `-Os` for stack/code-size builds, `-O3` for cycle-count builds (separate builds). `noinline` pass boundaries rely on `__attribute__((noinline))`.
- Target RAM budget framing: "8 kB-16 kB of available SRAM" after RTOS/network stack/drivers are loaded.

**Portability validation — also REAL HARDWARE (RIOT-OS), Table 4:**
- **Nordic nRF52840**: ARM Cortex-M4F @ 64 MHz, 256 kB RAM.
- **Espressif ESP32-C6**: RISC-V RV32IMAC @ 80 MHz, 512 kB RAM.
- Cycles via RIOT's `xtimer` converted through `coreclock_hz`; stack via canary high-watermark; 100 iterations.

**Stack usage (bytes), ours (C) vs reference, from Table 3 / text (KNOWN):**
| Op | HAETAE-2 | HAETAE-3 | HAETAE-5 | Reference (range, all levels) |
|---|---|---|---|---|
| Key generation | 5,848 | 5,848 | 4,816 | up to 41,236 / 52,500 B |
| Signing | 5,968 | 6,152 | 6,136 | 71,112-144,408 B (i.e. "71-141 kB") |
| Verification | 4,936 | 4,840 | 4,840 | 33,448-68,856 B |
- All ours variants: **.data = 0 and .bss = 0** (fully streaming, no static workspace).
- Stack reduction vs reference: keygen 75/86/8 %, signing 92/95/24 %, verification 85/91/22 % (HAETAE-2/-3/-5).

**Absolute cycle counts (kilocycles, ours (C), Table 3 — KNOWN, these ARE in the paper):**
| Op | HAETAE-2 | HAETAE-3 | HAETAE-5 |
|---|---|---|---|
| Key generation | 11,630 k | 20,998 k | 11,561 k |
| Signing | 115,534 k | 181,792 k | 218,957 k |
| Verification | 1,417 k | 2,897 k | 3,510 k |
- (asm variant is faster on sign, e.g. HAETAE-5 sign 163,125 k; see Table 3 for full asm figures.)
- Signing is 1.86-3.40x SLOWER than the reference C (cost of seed-based recomputation each pass); this is the explicit speed/memory tradeoff. Verification is actually FASTER than reference (0.82-0.91x) due to the smaller working set.

**Sizes (bytes), Table 5 (KNOWN):**
| | HAETAE-2 | HAETAE-3 | HAETAE-5 |
|---|---|---|---|
| Public key \|pk\| | 992 | 1,472 | 2,080 |
| Secret key \|sk\| | 1,408 | 2,112 | 2,752 |
| Signature \|sig\| | 1,474 | 2,349 | 2,948 |

**Code size (KNOWN):** ours (C) .text ~30,150 B for HAETAE-5 (+4.7 % vs reference); ML-DSA scheme-only code 19-24 kB (smaller than HAETAE).

## The ML-DSA / ML-DSA crossover (KNOWN — the load-bearing result for us)
Measured on the SAME board/clock/framework, from Tables 3 and 5:
- **Verification within 8 kB (sig buffer + stack):** HAETAE verify is **2.34-3.34x faster than ML-DSA m4fstack** at comparable security. Example cycle counts: HAETAE-2 verify 1,417 k vs ML-DSA-44 m4fstack 3,816 k; HAETAE-3 2,897 k vs ML-DSA-65 6,771 k; HAETAE-5 3,510 k vs ML-DSA-87 11,713 k. HAETAE verification totals (|sig|+stack) 6,410-7,788 B — all fit 8 kB.
- **Signing within 16 kB:** HAETAE sign totals (|sk|+|sig|+stack) are 8,850 / 10,613 / 11,836 B for HAETAE-2/-3/-5 — all fit 16 kB. **ML-DSA-87 m4fstack signing total = 17,667 B — EXCEEDS 16 kB.** (ML-DSA-44 and -65 sign totals are 10,044 B and 13,949 B, which fit.) So the crossover is specifically at the top security level: HAETAE-5 sign fits 16 kB, ML-DSA-87 does not — ~5.7 kB difference.
- ML-DSA m4fstack baseline follows the [BRS22] (Bos-Renes-Sprenkels) recomputation strategy; numbers taken from pqm4.

## Threat model (KNOWN — this is primarily a performance/implementation paper)
- No formal adversary/security-reduction section. It inherits HAETAE's (Fiat-Shamir-with-aborts) security.
- **Constant-time scope (from abstract/HTML + design sections):** avoids branches and memory accesses that depend on long-term secret coefficients, EXCEPT the rejection-sampling loop inherent to Fiat-Shamir-with-aborts. Verification processes only public inputs. Signature format checks (length, zero-padding, rANS decoder errors) are rejected before transcript recomputation.
- **Explicitly out of scope:** countermeasures against power-analysis and fault attacks are NOT addressed; a thorough side-channel analysis of the altered streaming data-access patterns is left for future work. (Directly relevant to our course's phase-2 side-channel plan.)

## Convenient assumptions (ASSUMPTION / inference)
- Public key is stored in **flash**, so the RAM budget for verification is only (received signature + stack) — this is what lets verification "fit 8 kB." Realistic for firmware auth, but worth flagging as an assumption baked into the 8 kB claim.
- Stack is the binding constraint (not code size or cycles). True for the stated 8-16 kB class; may not hold where flash is also tight or where signing latency matters.
- Speed numbers assume `-O3`; stack numbers assume `-Os`. The 8-16 kB "fit" is an `-Os` result — a different compiler/flag could change peak stack.
- m4f (asm) ML-DSA/HAETAE comparison uses an older spec version with hand-asm, which the authors themselves note is "not entirely fair"; the fair comparison is ML-DSA m4fstack vs ours (C).
- Energy/radio/communication costs are NOT measured (only cycles, stack, sizes). Relevant because our project framing asks for energy and communication estimates.

## Stated limitations & future work (KNOWN)
- Side-channel / fault resistance not provided; streaming changes data-access patterns and "a thorough side-channel analysis is left for future work" (one-line quote).
- Fixed-point approximation error of the two-pass streaming InvSqrt sampler is only argued distribution-identical conditioned on acceptance; "a further quantitative analysis of the fixed-point approximation error is left to future work."
- Signing is markedly slower than reference (explicit memory-for-time tradeoff), so this is for stack-bound, not throughput-bound, deployments.
- Code (per the HTML version) "will be available upon acceptance."

## Load-bearing references (KNOWN)
1. **[CCD+24]** Cheon, Choe, Devevey, Güneysu, Hong, Krausz, Land, Möller, Stehlé, Yi. "Haetae: Shorter lattice-based Fiat-Shamir signatures." TCHES 2024(3):25-75. — the baseline HAETAE scheme (CHES'24). Also [CCD+23] NIST round-1 additional-signatures technical report, and [Kpq26] the KpqC final spec.
2. **[BRS22]** Bos, Renes, Sprenkels. "Dilithium for memory constrained devices." AFRICACRYPT 2022, LNCS 13503, 217-235. — the ML-DSA/Dilithium low-stack recomputation baseline that m4fstack follows; the thing HAETAE is benchmarked against.
3. **[HCK+26]** Hyoung, Cho, Kim, Lee, Seo, Sim. "Memory-efficient implementation of SMAUG-T and HAETAE." ePrint 2026/442. — the prior state-of-the-art memory-optimized HAETAE on Cortex-M4 (HAETAE-5 only); the direct "prior work" this paper beats and extends to HAETAE-2/-3.
4. **[KPR+]** Kannwischer, Petri, Rijneveld, Schwabe, Stoffelen. pqm4 (post-quantum crypto library / benchmark framework for Cortex-M4). — the measurement harness; all headline numbers come through it.
5. **[NIS23]** NIST FIPS 204 (initial public draft), Module-Lattice-Based Digital Signature Standard (ML-DSA). — the standardized comparison target. (Supporting: [Lyu09] Fiat-Shamir with aborts; [LWKP24] two-pass hyperball/Gaussian sampler origin; [BZB+22]/[BDKR23] constrained-device firmware-update security.)

## Relevance to our project (honest assessment)
Strongly relevant and arguably the single best anchor paper for a "low-stack PQC signatures for firmware authentication / attestation on MCUs" direction. It gives us a concrete, same-board, same-framework **stack-RAM crossover**: HAETAE verification fits in 8 kB on a real Cortex-M4F at all security levels while being 2.3-3.3x faster than ML-DSA m4fstack, and — the sharper crossover — HAETAE-5 signing fits a 16 kB budget (11,836 B) whereas ML-DSA-87 signing does not (17,667 B). That is exactly the "primitive A fits where primitive B doesn't" framing our course wants, with numbers we can cite rather than estimate. KNOWN caveats before we lean on it: (a) it is an unrefereed arXiv preprint (Apr 2026), so treat numbers as author-reported until the TCHES/peer-reviewed version appears; (b) the 8 kB verification claim assumes pk-in-flash; (c) it measures cycles/stack/sizes only — NO energy, radio, or communication cost, which our project framing explicitly asks for, leaving us a genuine measurement gap to fill; (d) signing is slower than reference, so the application must be stack-bound (e.g. an occasional firmware-update signature or device attestation), not a high-rate signer. It does NOT do side-channel analysis — which is precisely our intended phase-2 DRP work, so it is a baseline to attack, not a competitor.

## Possible seeds (direction-miner moves, one line each)
- **Add the missing cost axis:** re-measure HAETAE-5 vs ML-DSA-87 for ENERGY (mJ/op) and OTA communication cost on the EFR32xG24, since this paper measured only cycles/stack/sizes — fills a stated gap.
- **Attack the deferred threat model:** do the side-channel/power leakage study the authors explicitly left open, targeting the FSwA rejection loop and the new streaming rANS data-access patterns.
- **Stress the crossover boundary:** find the exact RAM/flash point where ML-DSA stops fitting but HAETAE still does on a realistic RTOS-loaded 8-16 kB device, as a firmware-auth deployment rule.
- **Probe the convenient assumption:** test whether the 8 kB verification claim survives when pk must be in RAM (not flash) or under `-O2`/LTO instead of `-Os`.
- **Reproduce & diff:** independently rebuild under pqm4 to confirm the author-reported stack/cycle numbers before building on them (preprint, code not yet released).
