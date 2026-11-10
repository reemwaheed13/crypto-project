# HHE Kombat: Benchmarking Hybrid Homomorphic Encryption Schemes

Direction: D3 (nearest neighbour after the full-text check)

## Citation
- Title: HHE Kombat: Benchmarking Hybrid Homomorphic Encryption Schemes
- Authors: Hossein Abdinasibfar, Camille Nuoskala, Antonis Michalas (Tampere University)
- ID: IACR ePrint 2026/1314 (local PDF `2026-1314.pdf`, 23 pages). The PDF header gives no venue, so treat it as an ePrint preprint until a venue is verified.

## Reading depth
**Targeted sections of the local full text** (read by the orchestrator, not a paper-reader agent): abstract, §1 motivation, the benchmarking environment (pp. 5–6), the client-side SKE.Enc() results and Tables 4–9 summary text (pp. 11–12), "Insight #2", the Table 15 summary (p. 13) and the conclusion (p. 17). Server-side Tables 10–14 were skimmed. Read the rest before citing anything beyond the points below.

## Core claim
A unified, reproducible benchmark of 218 open-source HHE implementations from 19 frameworks: 184 client-side symmetric-encryption (SKE.Enc) and 134 server-side transciphering (HHE.Decomp) benchmarks, compared at a normalised 128-bit target.

## Platform
- **All measurements on an Intel Xeon Gold 6130 @ 2.10 GHz with 128 GB RAM, Ubuntu** (p. 5–6). The setup uses a fixed CPU frequency, process pinning and RDTSC cycle counts (p. 10).
- **Client-side numbers are also on this Xeon. Nothing runs on an MCU, embedded board or phone.** IoT and constrained devices appear only as motivation (§1, p. 4).

## Numbers that matter for D3
- Client-side winners (Table 15, p. 13):
  - F2 ciphers: FRAST, YuX2-8-16, FiLIP-ind, Kreyvium-12.
  - Fp ciphers: Hera-5, Rubato-2, YuXp-16, Pasta-4v2, Masta-5. The top Fp ciphers are "several orders of magnitude" more efficient than the F2 ones on the client.
- Insight #2 (p. 13): TFHE encryption is a poor fit for constrained clients. F2 ciphers improve on it, and Fp schemes (Hera, Rubato-2, YuXp-16, Pastav2) give the best throughput/memory trade-off.
- Table 9 (p. 13): TinyAES SKE.Enc is 4.62×10³ cycles for 16 B, 7100.7 KB/s, 56 KB memory. The Fp table on p. 12 gives HR5 (128 elements, 28-bit) at 2.41×10⁵ cycles and 3808.9 KB/s.
- **Internal inconsistency:** the prose says HR5 is "approximately 2× faster than TinyAES", but the two tables give TinyAES about 1.9× *higher* throughput than HR5. Do not quote the prose claim. The table numbers agree in direction with SoK 2025/669 Table 4, where AES-SW (1.86×10³ cycles/block, 116 C/B) beats HERA-5 (2.86×10⁴ cycles/block, 511 C/B) on the client. On both Xeon benchmarks, then, plain AES is the cheapest client cipher per byte.

## Convenient assumptions
- Server-class client CPU with unlimited RAM; memory is reported as process memory, not peak stack/heap on a small device.
- No radio and no communication energy; client "cost" is compute only.

## Limitations / future work (paraphrase)
Future work is systematic HHE parameter selection and real-world deployment (conclusion, p. 17). It does not mention MCUs or embedded clients.

## Relevance to D3 (collision assessment)
- **KNOWN now:** client-side SKE.Enc rankings exist for many HE-friendly ciphers, on Xeons (this paper and SoK 2025/669 Table 4). The server-optimal cipher is not always the client-optimal one (Rubato).
- **Open as far as searched:** the same ranking on a Cortex-M class MCU (cycles, peak RAM, flash, energy). Also open is whether it changes from the Xeon ranking (32-bit core, no AVX2 Keccak, software 64-bit modular arithmetic, kB-scale RAM), and the radio-inclusive crossover against AEAD for small sensor frames.

## Possible seeds
- **Move to device:** re-run this paper's client set on QEMU Cortex-M33 and compare rank orders.
- **Account for the whole system:** add radio bytes per frame (802.15.4), because Xeon "throughput" ignores ciphertext expansion on the air.
