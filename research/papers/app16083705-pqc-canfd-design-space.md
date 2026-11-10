# Design-Space Mapping of PQC Artifact Transport on CAN-FD (Lee et al., Appl. Sci. 2026)

## Citation
- Title: Design-Space Mapping of Post-Quantum Cryptographic Artifact Transport on CAN-FD: A Discrete-Event Simulation Study
- Authors: Min-Woo Lee, Minjoo Sim, Siwoo Eum, Gyeongju Song (Dept. of Information Computer Engineering, Hansung University), Hwajeong Seo (Dept. of Convergence Security, Hansung University; corresponding author)
- Venue/status: PUBLISHED, Applied Sciences (MDPI) 16(8):3705, 2026. Received 25 Mar 2026, accepted 7 Apr 2026, published 10 Apr 2026 (p. 1). DOI 10.3390/app16083705.
- Code: https://github.com/minunejip/CAN-PQC (Sec. 4.5, Data Availability). Not checked by us.
- Predecessor: Lee et al., "KpqC on CAN-FD: A Feasibility Assessment", ACK 2025 (ref. [21]); frame-count screening of 7 KpqC sets on an empty bus.

## Reading depth
Full text (local PDF, applsci-16-03705-v2.pdf, 40 pages), read via the page-marked text extraction, every page 1–40 including discussion, guidelines (6.4), limitations (6.5), future work (Sec. 7) and references. The paper has no appendices. Tables 5, 7, 8, 10, 12 and 13 extracted cleanly and are internally consistent (frame counts in Table 5 match Figure 2 labels). Keyword search was done by reading, not by grep (no shell available to this agent).

## Core claim
Transporting PQC public keys, ciphertexts and signatures (one-off, via ISO-TP) over a shared CAN-FD bus is feasible only in some operating regimes; for KEM handshakes the number of ECUs and startup staggering matter more than algorithm choice, for a single diagnostic signature the signature size decides, and FN-DSA-512 is the most transport-friendly signature.

## Platform
- Pure-Python (3.12) discrete-event simulator written by the authors, 1 µs time resolution, no SimPy (Sec. 1, 4.1). Host PC hardware is NOT stated. No MCU, no ECU, no hardware-in-the-loop, no CAN transceiver.
- FLAG: everything is a bus-level simulation on a PC. No cryptographic computation was run or timed at all (transport-only workload, Sec. 6.5 bullet "Transport-only PQC workload").
- CAN-FD: single bus segment, 1 Mbit/s arbitration / 5 Mbit/s data, 64 B data field, worst-case frame time T_base = 164 µs (164.2 rounded), T_gap = 0 (Sec. 3.2, 3.5).
- ISO-TP normal addressing: SF/FF carry 62 B, CF 63 B; N_frames(B) = 1 + ceil((B-62)/63) (Sec. 3.6, Eq. 2). STmin = 0, BS = 32, T_FC = 1 ms deterministic (Sec. 4.2, Table 6).
- Background traffic: 3 SAE-class profiles (light 4, typical 10, heavy 13 periodic streams, all 64 B, periods 5–1000 ms, implicit deadline D = T), filler streams at CAN-ID >= 0x700 to reach load 20/40/60/80 % (Sec. 3.3, Table 3). Phase A uses only the typical profile at 40/60/80 %.
- PQC frames at CAN-ID 0x090 (high) or 0x700 (low); supplementary 0x300 (Table 13).
- Horizon 300 ms per session; 10 seeds x 50 sessions (Phase A), but outcomes are deterministic regardless of seed (Sec. 4.4.1–4.4.2).
- Parameter sets (Table 5): ML-KEM-512/768/1024, HQC-128/192/256, SMAUG-T1/3/5, NTRU+768/864/1152, ML-DSA-44/65/87, FN-DSA-512/1024, SLH-DSA-128s/192s/256s (only "s" variants), HAETAE2/3/5, AIMer128s/f, 192s/f, 256s/f. 29 sets, 534 Phase A scenarios.
- Compiler flags: not applicable (no crypto code compiled).

## Threat model (Sec. 3.1)
- Adversary: passive, records all CAN-FD traffic; harvest-now-decrypt-later against asymmetric key-establishment handshakes. "The adversary does not inject or modify frames; bus-level authentication and intrusion detection are orthogonal concerns outside the scope of this study."
- Protected: (KEM) the per-session symmetric key from gateway-PK broadcast + per-ECU ciphertext; (DSA) authenticity/origin of a single UDS diagnostic response verified by a tester, with PK sent on demand.
- Out of scope: active attackers, compromised ECUs, SecOC key misuse, gateway routing, bit errors, crypto compute time, protocol state machine.

