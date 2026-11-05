---
name: crypto-direction-miner
description: Mines recent applied-cryptography and privacy literature (IACR ePrint, TCHES/CHES, USENIX Security, CCS, S&P, NDSS, PETS, plus ML venues for federated-learning privacy) to generate novel, collision-checked research directions for the CS 5803 Applied Cryptography software project on IoT crypto/privacy (post-quantum crypto, FHE, federated learning on constrained devices), each with a threat model, a falsifiable claim, a cheap kill experiment, an MCU cost plan and a post-semester DRP continuation. Use this skill whenever the user asks for project ideas, gaps, novelty checks, related work, or "what should we build" for the crypto project, mentions PQC/ML-KEM/ML-DSA/FHE/CKKS/TFHE/secure aggregation/gradient leakage/differential privacy on IoT or microcontrollers, or wants to refresh or extend an earlier direction report, even if they don't say "skill".
---

# Crypto direction miner (CS 5803 IoT crypto project)

Turn recent papers into 2–4 defensible project directions. Each must name an IoT application, a threat
model, a measurable claim, a first experiment that could kill it within a week, and a phase-2 plan that
makes it a DRP. The goal is a question a small undergraduate team can own, not a survey.

## Why the workflow is shaped this way
Two failure modes dominate. (1) A "gap" from a future-work paragraph that a 2025–26 ePrint already filled;
crypto engineering moves fast and ePrint is the real frontier, not conferences. (2) A direction with no
threat model, so nobody knows what to measure. The instructor explicitly wants the application and the
adversary defined first. So: threat model first, collision checks before scoring, reading depth recorded.

## Step 0: Load context
Read `references/project-context.md` and the repo's `CLAUDE.md`. Newer files in `research/` override them.
If the user names a focus (one primitive, one application, one paper), narrow everything below to it.

## Step 1: Harvest
Goal: 40–80 candidates triaged to 10–15. Use the query patterns in `references/search-playbook.md`.
Cover the three primitive families separately (PQC, FHE, FL-privacy) and then their intersections
(e.g. post-quantum secure aggregation, HE-friendly ciphers on MCUs). Record date searched and venue status
(published / accepted / ePrint-only / arXiv-only). Triage question: does the paper measure or bound a cost
or a leakage on a constrained device, or assume a device it never measures? The second kind seeds projects.

## Step 2: Read the shortlist (delegate)
Spawn the `paper-reader` subagent in parallel, one paper each (batches of up to 5). Each writes
`research/papers/<short-name>.md` with: core claim, platform and what was actually measured (real board,
emulator, or PC only), threat model, convenient assumptions, stated limitations, load-bearing references,
reading depth.

## Step 3: Follow references one hop both ways
Backward: the 3–5 works the result rests on. Forward: who cited it or attacked the same question in the
last 6–12 months (Semantic Scholar, Google Scholar "cited by", ePrint keyword search).

## Step 4: Generate candidates with named moves
Record the move for each candidate:
- **Move to the constrained device:** a result measured only on a PC or server, re-costed on Cortex-M.
- **Change the threat model:** honest-but-curious → malicious server, passive → active, add a quantum adversary.
- **Change the data:** image-centric results (gradient inversion, DP accuracy loss) moved to low-dimensional
  sensor time series, where they may behave very differently.
- **Swap the primitive:** classical piece of a protocol (e.g. DH in secure aggregation) replaced by a PQC one;
  measure what breaks in bandwidth, RAM, or round count.
- **Account for the whole system:** compute-only costs vs. radio energy and packet fragmentation
  (802.15.4/BLE/LoRa frame limits vs. PQC ciphertext sizes).
- **Find the crossover:** the point where a cheaper-looking option stops being cheaper.
- **Attack the defence:** does the proposed defence still leak under a stronger or more realistic attacker?
Every candidate must also state its DRP phase 2 (hardware validation on the EFR32xG24 kit, side-channel
study, or a stronger theorem).

## Step 5: Collision check (delegate)
Spawn the `collision-checker` subagent per surviving candidate, in parallel. It runs at least four distinct
searches (technical phrasing, plain phrasing, ePrint, and the neighbouring community) and returns
Open (as far as searched) / Partial overlap (state what remains) / Collision (name the killer paper).

## Step 6: Score and select
Score 1–5 with one-line justification: novelty after collision check; clarity of threat model;
falsifiability; feasibility in software this semester; DRP continuity; group split (attack, primitive
implementation, MCU cost model, evaluation). Keep the top 2–4.

## Step 7: Output
Save `research/direction-mining-<YYYY-MM-DD>.md` and summarise it in the reply:
1. Bottom line (3–5 sentences).
2. Mining table: paper, status, reading depth, seed, move, candidate or "none".
3. Direction cards using `references/card-template.md`, theory explained in plain words, no code.
4. Dropped candidates with the paper that killed each.
5. Verification log: every paper with ID and verified status; every query run; anything unchecked.

## Honesty rules
- Never invent ePrint/arXiv IDs, titles, authors, venues, cycle counts or quotes. Unverified = say so.
- Keep KNOWN / CONJECTURE / ASSUMPTION visibly separate.
- Emulator-derived costs are "(estimate, calibrated against <source>)" or "(uncalibrated estimate)".
- No publication promises.
- One short quote per source at most; paraphrase the rest.
