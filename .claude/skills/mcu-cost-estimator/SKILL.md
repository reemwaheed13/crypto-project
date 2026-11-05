---
name: mcu-cost-estimator
description: Estimates what a cryptographic or privacy primitive (ML-KEM, ML-DSA, other PQC, FHE client operations, HE-friendly ciphers, secure aggregation, DP noise, on-device training steps) would cost on an IoT microcontroller without physical hardware, using QEMU emulation of Cortex-M boards, instruction counting, static memory analysis, published benchmark calibration, and datasheet energy models. Use whenever the user wants to run, benchmark, measure, cost, profile or estimate cycles, memory, energy or communication of crypto code "on an MCU", "on Cortex-M", "on the EFR32", "on IoT", or asks how to run the analysis for the crypto project.
---

# MCU cost estimator

Produce defensible cost estimates for a primitive on a typical IoT MCU, with every number labelled by how
it was obtained. Emulators are not cycle-accurate; the value of this skill is honest calibration.

## Step 1: Fix the target and the operation set
Target: Cortex-M33 class (EFR32xG24-like) with Cortex-M4 as the comparison point because most published
embedded PQC numbers are on M4. List the exact operations to cost (e.g. keygen, encaps, decaps; or
one FL round = local training + DP noise + masking + key agreement + upload).

## Step 2: Build for the target
- Toolchain: arm-none-eabi-gcc, same optimisation flags as the reference you will calibrate against.
- Prefer existing embedded-ready code (pqm4/mupq, PQClean "clean" variants, mlkem-native) over porting.
- Emulator: QEMU system emulation (mps2-an386 for M4; mps2-an505 or an521 for M33). Confirm the board
  names against the installed QEMU version.

## Step 3: Measure what can be measured
- **Instruction counts:** QEMU TCG plugin (insn counting) around each operation. These are instructions,
  not cycles.
- **Cycles (estimate):** convert with a stated cycles-per-instruction factor, OR better, calibrate: run a
  scheme that pqm4 already benchmarked on real M4 hardware, compute your ratio of measured instructions to
  their published cycles, and apply it to your new code. Report the calibration source and the ratio.
- **Memory:** arm-none-eabi-size for flash/static RAM; -fstack-usage plus stack painting for peak stack;
  heap high-water mark if any allocation.
- **Sizes:** keys, ciphertexts, signatures, model updates, from the spec and from the actual buffers.

## Step 4: Energy model
energy = (cycles / clock_hz) × active_power + bytes_sent × radio_energy_per_byte (+ receive, + sleep).
Take every constant from the EFR32xG24 datasheet and the radio's data rate; cite page/table. Include packet
fragmentation: count frames when a payload exceeds the link's frame size (e.g. 127-byte 802.15.4 frames).
Report compute energy and radio energy separately; the crossover between them is often the finding.

## Step 5: Sanity checks
- Reproduce at least one published number within a stated tolerance before trusting any new number.
- Run each measurement at least 3 times; report the spread.
- Check the code actually ran the intended parameter set (e.g. ML-KEM-512 vs 768).

## Step 6: Output
Save `research/cost-<primitive>-<YYYY-MM-DD>.md` with a table: operation, instructions, estimated cycles,
calibration source, peak stack, flash, message bytes, frames, compute energy, radio energy. Add a plain-words
paragraph on what dominates the cost and why. Put build scripts in `experiments/<direction>/mcu/`.

## Honesty rules
Never present an emulator or converted number as a measured cycle count. Never invent datasheet values or
benchmark numbers; if not found, leave the cell as "not verified".
