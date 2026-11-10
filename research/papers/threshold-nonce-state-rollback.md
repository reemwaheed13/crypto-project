# Threshold-signature nonce state and rollback (TALUS + GKMN21 + RFC 9591)

Lane: B

## Citation
- **Primary:** Leo Kao, Raymond Chang. "TALUS: FIPS-204-Exact Threshold ML-DSA via Boundary Clearance."
  arXiv:2603.22109 [cs.CR], v1 23 Mar 2026, current v5 9 Aug 2026. **Status: arXiv-only** (no venue listed).
  The v5 comment says it is a substantial security revision that incorporates external cryptanalysis. The
  text cites Niot [15], who found two key-recovery attacks on an earlier version.
- **Secondary:** F. Garillot, Y. Kondi, P. Mohassel, V. Nikolaenko. "Threshold Schnorr with Stateless
  Deterministic Signing from Standard Assumptions." CRYPTO 2021, Part I, pp. 127–156. Also IACR ePrint 2021/1055.
  (Called GKMN21 here. The brief called it "Nikolaenko et al.", but Garillot is the first author.)
- **Secondary:** RFC 9591, "The Flexible Round-Optimized Schnorr Threshold (FROST) Protocol for Two-Round
  Schnorr Signatures."

## Reading depth
- TALUS: **full text (arXiv HTML v5), read through an extraction tool.** The tool cut off part of Remark 7.1
  (the "post-session erasure" bullet) and did not return the reference list, so the authors and venues of the
  numbered references were **not verified**.
- GKMN21: **abstract only.** The iacr.org archive PDF returned HTTP 403. I used the ePrint abstract page instead.
- RFC 9591: **full text,** about the first 100k characters, which cover §1–§7.

## Core claim (TALUS)
ML-DSA signing can be thresholdized so that it outputs exact FIPS 204 signatures. The trick is to sign only with
preprocessed nonces that satisfy a "Boundary Clearance Condition," which holds for about 31.7% of nonces at
ML-DSA-65. For those nonces the rejection check can be done offline. The scheme has a TEE-assisted one-round
profile and an honest-majority MPC profile with two online rounds.

## Platform
- Rust implementation covering ML-DSA-44/65/87 in both profiles (§10).
- **PC only:** benchmarked with `cargo bench` on "a single 16-core desktop CPU" (§10). The CPU model, OS, Rust
  version and compiler flags were not given in the text I read.
- No network round-trips and no wall-clock distributed latency were measured (Abstract footnote, §10).
- No MCU, embedded or IoT evaluation appears in the text I read.

## Threat model (TALUS)
- **Adversary:** statically corrupts up to T−1 parties (§2.4). Adaptive or mobile corruption is open; the paper
  says it would need proactive refresh or faster key rotation (§11).
- **Security proven:** EUF-CMA against malicious adversaries in the F_DKG-hybrid model. Transcript privacy is
  proven only for semi-honest adversaries (Appendix F); the malicious proof is deferred.
- **MPC profile:** needs an honest majority, N ≥ 2T−1, and the offline committee must be live (§7.3).
- **TEE profile:** trusts the enclave for both confidentiality and integrity. If enclave confidentiality is
  broken, the key falls out of a linear solve (§6).
- **Out of scope:** concurrent ROS/Wagner-style security (§7.2), full UC composition of the DKG, and
  physical attacks or side channels (not discussed in the text I read).

## Nonce-state requirements (the part relevant to Lane B)
Remark 7.1 lists the "nonce lifecycle" rules as hard requirements. Each one closes a separate key-recovery channel.
1. **Single use, consumed before emit.** A pool entry may be opened for at most one (μ, c) pair. An honest signer
   "atomically marks the entry consumed before releasing" its response share, and refuses any later or
   concurrent request on that entry. The paper warns that a non-atomic check, emit, mark sequence leaves a race
   that reopens nonce reuse.
2. **Crash-surviving rollback protection.** The consumed flag must be persisted with rollback and replay
   protection, for example a monotone counter or an equivalent mechanism. Losing it in a crash would allow
   replay and therefore key recovery.
