# Decision summary: D2 vs D3 vs A4 (2026-11-10)

This compares the three front-runners after the full-text checks of 10 November, to help the group pick one.
Details, citations and query logs are in the two direction reports and the files named below. Labels follow the
project rules: **KNOWN** = from a cited source we have read, **CONJECTURE** = our hypothesis, **ASSUMPTION** = a
modelling choice. "Open" always means *open as far as searched*, never proven open.

## The three directions in one line each

- **D3: Which cipher is cheapest for a sensor that sends data to an encrypted-computing cloud?**
  A sensor scrambles its readings with a special "HE-friendly" cipher so that the cloud can later turn them into
  homomorphically encrypted data and compute on them without seeing them. We measure what each cipher costs the
  *sensor*, on a Cortex-M33 like the one on our dev kit.
- **D2: What does it cost a sensor to stop the federated-learning server from rebuilding its data?**
  Smart meters train a shared forecasting model and upload model updates. A curious server can partly rebuild a
  household's load curve from one update. Adding noise stops this, but costs accuracy and battery. We measure how
  much noise is enough, and what it costs on the chip.
- **A4: Can a car prove *which* control unit sent a message, in a way that survives quantum computers?**
  Car control units share secret keys today, so a hacked unit can pretend to be the brakes. Post-quantum signatures
  would fix this, but they are far too big for a 64-byte CAN FD frame. So we sign a batch of frames at once, and ask
  how big the batch can be before the receiver has to wait too long.

## Side by side