## Numbers that matter
- Frame counts (Table 5, Fig. 2), PK + Sig: ML-DSA-44 21 + 39; FN-DSA-512 15 + 11; SLH-DSA-128s 1 + 125; HAETAE2 16 + 24; AIMer128s 1 + 67; ML-KEM-512 13 + 13; SMAUG-T1 11 + 11; HQC-128 36 + 71.
- Sizes (Table 5): ML-DSA-44 PK 1312 / Sig 2420 B; FN-DSA-512 897 / 666 B; SLH-DSA-128s 32 / 7856 B; SLH-DSA-256s 64 / 29,792 B.
- Feasibility thresholds (Sec. 3.8): Feasible = P99 PQC completion <= 50 ms AND P99 periodic jitter <= 50 ms AND miss rate <= 0.01; Borderline = 100 ms / 80 ms / 0.05. 50 ms anchored to ISO 14229-2 P2_Server_max.
- DSA results, 6 scenarios each, F/B/I (Table 7b, Fig. 4): FN-DSA-512 6/0/0; FN-DSA-1024 4/2/0; HAETAE2 4/2/0; ML-DSA-44 2/4/0; ML-DSA-65 0/4/2; ML-DSA-87 0/1/5; SLH-DSA-128s 0/1/5; SLH-DSA-192s and -256s 0/0/6; AIMer128s 2/4/0.
- KEM (Table 7a): SMAUG-T1 12/13/11, ML-KEM-512 8/13/15, HQC all 0/0/36. Totals 50 F / 106 B / 378 I of 534.
- ML-DSA-44, typical, 60 % load, low priority (Table 10): startup P99 12.96 ms, jitter P99 23.63 ms, miss 0.014 (B); RW-5 gives 11.97 / 7.66 / 0.000 (F).
- Validation: sim worst case <= closed-form WCRT in all 26 message/set combinations (Table 4); P99_sim / T_upper 0.11–0.68 (median 0.34) over 534 scenarios (Sec. 4.3.3); ISO-TP frame counts match python-can-isotp for 45 sizes (Sec. 4.3.4).
- Burst theory (Sec. 5.4.2, Table 9): transient overload when rho_PQC + rho_bg > 1, e.g. SMAUG-T1 16-ECU simultaneous rho_total = 1.478, jitter 61.34 ms.
- Baseline (empty-bus) error E99 < 5 % at high priority, 12–27 % at low priority (Sec. 5.2.1).
- CAN-FD vs 100BASE-T1 analytic ratio about 19x–42x (Table 12).

## Convenient assumptions
- No crypto compute time anywhere; encapsulation folded into the 5 ms stagger (Sec. 3.4.1, 6.5.2).
- DSA workload is a single ECU-to-tester exchange, N_ECU = 1, one-shot; PK always sent along (pre-provisioned case only read off Table 5, not simulated) (Sec. 3.4.2).
- Deterministic timing, no bit errors/retransmissions, single bus, binary priority (Sec. 6.5).
- Synthetic SAE-class traffic, not vehicle traces (Sec. 6.5.2).
- Passive adversary only.

## Stated limitations and future work (paraphrased)
- Limitations (Sec. 6.5): deterministic timing underestimates tails; fixed STmin/BS; single segment; no error frames; ideal arbitration; binary priority; transport-only workload; thresholds are engineering criteria; no NIST additional-signature Round 2 candidates.
- Future work (Sec. 7): real-vehicle CAN-FD traces; HIL bench (Vector CANoe / PEAK PCAN, 2–4 nodes); multi-bus gateway and CAN-FD/Ethernet topologies; protocol-level modelling (encapsulation delay, state machine, error recovery); add NIST additional signatures and KpqC updates; adaptive reserved windows from runtime bus-load estimation; re-evaluate under CAN XL (2048 B payload) and CANsec; sweep (STmin, BS) for Pareto configurations. Nothing about batching, amortisation, Merkle trees, TESLA, or per-message authentication of periodic frames.
- Notable remark (Sec. 6.1): FN-DSA signing needs discrete Gaussian sampling, "substantially slower than ML-DSA signing", and constant-time care; deployment must weigh transport vs compute vs implementation complexity, and this paper supplies only transport.