3. **Quorum binding.** A nonce's shares go only to its owner quorum S, and requests naming another quorum are
   refused. A nonce that two different quorums can reconstruct lets a corrupt coordinator recover the key. To
   choose the quorum at signing time, the signer keeps one pool per quorum, prepared offline.
4. **Post-session erasure.** After an authenticated session-completion signal, the signer erases ŷ_h, ρ_h and
   the Beaver/mask intermediates for that entry. Erasure must happen before any Blame opening, because opening a
   live nonce after z leaks s₁. If a member cannot confirm completion, it treats the entry as live (§7.4).
5. **Session-id freshness.** Every honest party adds fresh randomness to the session id and refuses a repeated
   one (§7.4).
6. **Per-key issuance counter.** A per-key signature counter, kept by a threshold or equivalent mechanism, is
   checked before each signing and enforces a cap of about 2^13–2^14 signatures per key (§8.3, §11).

**Why reuse is fatal.** If the same ŷ is used under two challenges c and c′, then z − z′ = (c − c′)·s₁. A
coordinator holding T−1 shares can therefore solve for s₁ (§7.3). This is the same linear-algebra failure as
Schnorr nonce reuse.

**What TALUS does not specify.** The paper does not name a storage medium, an atomicity mechanism, a counter
implementation, or what happens to a nonce on power loss. Its rollback requirement is stated at the protocol
level only.

**On FROST.**
- §7.2: a one-round MPC variant "in the manner of FROST" under OMDL is not pursued.
- §7.3: in FROST the binding factor recombines independent per-signer nonce contributions. TALUS instead has a
  single threshold-shared nonce with nothing to rebind, so a FROST-style binding factor cannot rescue a reused
  nonce in TALUS.
- FROST is cited [13] for the one-broadcast-per-round model.
- The paper does not analyze FROST's own nonce persistence.

## Numbers that matter (TALUS, PC estimates, §10 and Appendix H)
All timings are local compute on a desktop CPU, not distributed latency.

| Quantity | TEE profile | MPC profile | Notes |
|---|---|---|---|
| Online compute | 0.29 ms | 0.68 ms | ML-DSA-65, T=3 |
| Preprocessing per attempt | 0.39 ms | 1.08 ms | |
| Amortized per signature | ≈1.5 ms | ≈4.1 ms | about 3.15 trials per usable nonce |

- First-attempt success is about 99.6% at ML-DSA-65 over 12,645 attempts, about 98.0% at level 2 and about 99.2%
  at level 5 (Appendix H).
- Any FIPS-204-exact threshold scheme that reveals a summed nonce is open to key recovery after about 2^30
  signatures under one key at ML-DSA-65 (Abstract).
- The query-bounded analysis gives over 105 bits of security at the signing cap. The unconditional
  meet-in-the-middle floor is about 2^58/2^49 (Abstract, §1).
- No memory figures, pool-entry sizes or flash footprint were reported in the text I read.

## Convenient assumptions
- Static corruption only.
- Sequential or bounded concurrency.
- A trusted TEE in the one-round profile.
- An always-available honest-majority committee for preprocessing.
- Atomic, rollback-proof persistent storage, which is required but its cost is not modelled.
- Desktop CPU, with no network, energy or radio cost.
- A key-rotation cap enforced by an external counter.

## Stated limitations and future work (paraphrased)
- The malicious-model simulation proof and online attribution of malformed z_h are deferred to an extended version.
- Concurrency is open.
- Adaptive or mobile corruption is open.
- The protection against the s₂ channel is supported by a model, not a proven bound.
- Level 2 needs a parameterized carry reduction.
- The fully unconditional boxed-nonce case is open (§5.3).
- Signing is capped per key, and the TEE m=1 coalition does not inherit that cap.
- Wall-clock deployment time was not measured.

