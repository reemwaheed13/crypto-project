# Collision check R2-B1: leveled vs uniform masked Ascon-AEAD128 for 8–64 B IoT frames on Cortex-M33

Date: 2026-11-05. Candidate: Lane B (lightweight crypto, outside course PQC/FHE/FL scope).

## Queries run (web search, standard mode)
1. `leveled implementation masked Ascon Cortex-M software short messages cycles` (technical)
2. `ePrint masked Ascon software ARM Cortex-M implementation first-order second-order` (ePrint-focused)
3. `side-channel protected Ascon cost small packets IoT sensor energy microcontroller` (plain language)
4. `leakage-resilient Ascon mode protect only initialization finalization software benchmark 2025` (failed: rate limit, no results)
5. `masked Ascon Cortex-M4 OR Cortex-M33 software 2025 2026 side-channel ePrint` (date-restricted variant)
6. `"leveled" Ascon masking short messages overhead crossover software microcontroller`
7. `masked Ascon software performance per message length Cortex-M ChipWhisperer STM32 leveled implementation benchmark table`
8. `Ascon-AEAD128 SP 800-232 masked implementation embedded IoT energy 2026` (date-restricted)
9. `Ascon power analysis countermeasure overhead per packet LoRaWAN BLE short payload microcontroller 2025` (networking/embedded neighbour)
10. `"Efficient Second-Order Masked Software Implementations of Ascon" Cortex-M4 STM32 cycles message length` (coordinator follow-up on 2024/755)

## Papers / sources opened
| ID | Title / what | Verified exists | Depth | Same claim? | Same device? | Same threat model? |
|---|---|---|---|---|---|---|
| ePrint 2024/755 (also NIST LWC 2023 workshop paper + slides) | Gigerl, Mendel, Schläffer, Primas, "Efficient Second-Order Masked Software Implementations of Ascon in Theory and Practice" | Yes (ePrint abstract page) | Abstract only. Full-text attempts failed: eprint PDF HTTP 403; NIST paper PDF and slides PDF downloaded but not text-extractable in this environment (no poppler); reviewer.ly mirror DNS fail | Partly: masked d=2/3 (~300/550 c/B) and leveled (~90 c/B) Ascon-128 software, TVLA on ARM. UNVERIFIED: which message length the c/B figures assume (the ~90 c/B leveled figure reads like a long-message asymptotic throughput, but this is our inference); whether per-message cost for 8–64 B is tabulated; which ARM core (abstract says only "32-bit ARM") | 32-bit ARM + RISC-V; core not named in abstract (related ascon simpleserial targets are STM32F3/F4 Cortex-M4) | Same (DPA on key, d-th order) |
| TCHES 2024 (article 11689) | Cassiers, Standaert, Verhamme, low-latency masked Ascon gadgets, leveled; HPC4 for short msgs, HPC2 for long | Yes (search snippets; full text blocked by Anubis) | Snippet only | Same question (short vs long message, leveled amortisation) but for hardware latency | No (hardware) | Same |
| ePrint 2026/1082 (DATE 2026) | Mirigaldi, (Paninforni), Martina, Masera, reconfigurable d-order masked Ascon | Yes (ePrint abstract) | Abstract | Exploits leveled mode; no short-message crossover in abstract | No (hardware) | Same |
| github.com/ascon/ascon-c | Reference repo: `protected_bi32_armv6` and `protected_bi32_armv6_leveled` | Yes | README | Implementations exist; README benchmark tables do not include masked/leveled rows or Cortex-M cycles | ARMv6/v7(-M) code | Same |
| ePrint 2026/1882 | Shukla & Shukla, ASCON-Edge reproducible benchmark protocol for Ascon-AEAD128 on IoT nodes | Yes (ePrint abstract) | Abstract | No masking, no measurements yet | Generic IoT nodes | No SCA |

Not opened (seen in results only): ePrint 2023/1914 (hardware 2nd-order Ascon), MDPI Chips 2025 Ascon survey, GMU "Masked_Ascon_in_SW.pdf" (connection refused), github.com/ascon/simpleserial-ascon, Cambridge ASCON trace dataset.

## Verdict: PARTIAL OVERLAP (weak novelty)
- KNOWN: leveled Ascon software implementations exist and are benchmarked against uniform masking on 32-bit ARM (ePrint 2024/755; ascon-c protected/leveled code). The "short messages do not amortise masked init/final" observation is stated for hardware in TCHES 2024 and implied in DATE 2026.
- Remaining open (as far as searched): an explicit per-frame-length (8–64 B) cycle/energy curve and crossover L* for unprotected / leveled / uniform d=2,3 on Cortex-M33 with SP 800-232 Ascon-AEAD128. Not found in any source read. BLOCKER: 2024/755 full text not read; its tables may already give per-length numbers (ascon-c-style benchmarks usually list 1..1536 B), which would turn this into a collision. Someone must read the 2024/755 PDF manually before committing.
- CONJECTURE (ours) weakening the claim: with a 16-B rate, a 64-B frame is ~4 extra p8 calls vs 2 p12 (24 rounds) masked in init/final, so leveling saves roughly 30–55% of masked rounds even at 64 B; L* is essentially analytic from per-round costs, so the result is a measurement note, not a research question. Not a variant of round-1 D3 (different asset and adversary), but it is a minor engineering extension of 2024/755.
