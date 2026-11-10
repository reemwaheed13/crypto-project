# Collision check r3: D3 salvage, reversed transciphering-vs-direct-HE crossover (2026-11-10)

Checked by: collision-check agent. Labels: **KNOWN** (cited, source given), **CONJECTURE** (ours), **ASSUMPTION** (modelling
choice), **DERIVED** (our arithmetic from KNOWN inputs). "Open" always means *open as far as searched*.

## Candidate claim
On a Cortex-M class sensor, transciphering (Pasta / HERA / Kreyvium) saves the sensor energy only for small uploads.
Above a few kB per upload, direct BFV/CKKS client encryption is cheaper. This reverses the usual "transciphering always
helps the client" story.

## Queries run (verbatim)
1. (technical) `transciphering versus direct homomorphic encryption client energy crossover microcontroller ciphertext expansion break-even` (extended)
2. (plain-language) `when is hybrid homomorphic encryption not worth it for IoT sensor: large uploads direct BFV encryption cheaper than symmetric cipher`
3. (ePrint-oriented) `eprint.iacr.org hybrid homomorphic encryption client communication computation trade-off embedded device BFV ciphertext size Pasta HERA comparison`
4. (baseline lookup) `SEAL-Embedded: A Homomorphic Encryption Library for the Internet of Things TCHES 2021 pdf`
5. (2025-2026 date variant) `eprint 2025 2026 hybrid homomorphic encryption IoT device energy communication cost comparison direct CKKS BFV ciphertext transciphering break-even payload size` (extended)
6. (neighbour: networking / WSN) `wireless sensor network energy radio transmission homomorphic ciphertext expansion versus symmetric stream cipher privacy-preserving aggregation energy per byte Cortex-M`
7. (ePrint, title lookup) `"HHE Land" eprint 2025/071 client-side PASTA HERA communication cost BFV comparison` (no relevant hits)
8. (2026 date variant, embedded) `2026 seeded BFV ciphertext microcontroller versus hybrid homomorphic encryption sensor upload energy radio 802.15.4 BLE` (extended)
9. (neighbour: ML / FL) `federated learning edge device homomorphic encryption packing slots ciphertext per update hybrid HE vs CKKS client upload energy crossover model size`
10. (ePrint-restricted, allowed_domains=eprint.iacr.org) `transciphering client cost ciphertext expansion direct homomorphic encryption cheaper large messages microcontroller`

## Papers opened
| Paper | ID | Verified | Reading depth | Same claim / device / threat model? |
|---|---|---|---|---|
| Zeppelin: Client-Side BFV Enc/Dec for Helium MCUs | arXiv 2610.08301 | yes (arXiv HTML) | **Full HTML**, both halves (0-100k and 100k-122k chars) | Direct BFV on STM32N657 Cortex-M55 only. No transciphering, no HHE, no symmetric-cipher baseline, no radio energy, no crossover. Cites SEAL-Embedded (§5.5, Table 6). |
| SEAL-Embedded (Natarajan, Dai), TCHES 2021(3):756-779 | DOI 10.46586/tches.v2021.i3.756-779 | yes (MSR page, dblp) | **ABSTRACT ONLY** + GitHub `user_defines.h` + Zeppelin Table 6. Both PDF hosts (tches.iacr.org, icscm.ub.rub.de) blocked by Anubis bot wall. | Direct CKKS on nRF52840 Cortex-M4. No transciphering comparison found in abstract. |
| On the Feasibility of HHE for Intelligent Transportation Systems | arXiv 2602.02717 | yes | Full HTML (via fetch summary) | Closest framing: ciphertext size and fragment counts, Rubato vs BFV/BGV/CKKS, 200 B messages. **Theoretical only, no device, no client energy, no crossover**; says experiments "remain to be conducted". Small messages only, so it argues the opposite direction (HHE wins). |
| Federated Learning: An approach with HHE (Porto) | arXiv 2509.03427 | yes | Full HTML (via fetch summary) | PASTA+BFV vs BFV for FL on Colab (T4/Xeon). Client upload about 2000x smaller with HHE, client enc 9.7x faster. No MCU, no energy, no payload crossover. |
| Earlier, from `collision-cand3-transciphering-client-mcu.md` and notes (not re-opened) | SoK 2025/669 (full text), HHE Kombat 2026/1314 (targeted full text), Pasta 2021/731, 2025/720 (abstract) | | see those files | All Xeon/server or key-size only; none computes a client crossover against direct HE on an MCU. |

