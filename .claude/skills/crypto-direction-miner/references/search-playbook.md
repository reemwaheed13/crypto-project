# Search playbook

## Where to look (in this order)
1. IACR ePrint (eprint.iacr.org) — the real frontier for crypto engineering. Search by keyword, sort by date.
2. TCHES / CHES — embedded and side-channel work, pqm4-style benchmarks.
3. USENIX Security, ACM CCS, IEEE S&P, NDSS, PETS/PoPETs, ACNS, ESORICS, CARDIS, AsiaCCS.
4. For FL privacy: NeurIPS, ICML, ICLR, plus arXiv cs.CR and cs.LG.
5. Code and benchmarks as evidence of what is already done: pqm4 / mupq, PQClean, liboqs,
   mlkem-native, OpenFHE, Microsoft SEAL, Lattigo, TFHE-rs, Flower, Opacus.

## Query patterns (fill in the brackets)
- "[primitive] Cortex-M" / "[primitive] microcontroller" / "[primitive] IoT energy"
- "[primitive] memory footprint" / "stack usage [primitive] embedded"
- "post-quantum [protocol] IoT" (TLS, DTLS, EDHOC, OSCORE, Matter, Thread, BLE pairing, LoRaWAN)
- "[primitive] 802.15.4 fragmentation" / "ciphertext size constrained network"
- "secure aggregation post-quantum" / "lattice secure aggregation" / "secure aggregation microcontroller"
- "gradient inversion time series" / "gradient leakage sensor data" / "federated learning IoT privacy attack"
- "differential privacy federated learning microcontroller energy"
- "transciphering IoT" / "HE-friendly cipher microcontroller" / "CKKS client encryption embedded"
- "[primitive] side-channel Cortex-M33" (for DRP phase 2)
- Always add a date filter or "2025" / "2026" to one variant of each query.

## Collision-check discipline
At least four queries per candidate: technical phrasing, plain-language phrasing, ePrint-only search,
and one aimed at a neighbouring community (networking, embedded systems, ML). List every query.