## Load-bearing references
- [22] Davis, Burns, Bril, Lukkien, CAN schedulability analysis: refuted, revisited and revised, Real-Time Systems 2007 (WCRT validation and burst bound).
- [21] Lee et al., KpqC on CAN-FD: A Feasibility Assessment, ACK 2025 (predecessor).
- [18] Pott et al., Secure Boot in Automotive Real-Time Systems in Context of PQC, CSCS '25 (ML-DSA/SLH-DSA/FN-DSA on AURIX TC277/TC397; boot budget).
- [17] Fritzmann, Vith, Sepúlveda, PQC for automotive systems, Microprocess. Microsyst. 2021 (AURIX KEM timings used for 5 ms stagger and T_FC).
- [16] AUTOSAR CP R22-11 SecOC specification (current symmetric-MAC baseline, mentioned only in the introduction).

## Possible seeds (direction-miner moves, one line each)
- Change the workload: one-off artifact transfer -> continuous per-frame source authentication of periodic streams (paper never does this).
- Add the missing cost axis: put measured/pqm4 sign and verify cycles on a Cortex-M into their transport model (paper ignores compute entirely).
- Amortise: one signature per window of N frames (Merkle root or TESLA anchor) and re-rank FN-DSA vs ML-DSA vs SLH-DSA; their ranking is purely by single-shot size.
- Strengthen the adversary: active/insider ECU holding SecOC keys instead of a passive HNDL recorder.
- Reuse their open simulator and validated frame model (Eq. 2, T_base = 164 µs, Table 3 traffic) as the bus-load calibration for our estimates, with citation.
- CAN XL re-evaluation is their own stated next step: avoid as a primary direction (likely competitor).

## Collision questions for A4

### 1. Batching / Merkle / amortised or aggregated signatures / hash chains / TESLA? Verdict: NO
Keyword hits found by full reading (no grep):
- "batch": none.
- "Merkle", "tree", "authentication path": none. (SLH-DSA is described only as "stateless hash-based", Sec. 2.2; its internal hypertree is never mentioned.)
- "amortis/amortiz": none.
- "aggregate": only non-cryptographic uses: "target aggregate bus utilization" (Sec. 3.3, p. 8); "Aggregate metrics across seeds" (Algorithm 1, p. 16); "Table 7 aggregates the results per algorithm" (Sec. 5.2, p. 19).
- "window": all scheduling windows, none cryptographic: "reserved startup window(s)" / "reserved bus windows" (abstract; C3; Sec. 4.1, 4.4.3, 5.5, 6.4, 7: W in {5,10,20,30} ms deferring background traffic); "16 x 5 = 80 ms stagger window" (Sec. 4.4.1); "burst window" (Sec. 5.4.2); "narrow time window" for re-keying (Sec. 3.1); "extended startup window" (Sec. 3.8); future "adaptive mitigation ... reserved window duration" (Sec. 7).
- "TESLA", "delayed key disclosure": none.
- "chain": none ("toolchain" only, Sec. 1).
- "MTL": none.
- "group of frames": none. "Group" appears only as the Table 3 column (Powertrain, Chassis/Safety...), "small ECU groups" in Castiglione et al. (Sec. 2.4), and AUTOSAR CAN-ID "functional groups" (Sec. 6.5.3). "block" appears only as ISO-TP block size BS.
The previously flagged risk ("guidelines may already discuss amortisation") does not materialise: the guidelines (Sec. 6.1, 6.4) are about artifact compactness, domain scale, staggering, reserved windows and Ethernet offload.

### 2. What is the "artifact"? Verdict: one-off handshakes / single diagnostic signature, NOT per-message authentication
"PQC public keys, ciphertexts, and signatures (collectively termed artifacts throughout this paper)" (Sec. 1, p. 2). KEM workload = gateway PK broadcast + N_ECU ciphertexts at startup or re-keying (Sec. 3.4.1). DSA workload = one UDS diagnostic response: ECU sends its verification PK then one signed response to a tester (Sec. 3.4.2), N_ECU = 1. No periodic control/sensor frame is ever signed; periodic frames are only background traffic whose jitter and misses are measured.

### 3. Schemes and CAN FD settings. Verdict: overlaps on schemes, not on workload
ML-DSA-44/65/87, FN-DSA-512/1024, SLH-DSA-128s/192s/256s (plus HAETAE, AIMer, ML-KEM, HQC, SMAUG-T, NTRU+) (Table 5). No SLH-DSA "f" variants. CAN-FD 1/5 Mbit/s, 64 B, ISO-TP normal addressing (62/62/63 B), STmin 0, BS 32, T_FC 1 ms, background load 40/60/80 % (Phase A) and 20–80 % (Phase B), typical/light/heavy profiles (Sec. 3.2–3.6, Table 6).