Search-snippet-only (not opened, not relied on): GuardML arXiv 2401.14840 (single-input hybrid slower than BFV, 9 vs 7 ms, PC);
ePrint 2025/093 survey; ePrint 2020/1335 (RtF; notes short messages are a weak spot for expansion); TFHE-SBC arXiv 2503.02559.

## Extracted parameters
**Zeppelin (arXiv 2610.08301), all FULL TEXT (HTML):**
- N = 4096 (§2.1, §5.1). Slots = 4096, batching, all filled in benchmarks (§2.3, §5.1; abstract "4096 packed values").
- t = 65537 (§2.1, §5.1), so each slot holds an integer mod 65537. "About 16 useful bits per slot" is our inference (ASSUMPTION), not stated.
- Q = 104 bits, 4 RNS primes of about 26 bits (§2.4, §5.1). 128-bit security per the HE standard (abstract).
- Ciphertext: classic 131,072 B; seeded 65,572 B (32-byte seed replaces c1) (§4.6, Fig. 6, Table 4). Seeded applies to **secret-key** encryption only, so the sensor holds the BFV secret key.
- Symmetric seeded encode+encrypt: 3,810,352 cycles, 4.76 ms, 339.4 µJ. Public-key encode+encrypt: 7,051,224 cycles, 8.81 ms, 628.2 µJ (Table 4). Energy is **estimated** from 71.3 mW core power at 0.81 V, not measured (Table 4 footnote).
- Device: STM32N657 Cortex-M55 with Helium, 800 MHz (Table 1, §5.1). RAM peak 408.5 KB (symmetric), 488.5 KB (public key) (Table 7).

**SEAL-Embedded (TCHES 2021):**
- N = 4096, RNS primes of 30 bits or fewer, 64-137 KB RAM, encrypts 2048 single-precision numbers: **ABSTRACT ONLY**.
- 737 ms, 47.2 M cycles, Cortex-M4 at 64 MHz (nRF52840); 77 ms on Cortex-A7: **ABSTRACT ONLY** (time), cycles and clock from **Zeppelin Table 6** (secondary source).
- Default 3 primes, 30-bit, symmetric mode when malloc is off: **GitHub `device/lib/user_defines.h`** (code, not paper).
- Ciphertext size as transmitted: **NOT FOUND** (paper body unread). We DERIVE: 3 primes x 30 bits x 4096 = 46,080 B per polynomial if bit-packed. Seeded symmetric about 46,112 B; full (asymmetric) about 92,160 B. If stored as 32-bit words: 49,152 / 98,304 B. ASSUMPTION, needs the PDF.
- Energy: **NOT REPORTED** in the abstract.

## Recomputed break-even
Model (from `cost-he-friendly-ciphers-2026-11-10.md`, ASSUMPTION): 0.50 µJ per on-air byte at 0 dBm; 802.15.4 frames of 102 B payload + 31 B overhead.
Transciphering per payload byte (MEASURED instructions x CPI 1-2 x 0.100-0.147 nJ/cycle, plus radio): Pasta-4 6.7-18.5, HERA-5 1.43-2.85, Kreyvium 0.78-1.0 µJ/B.
Capacity per ciphertext (ASSUMPTION: 16-bit readings, 2 B per slot): Zeppelin 4096 x 2 = **8,192 B**; SEAL-Embedded 2048 x 2 = **4,096 B**.

Radio energy per direct-HE ciphertext (DERIVED):
- Zeppelin seeded: ceil(65,572/102) = 643 frames; 65,572 + 643x31 = 85,505 B on air -> **42.75 mJ**. Compute +0.339 mJ (M55, estimated; separate term, not ported).
- Zeppelin public key, classic: ceil(131,072/102) = 1,286 frames; 131,072 + 39,866 = 170,938 B -> **85.47 mJ**. Compute +0.628 mJ (M55).
- SEAL-Embedded seeded (DERIVED size 46,112 B): 453 frames; 60,155 B -> **30.08 mJ**. Compute: symmetric time not known.
- SEAL-Embedded asymmetric (DERIVED size 92,160 B): 904 frames; 120,184 B -> **60.09 mJ**. Compute, separate term: 47.2 M Cortex-M4 cycles x 0.100-0.147 nJ/cycle = 4.7-6.9 mJ (ASSUMPTION: M4 cycles approximately equal to M33 cycles at EFR32MG24 energy per cycle).

