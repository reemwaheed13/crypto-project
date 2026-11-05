# CS 5803 Applied Cryptography — software project (LUMS, Fall 2026)

## Goal
Find a novel, publishable question in IoT cryptography/privacy (PQC, FHE, or federated learning)
that our group can answer in software this semester AND continue as a DRP afterwards
(phase 2 = validate on the EFR32xG24 dev kit, then side-channel analysis of our own implementation).

## Instructor's framing (from the course email, 20 Oct 2026)
- Pick an IoT application that needs a crypto/privacy primitive (PQC, FHE, or federated learning).
- First define the application and what we are protecting against; that decides what to measure.
- Software teams need no hardware: run the primitive on a PC or simulator, then estimate the cost on a
  typical IoT MCU (emulation, cycle or operation counts, memory, ciphertext/key sizes).
- Example: FL on IoT sensor data — how much an update leaks about raw readings (gradient leakage), and
  what DP or secure aggregation costs on a constrained device in accuracy, memory, energy, communication.
- Side-channel follow-ups come later. Groups may use LLMs to find gaps and should aim for novel work.

## Working rules
- Threat model before experiments. Every direction states: application, asset, adversary, what is measured.
- Never claim a gap is open. Say "open as far as searched" and list the queries.
- Separate KNOWN (cited), CONJECTURE (ours), ASSUMPTION in every claim.
- Emulator numbers are estimates. Label them and say how they were calibrated.
- Explain theory in plain words, not code.
- No optional experiments in plans.

## Layout
- research/          reports from the skills (direction mining, collision logs, cost estimates)
- research/papers/   one note per paper read (written by the paper-reader agent)
- experiments/       code, one folder per direction
