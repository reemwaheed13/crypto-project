# Batch Signatures, Revisited

## Citation
- Title: Batch Signatures, Revisited
- Authors: Carlos Aguilar-Melchor, Martin R. Albrecht, Thomas Bailleux, Nina Bindel, James Howe, Andreas Hülsing, David Joseph, Marc Manzano. All are at SandboxAQ except Hülsing (TU Eindhoven).
- ID: IACR ePrint 2023/492, received 2023-04-04.
- Venue, as verified:
  - The local PDF (11 pages) has no venue in its header or footer.
  - The ePrint landing page (https://eprint.iacr.org/2023/492) gives the publication info as "Preprint".
  - A published version exists at CT-RSA 2024, LNCS 14643, pp. 163–186, DOI 10.1007/978-3-031-58868-6_7. Sources: a web search (bibsonomy/bibbase/unpaywall) and the SandboxAQ publications page, which says "CT-RSA 2024". I did not open the Springer text.
  - The CT-RSA version is 24 pages, so it is probably longer than the 11-page ePrint version read here. A search-engine summary says the published abstract reports about 4.6x for SPHINCS+-128. That figure is UNVERIFIED and does not appear in the ePrint text I read.
- Reading depth: full text (local PDF, ePrint version, all 11 pages, references included).

## Core claim
If you sign one Merkle root over N randomised, index-tweaked messages with a single "inner" signature (e.g. Falcon-512), signing becomes several times faster in throughput. The costs are a logarithmic size overhead (82–98 B) and a moderate latency increase. The construction is proven EUF-CMA secure assuming only target-collision resistance of a tweakable hash, not full collision resistance.

## Platform
- Hardware: server/PC only. The server is one Google Cloud c2-standard-30 (Intel Cascade Lake, 3.9 GHz, 30 vCPU, 120 GB RAM, 32 Gbps egress, HyperThreading disabled). There are 1–4 identical client machines (Sec 7.2).
- No microcontroller, embedded or emulator measurements of any kind.
- Software:
  - Rust `bsign_engine`, which does asynchronous tree building and has a fixed number of signing threads.
  - OpenSSL 1.1.1 fork with liboqs; `ring` for ECDSA (Sec 7.1).
  - Compiler flags are not reported.
- Parameter sets:
  - ECDSA P-256, Dilithium2, Dilithium5, Falcon-512 and Falcon-1024.
  - Falcon also in "fpemu" variants, i.e. emulated floating point (Tables 1–2).
  - SPHINCS+ is deferred to the "full version" (Sec 7.2).
  - Batch sizes N = 16 or 32 only.
- Intro cycle counts (Sec 1) come from SUPERCOP on a Core i7 Tiger Lake, not from the authors' own measurements. Signing costs 85K cycles for EdDSA-type ECDSA, 272K for Dilithium, 570K for Falcon and 25M for SPHINCS+.
- FLAG: every cost was measured on a server. No device cost is given.

## Threat model
- Adversary: classical or quantum (PPT/BQP) with chosen-message access to a batch-signing oracle (BEUF-CMA, Fig 3 / Def 3.2).
- Protected:
  - Unforgeability of individual messages (Thm 5.1).
  - Weak batch privacy (Def 3.4 / Thm 5.2): signatures from the same batch reveal nothing about a message whose signature is withheld.
- Not achieved: full BATCH-PRIV. Being able to tell that two signatures came from the same batch is not hidden; the shared id and root are visible (Sec 3).
- Out of scope:
  - Side channels, apart from a note on constant-time floating point for Falcon (Sec 2.3).
  - Denial of service and availability.
  - Replay and freshness.
  - Protocol-level authentication beyond the TLS integration.

## Numbers that matter
- Table 1, signature sizes (bytes). The overhead |sig| − |σ| equals the sig_c column (the size when the root signature is cached):
  - Falcon-512, N=16: 666 → 748 (+82).
  - Dilithium2, N=32: 2420 → 2518 (+98).
  - Falcon-1024, N=32: 1280 → 1378 (+98).
  - ECDSA P-256, N=32: 64 → 162.
  - Going from N=16 to N=32 adds 16 B. This is consistent with 16-byte tree nodes, i.e. one 16 B node per tree level. That is my inference; the paper does not give the hash output length explicitly.
- Table 2, TLS on the 30-core server. Columns: handshakes/s, latency med/p90/p99 in ms, signing cores.

  | Scheme | Plain | MT batch |
  |---|---|---|
  | Falcon-512 | 28,000/s; 1.1/1.3/1.5 ms; 7.8 cores | N=16: 43,000/s; 1.5/1.8/2.5 ms; 2.0 cores |
  | Dilithium2 | 29,000/s; 1.6/1.8/2.1 ms; 2.9 cores | N=32: 50,000/s; 1.8/2.2/2.7 ms; 1.0 core |
  | Falcon-512-fpemu | 5,000/s; 5.1/5.2/6.0 ms; 20 cores | N=16: 16,000/s; 6.4/7.6/8.4 ms; 8 cores |
  | Falcon-1024-fpemu | 2,600/s; p99 11.0 ms | N=16: 8,200/s; p99 17.0 ms |

- Abstract: 3.2x connections/s for Falcon-512, +14% signature size, +25% median latency. Note that the Table 2 non-fpemu Falcon-512 row gives 43k/28k ≈ 1.5x. The 3.2x matches the fpemu row (16k/5k). The abstract does not make clear which row it means.
- Sec 2.3: constant-time emulated floating point costs Falcon about 20x (citing Pornin [19]).
- Sec 6: amortised cost is fewer than two hashes per message plus one signature per batch.
- Sec 6.2.2: caching the root signature saves 1–3 KB per certificate for Falcon/Dilithium and up to 30 KB for SPHINCS+.

## Convenient assumptions
- Clients arrive at high rate, so trees fill quickly. Latency is measured under saturating load.
- Cores are plentiful (signing threads are reserved explicitly). Memory and radio/bus cost are not considered.
- Power-of-two N. Partial trees are padded or use L-trees, described as "standard" and left out (Remark, Sec 4).
- The verifier is assumed to be updated to handle batch signatures.
- The proof loses a factor q_s (it is not tight). The authors argue informally that the per-tree id removes multi-instance gain, but leave this unproven (Remark after Thm 5.1).

## Stated limitations and future work
- Hard PQ HSM figures are not available, so no HSM throughput estimate is given (Sec 6.1.1).
- SPHINCS+ results and an HSM benchmark are deferred to a full version (Sec 7.2).
- A tight analysis without the q_s factor is left to future work.
- Trees larger than 32 were not benchmarked because "for large trees the latency added is unrealistic" (Sec 7.2).

## Load-bearing references
- [2] Benjamin 2020, draft-ietf-tls-batch-signing-00. The origin of the construction.
- [4] Benjamin, O'Brien, Westerbaan 2023, Merkle Tree Certificates for TLS (draft).
- [5] Bernstein et al., SPHINCS+ framework, CCS 2019. Source of the tweakable hash and SM-TCR definitions.
- [9] Fregly et al., Merkle Tree Ladder Mode, ePrint 2022/1730.
- [19] Pornin 2019, constant-time Falcon, ePrint 2019/893. Source of the fpemu ~20x figure.

## Possible seeds (direction-miner moves)
- Change the setting from server to device: batch signing on a constrained ECU/MCU, where signing cost per core matters more and a 20x fpemu penalty applies on FPU-less cores.
- Change the objective from throughput to a deadline: the paper optimises handshakes/s; a real-time bus needs N chosen so that window fill time plus signing time stays within a deadline.
- Missing scheme: SPHINCS+ is deferred in the ePrint version, so check the CT-RSA version before claiming it is absent.
- Change the bandwidth model: the cached-root trick (Sec 6.2) maps onto a broadcast bus where receivers see every frame of a window.
- Revisit the ranking: Table 2 suggests batching narrows the Falcon vs Dilithium signing gap (both get to 1–2 cores), but the paper never states a ranking.

## Relevance to A4
1. Construction (KNOWN, Sec 4, Alg 1–2):
   - Messages and randomisation:
     - A fresh random tree id (λ bits) is drawn per batch.
     - Each leaf is H(id, 0|i, r_i|μ_i), with a fresh λ-bit r_i per message. The tweak carries the leaf index i.
     - Internal nodes are H(id, 1|(k+1)|j, left|right).
     - The inner scheme signs id|root|N.
   - Per-message signature is (id, N, σ, i, r_i, path_i), with |path| = log2 N nodes. Overhead is 82 B at N=16 and 98 B at N=32 (Table 1).
   - Security notions:
     - BEUF-CMA ≤ EUF-CMA(S) + q_s·SM-TCR(N) (Thm 5.1). Collision resistance is not needed, which is what allows the smaller node size.
     - Multi-target caveat: the bound has a non-tight q_s factor. The per-tree id is argued, but not proven, to stop multi-instance attacks.
     - Only weak batch privacy is achieved.
2. Settings: TLS server signing, HSM certificate issuance, fleets of load balancers, and Envoy mTLS meshes (Sec 6). The paper does not mention embedded systems, real-time, vehicles, CAN, IoT or deadlines (full-text read). Sec 2.3 notes that many CPUs lack constant-time floating point, citing a Cortex-M7 benchmark [13], but this is not developed further.
3. Cost vs batch size:
   - Only N ∈ {16, 32} was measured, in TLS on a 30-core Cascade Lake server (Table 2). There is no sweep over N and no device cost.
   - There is no explicit Falcon vs Dilithium vs SPHINCS+ ranking under batching. SPHINCS+ is deferred.
   - Table 2 implicitly shows plain Falcon-512 needing 7.8 cores against 2.9 for Dilithium2, while batched both need 1–2 cores, at 43k vs 50k handshakes/s.
4. Waiting for a batch means extra latency:
   - Larger trees reduce per-client cost but raise median and worst-case latency (Sec 7.1).
   - With one signing thread, worst latency is about twice one signing time (Sec 7.1).
   - Trees were capped at 16/32 because larger ones add "unrealistic" latency (Sec 7.2).
   - The HSM agent signs whenever the signer is ready and pads with zero leaves. So batching is opportunistic and driven by signer readiness, not a fixed window (Sec 6.1.1).
   - Measured p99 increases are about 1 ms for fast schemes and up to about 6 ms for slow ones (Table 2 caption).
   - Fill time under periodic, low-rate sources is not modelled.
5. Bottom line:
   - A4 can cite as KNOWN: the Merkle batch-signature construction (index-tweaked, randomised leaves, TCR-based security proof, log2 N path overhead) and the throughput/latency trade-off on servers.
   - Left for A4, open as far as this paper goes (still needs a literature search, including the CT-RSA full version and later citing works):
     - Cortex-M ECU cost, including FN-DSA without a floating-point unit (fpemu).
     - Periodic CAN FD frames, where window fill time is set by the frame period.
     - The 10 ms deadline and a bus-load cap.
     - Choosing N* under those constraints.
     - Any ML-DSA / FN-DSA / SLH-DSA ranking under amortisation.