## Secondary sources (one paragraph)
**GKMN21** (abstract only). The authors argue that Schnorr's per-signature fresh randomness is a major attack
surface in practice, because real randomness sources are often unreliable or need *state continuity*: fresh
state that cannot be rolled back. They name crashes from bugs, malicious actors and **power interruptions** as
ways that continuity breaks. Their answer removes state rather than protecting it. They give a dishonest-majority
threshold Schnorr protocol in which nonces are derived statelessly and deterministically with a standardized
block cipher, and correct derivation is proven with garbled-circuit zero-knowledge plus a UC-commitment trick.
The cost is stated as a "small constant number of exponentiations" per proof. The abstract gives no MCU or
benchmark platform.

**RFC 9591** requires that each nonce:
- comes from secure randomness: 32 fresh random bytes hashed with the secret key in `nonce_generate` (§4.1);
- is "stored locally and kept for use in the second round" (§5.1);
- MUST NOT be used in more than one invocation of `sign` (§5.1);
- is deleted together with its commitment after `sign` completes (§5.2).

The Coordinator MAY also track nonce commitments as a hedge against reuse (§7.3). The RFC says nothing about
crashes, restarts, state rollback or the order of persisting and emitting. It says it does not specify
implementation behavior on failure beyond aborting (§7.4).

## Load-bearing references
- FROST: Komlo and Goldberg; RFC 9591. Cited as [13] in TALUS. The TALUS bibliography entry was not seen.
- GKMN21, CRYPTO 2021 / ePrint 2021/1055: stateless deterministic threshold Schnorr, which avoids rollback by
  design.
- Quorus [3] and Trilithium [11]: threshold ML-DSA schemes that reveal a uniform nonce, the contrast for
  TALUS's summed-nonce attack. Authors not verified.
- Niot [15]: concurrent cryptanalysis of an earlier TALUS version. Entry not verified.
- FIPS 204 [14].

## Bearing on the Lane B candidate (intermittently powered sensors co-signing with FROST)
- **KNOWN:**
  - TALUS states consumed-before-emit and a crash-surviving monotone counter as hard requirements for
    threshold ML-DSA.
  - GKMN21 names power interruption as a cause of lost state continuity in threshold Schnorr.
  - RFC 9591 requires single use and deletion of nonces but gives no crash or rollback guidance.
- **Not found in these three sources:**
  - any analysis of FROST nonce bookkeeping under brownout or flash-write failure on an MCU;
  - any measured cost of crash-safe nonce bookkeeping on a Cortex-M.

  This is open as far as these three documents go. A dedicated literature search is still needed: these were
  not queried here beyond the GKMN21 title search.
- **ASSUMPTION:** a NOR-flash "mark consumed" write (bit-clearing in a pre-erased page) can be ordered before the
  radio transmit. This is to be checked against EFR32xG24 flash semantics.

## Possible seeds (direction-miner moves)
- **Transplant setting:** take TALUS's protocol-level rules (consumed-before-emit, monotone counter) and
  implement them for FROST on an intermittently powered MCU, with brownout injection at every point between
  writing the nonce and emitting the share.
- **Make the hidden cost explicit:** measure the flash writes, wear and latency that crash-safe bookkeeping adds
  per signature. TALUS, GKMN21 and RFC 9591 all leave this cost unquantified.
- **Compare the stateful fix with the stateless fix:** a monotone counter or flash log versus GKMN21-style
  deterministic derivation (or a cheaper hedged variant), costed on Cortex-M.
- **Attack demonstration:** fault-inject a power cut in the emit-then-mark ordering to show nonce reuse and
  key-share recovery on a FROST participant, and show that the fix closes it. This fits phase 2 (EFR32xG24 dev
  kit plus side-channel or fault work).
- **Coordinator-side mitigation:** test whether RFC 9591 §7.3's optional commitment tracking by the Coordinator
  catches device-side replay after a rollback, and what it costs on a gateway.

Sources: https://arxiv.org/abs/2603.22109 ; https://arxiv.org/html/2603.22109v5 ; https://eprint.iacr.org/2021/1055 ;
https://www.rfc-editor.org/rfc/rfc9591.html