### 4. Deadline / latency metric for periodic control frames? Verdict: PARTLY
Periodic background streams have implicit deadlines D = T (5, 10, 20, 50, 100, 1000 ms; Table 3) and the paper measures their P99 jitter (threshold 50 ms) and deadline-miss rate (1 % / 5 %) (Sec. 3.8). But the deadline applies to delivery of the periodic frames, not to authenticating them. The PQC deadline is 50 ms completion (borderline 100 ms) for a one-off transfer, anchored to UDS P2_Server_max (Sec. 3.8). No 10 ms authentication deadline, no per-frame verification latency.

### 5. Source of sign/verify compute costs? Verdict: ignored
No sign/verify/encaps timing is modelled or measured. Compute is only cited: ML-KEM-512 encapsulation "1–3 ms on automotive-grade AURIX platforms [17]" used to justify the 5 ms stagger (Sec. 3.4.1); T_FC = 1 ms justified from [17] (Sec. 5.1.2); pqm4 [38] cited only as evidence that PQC is feasible on MCUs (Sec. 2.3); FN-DSA signing called slower than ML-DSA qualitatively (Sec. 6.1). Sec. 6.5 lists "Transport-only PQC workload (no encapsulation computation)". No Cortex-M numbers at all.

### 6. Compromised ECU holding SecOC keys / source authentication vs shared MAC? Verdict: NO
Adversary is passive HNDL; "The adversary does not inject or modify frames" (Sec. 3.1). SecOC appears once, in the introduction: "Current in-vehicle authentication relies on symmetric MACs under the AUTOSAR Secure Onboard Communication (SecOC) framework [16]; a transition to PQC-based asymmetric schemes would require transporting substantially larger cryptographic artifacts" (Sec. 1, p. 2), and once as a step after key establishment ("SecOC session activation", Sec. 3.8). The insider-with-shared-MAC-key problem that motivates A4 is never raised.

### 7. Ranking FN-DSA vs ML-DSA vs SLH-DSA; depends on amortisation? Verdict: YES ranks, NO amortisation dependence
Ranking by single-shot PK+Sig frame count: FN-DSA-512 (6F/0B/0I) > ML-DSA-44 (2F/4B/0I) > SLH-DSA-128s (0F/1B/5I) (Table 7b, Fig. 4; Sec. 5.2.2–5.2.3, 6.4: "FN-DSA-512 ... is the recommended DSA choice for CAN-FD"). The ranking is entirely size-driven per single signature; amortisation is not considered, so whether the ranking flips under amortisation (A4's conjecture) is untouched. The only counterweight they mention is FN-DSA signing cost (Sec. 6.1), not quantified.

### 8. Future work proposes amortisation/batching? Verdict: NO
Sec. 7 future work: vehicle traces, HIL bench, multi-bus/zonal, protocol-level modelling, more signature candidates, adaptive reserved windows, CAN XL + CANsec, (STmin, BS) sweep. No batching, amortisation, Merkle, TESLA or periodic-frame authentication. Near-term competitor risk from this group is on CAN XL/CANsec re-evaluation and HIL validation, not on A4's mechanism. (Inference about the group's plans beyond what is written is not possible from this paper.)

### 9. Bottom line for A4
Already covered by this paper (KNOWN, cite it): single-shot ISO-TP frame counts and bus-time model for ML-DSA, FN-DSA, SLH-DSA on CAN-FD 1/5 Mbit/s with 64 B frames; a validated contention/jitter simulator with SAE-class periodic background load at 20–80 %; the single-shot ranking FN-DSA-512 > ML-DSA-44 > SLH-DSA-128s; reserved-window/stagger mitigation.
Open as far as this paper goes: (a) per-message or per-window source authentication of periodic sensor/control frames; (b) Merkle-root or TESLA-anchor amortisation over N frames and the per-frame authentication-path overhead; (c) a deadline on authentication (e.g. 10 ms) and finding N*; (d) sign/verify compute on a Cortex-M ECU combined with bus cost; (e) an insider/compromised-ECU adversary holding SecOC MAC keys; (f) whether the FN-DSA advantage vanishes or flips under amortisation. A4 should cite this paper as the transport baseline and position itself as the periodic, amortised, active-insider complement. Collision status for A4 against this paper: no collision on the core idea; the risk raised from the abstract alone is cleared by the full text.