| | **D3** (client cost of HE-friendly ciphers) | **D2** (on-device cost of stopping gradient inversion) | **A4** (batched PQ source authentication on CAN FD) |
|---|---|---|---|
| **Area** | FHE, in scope | Federated learning, in scope | PQC, in scope |
| **Still open after full-text checks** | (1) The cipher ranking *on a Cortex-M33*, and whether it differs from the ranking on a server CPU. (2) The payload size below which the clever cipher costs the sensor more battery than ordinary encryption, once radio cost is included. | The battery, time and memory cost *per training step on a Cortex-M33* of the noise level that actually stops a *learned* reconstruction attacker on smart-meter data, with a proper privacy budget, and the accuracy lost. | Signing a *window* of periodic CAN FD frames with one PQ signature, with the window size chosen by both a delivery deadline and a bus-traffic cap, including chip signing time, and which PQ scheme wins once costs are shared across the window. |
| **What turned out to be already done** | Ciphers have already been ranked by *client* cost, on Intel Xeons (SoK ePrint 2025/669, Table 4; HHE Kombat, ePrint 2026/1314). "The best server cipher is not the best client cipher" is already published. | Noise against time-series reconstruction is studied (Sudars et al., Applied Sciences 2025; TS-Inverse Table IX). Membership leakage of on-device sensor models is measured, but on a desktop (MDPI MAKE 2026). | Signing every message on CAN / CAN FD (PQ-CAN; Applied Sciences 16(8):3705). Generic batch signing for web servers (ePrint 2023/492). Symmetric source authentication against a hacked ECU (CAIBA, EuroS&P 2025). |
| **Collision verdict** | Partial overlap (was "Open" before reading the full texts) | Partial overlap | Partial overlap (the flagged risk paper app16083705 has no batching at all) |
| **Updated score / 30** (2026-11-11, after all three kill tests) | **25**: novelty 2, threat model 4, falsifiable 4, feasible 5, DRP 5, split 5. The salvage survives only for Pasta-4 against seeded secret-key BFV | **27** with N1 merged in (was 26): novelty 3, threat model 5, falsifiable 5, feasible 5 (kill test ran on CPU in about 3 h), DRP 4 (5 if the N1 sampler side-channel phase 2 is accepted), split 5 | **23** (was 25): novelty 3, threat model 3, falsifiable 5, feasible 4, DRP 3, split 5 |
| **Biggest remaining risk** | The chip ranking may simply copy the server ranking, because the same reference code has the same bottleneck (random-number generation). Then only the radio crossover is left. | The noise step may turn out to be almost free next to training itself. Then the "cost" half of the story is thin, unless the random-number generator or per-example memory turns out to be expensive on the chip. | **Motivation.** CAIBA already stops a hacked ECU without signatures. We must convince a reviewer that signatures are worth it: no trusted extra node on the bus, proof for forensic logs, and keys that last the car's lifetime. |
| **Kill experiment status** | **Run 2026-11-10: failed on both claims.** Cortex-M33 ranking matches the Xeon ranking (Spearman ρ = 0.976, 8 settings). No crossover against Ascon: every HE-friendly cipher costs 1.2× to more than 10⁴× more at every payload size. See `cost-he-friendly-ciphers-2026-11-10.md`. | **Run 2026-11-10/11: survives.** No defence: the learned attacker (LTI) leaks under 5-step FedAvg, B = 4 (sMAPE 0.25 against the prior's 0.45). At ε = 8 the optimisation attack is defeated, but LTI is still about 2 SE better than the prior, at a cost of 1.3 sMAPE points. At ε = 1 both attacks are defeated (LTI is within about 1 SE of the prior), at a cost of 5.4 sMAPE points. On the M33, the DP step costs 4.85× the plain step, and 66% of it is noise. See `d2-kill-2026-11-10.md`. | **Run 2026-11-10: claim dead.** 0 of 96 configurations meet 10 ms: signing alone takes ≥ 16 ms (ML-DSA-44 at 240 MHz). Batching works only for deadlines of about 100 ms, where ML-DSA-44 beats FN-DSA. CAIBA has no delay, according to its abstract. See `a4-kill-canfd-window-model-2026-11-10.md`. |
| **DRP path** | **Strongest.** Run the winning cipher on the real EFR32MG24 kit, measure real battery use, then a power side-channel study of the cipher's keystream, an active research topic. | Good. Run the defended training step on the kit and measure real energy. The side-channel follow-up is weaker, because this is a machine-learning privacy study, not a crypto primitive. | **Weakest on our hardware.** The EFR32MG24 has no CAN controller (datasheet), so the bus part stays simulated unless we get a CAN FD board. The signer's side channel does transfer to the kit. |

## Plain-words explanation of each remaining question

**D3.** A tiny sensor pays for two things: thinking (CPU cycles) and talking (radio bytes). Our datasheet numbers
say that sending one byte at 0 dBm uses about as much energy as 3,400–5,000 CPU cycles (KNOWN from the datasheet
values; the conversion is our arithmetic). Some HE-friendly ciphers already spend thousands of cycles per byte on a
server CPU. On a much weaker chip that is plausibly *more* than the cost of just sending the byte (CONJECTURE). So the
supposedly lightweight cipher could be the main battery drain, and the chip also has a free hardware AES unit as a
competitor. Nobody has measured this on a microcontroller.

**D2.** Earlier attacks guessed the data by trial and error. The newer, stronger attacks *learn* how to undo the
update, and they still work when noise is added (TS-Inverse, Table IX). So a fair test must train such a learned
attacker against our noisy updates. The amount of noise needed against it is the amount the sensor must pay for, and
that payment on a Cortex-M33 has not been measured.

**A4.** Batching means waiting. A receiver can only trust a frame once the signature for its batch arrives. Bigger
batches mean less bus traffic but longer waiting, and a brake controller cannot wait long. Somewhere there is a
largest safe batch size. We also expect that once the signature is shared across many frames, the scheme with the
smallest signatures (Falcon) loses its main advantage (CONJECTURE). A new low-memory Falcon (ePrint 2026/1915) makes
that less certain.

## Recommendation

> **Final update 2026-11-11 (all three kill tests run, plus the niche round).**
> - **D3:** both original claims failed. The salvage collision check (`collision-r3-D3s-transcipher-vs-directHE-crossover.md`,
>   open as far as searched) finds the reversed crossover only for expensive F_p ciphers such as Pasta-4, against
>   seeded, fully packed, secret-key BFV (Zeppelin: N = 4096, 4,096 slots, 65,572 B; full text). Break-even is about
>   2.3–6.4 kB per upload. HERA-5 and Kreyvium win at every size, and SEAL-Embedded's ciphertext size is unverified.
>   Re-scored **25**.
> - **A4:** dead as worded (0/96 configurations meet 10 ms). It survives only as a niche of signals that tolerate about
>   100 ms, and CAIBA covers the safety-critical case. **23**.
> - **D2:** survives its kill rule. Privacy against a learned attacker is not free at ε = 8, accuracy loss exceeds
>   1 point, and DP costs the chip 4.85× per step. The niche round (`direction-mining-niche-2026-11-10.md`) adds a
>   crypto-engineering core. The float32 Box-Muller noise D2 pays for can be broken by a pair-inversion attack in
>   single-step releases (96% single-release accuracy on the M33 code path), and a sound constant-time sampler costs about
>   8–17× more. **27** with N1 merged.
>
> **Recommendation: commit to D2, with N1's sound-noise-sampler work as one of its four work packages.** In plain words,
> we study what it really costs a smart meter to keep its readings private while helping train a shared model.
> - That means measuring how much noise stops a learned attacker, how much accuracy it costs, and what the chip pays.
> - We also check that the noise the chip generates is actually safe, and price the safe version.
> - Phase 2 measures real energy on the EFR32 kit, then attacks our own noise sampler with power analysis.


> **Update 2026-11-10 (D3 kill experiment run).** Both D3 checks came out flat: same ranking as the Xeon (ρ = 0.976), and
> no AEAD crossover (the HE-friendly ciphers cost more at every size). D3 is re-scored to **26** (novelty 2). By the rule
> below, **switch to D2**. One untested salvage for D3 is a *reversed* crossover against direct BFV/CKKS encryption. It
> needs its own collision check. Details: `cost-he-friendly-ciphers-2026-11-10.md`.

**Run D3's kill experiment first** (about one day once approved). D3 still scores highest, has the best hardware
continuation on our kit, and its harness is already written. Pick by outcome:
- If the chip ranking differs from the Xeon ranking, or the radio crossover is real, **commit to D3**.
- If both come out flat, **switch to D2**. Run its sharper week-1 test first (learned attacker plus a proper privacy budget).
- Keep **A4** as the fallback. Only choose it if the group is comfortable defending "why signatures, given CAIBA",
  and accepts that the CAN part will be simulated.

## Open items before committing (from the full-text pass)

- **Kit part number:** taken from Zephyr's board page (EFR32MG24B310F1536IM48-B), because the kit user guide UG524 is
  not in `research/papers/` and the Silicon Labs download returned an error. Please add UG524 and confirm.
- **Missing PDFs** (not in `research/papers/`, so checked from abstracts or landing pages only):
  - D2: VAHANA ePrint 2026/1936; arXiv 2609.02971.
  - A4: IETF MTL draft; SAE 2025-01-0451; ePrint 2026/1915; LiBrA-CAN.
  - New this round: Sudars et al. 2025 (D2); CAIBA arXiv 2504.16695 and arXiv 2611.08108 (A4).
- **D2:** a ResearchGate item claiming DP tests on a Cortex-M7 could not be verified.

## Files

- Direction reports: `direction-mining-2026-11-05.md` (D2 and D3 cards), `direction-mining-round2-2026-11-05.md` (A4 card, combined ranking).
- Collision re-checks: `collision-r3-D2-sensor-gradinv-defence-cost.md`, `collision-r3-A4-canfd-amortised-pq-auth.md`.
- New paper notes: `papers/make8100320-ondevice-not-private.md`, `papers/app16083705-pqc-canfd-design-space.md`,
  `papers/batch-signatures-revisited.md`, `papers/hhe-kombat.md`. Full-text addenda were added to the TS-Inverse, GIA SoK,
  on-device-training, PQ-CAN and transciphering-SoK notes.