L* = direct-HE energy per ciphertext / transciphering µJ per byte (radio only; the M55 compute adds under 1%):

| Direct-HE baseline (capacity) | Pasta-4 L* (high bracket 18.5 to low bracket 6.7 µJ/B) | HERA-5 L* (2.85 to 1.43) | Kreyvium L* (1.0 to 0.78) |
|---|---|---|---|
| Zeppelin seeded 42.75 mJ (8,192 B) | 2,311 to 6,381 B: **inside capacity** | 15.0 to 29.9 kB: beyond capacity | 42.8 to 54.8 kB: beyond capacity |
| Zeppelin public key 85.47 mJ (8,192 B) | 4,620 (inside) to 12,757 B (beyond) | 30.0 to 59.8 kB: beyond | 85.5 to 109.6 kB: beyond |
| SEAL-E seeded* 30.08 mJ (4,096 B) | 1,626 (inside) to 4,490 B (beyond) | 10.6 to 21.0 kB: beyond | 30.1 to 38.6 kB: beyond |
| SEAL-E asym* 60.09 mJ (+4.7-6.9 mJ M4 compute) (4,096 B) | 3,248-3,622 (inside) to 8,969-10,000 B (beyond) | 21.1 to 46.9 kB: beyond | 60.1 to 85.9 kB: beyond |

*SEAL-Embedded ciphertext size DERIVED, not read from the paper.

Multi-ciphertext check (DERIVED). Fully packed direct HE costs 42.75 mJ / 8,192 B = **5.22 µJ/B** (Zeppelin seeded), 10.43 µJ/B (Zeppelin public key), 7.34 µJ/B (SEAL-E seeded*), 14.7 µJ/B (SEAL-E asym*, radio only).
- **HERA-5 and Kreyvium:** cheaper per byte than every fully packed direct-HE baseline. **Transciphering wins at every upload size.** The claim is false for them.
- **Pasta-4, high bracket (18.5 µJ/B):** worse per byte than every baseline. Direct HE is cheaper above L* = 1.6-4.6 kB, and stays cheaper for multi-ciphertext uploads (Zeppelin seeded: 2,311(k+1) < 8,192k for all k >= 1). The claim holds.
- **Pasta-4, low bracket (6.7 µJ/B):** beats Zeppelin seeded only within the first ciphertext (crossing at 6.4 kB). Because cost steps up with each new ciphertext, transciphering wins again just after each 8,192 B boundary while 6,381(k+1) > 8,192k, i.e. k <= 3. Direct HE is cheaper at all sizes above about 32.8 kB. Against the public-key and SEAL-E baselines, the low bracket never crosses.

## KNOWN / CONJECTURE / ASSUMPTION
- KNOWN: Zeppelin parameters above (full text); SEAL-E N, slots, RAM, time (abstract); no opened paper computes a client energy crossover between transciphering and direct HE on an MCU.
- CONJECTURE: the reversal exists only for expensive F_p ciphers (Pasta-4, probably Pastav2 at 3.3-8.3 µJ/B, Dasta, Rasta, FiLIP). It needs a seeded secret-key BFV client with full slot packing. A Helium-less M33 would raise Zeppelin's compute term from 0.34 mJ to perhaps a few mJ. That is still small next to 42.75 mJ of radio, so the conclusion should hold.
- ASSUMPTION: 16 useful bits per slot; 0 dBm radio and frame model; CPI 1-2; SEAL-E ciphertext size; transciphering per-byte figures already include each cipher's own expansion; secret-key direct HE is acceptable in the threat model (the sensor holds the HE secret key).

## Verdict
**Open as far as searched, but the claim as worded is false for 2 of the 3 ciphers.** No paper among the 10 queries and 4 newly opened texts computes this crossover on an MCU. arXiv 2602.02717 is the nearest framing (sizes only, theoretical). The surviving claim is narrow. Against seeded, fully packed BFV (Zeppelin), Pasta-4 costs the sensor more than direct HE above about 2.3-6.4 kB per upload. HERA-5 and Kreyvium never do. Before relying on the SEAL-E rows, read the SEAL-Embedded PDF for its ciphertext size and seeding.
