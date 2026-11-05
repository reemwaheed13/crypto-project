# Li & Micciancio: On the Security of Homomorphic Encryption on Approximate Numbers

Lane: A

## Citation (verified)
- Title: "On the Security of Homomorphic Encryption on Approximate Numbers"
- Authors: Baiyu Li, Daniele Micciancio (UC San Diego)
- Venue: EUROCRYPT 2021, LNCS 12696, pp. 648-677, DOI 10.1007/978-3-030-77870-5_23. Also IACR ePrint 2020/1533,
  which describes itself as "a minor revision of an IACR publication in EUROCRYPT 2021". Received 2020-12-08; last revised
  2021-03-07. That revision renamed the notion to IND-CPA^D.
- Status: published (peer-reviewed).

## Reading depth
**Abstract plus secondary sources only. The full text was NOT read.**
- ePrint PDF: HTTP 403.
- Springer: paywalled.
- NSF-PAR copy (par.nsf.gov/servlets/purl/10340712): downloaded, but the PDF text could not be extracted here (no PDF tooling).
  Only the section outline was visible: 1 Introduction ... 5 Experiments (5.1 "Implementation of Our Attack and
  Experiments") ... 6 Conclusion.
- The details of the attack mechanism below come from:
  - the ePrint abstract;
  - the Lattigo security note (github.com/ChainReaction-LTD/lattigo/security);
  - the OpenFHE security docs;
  - abstracts of the follow-up papers.

**Action item:** a group member should open the PDF and fill in the empty fields marked UNVERIFIED.

## Core claim
CKKS decryption returns the message plus the noise (m+e), not m alone. Anyone who sees a decrypted result together with its
ciphertext can solve a linear equation for the secret key. So IND-CPA is the wrong security target for approximate FHE, and
the paper proposes IND-CPA^D, which adds a decryption oracle.

## Mechanism (KNOWN, from the Lattigo note; the paper itself was not read)
- A ciphertext has the form (b, a) = (-a*s + m + e, a). Decryption gives m+e.
- The attacker knows b, a and the decrypted m+e. It computes (b - (m+e)) * a^-1 = s in the ring.
- a is "almost always invertible". If it is not, "only a few more samples are required". The Lattigo note gives no exact count.
- **What the attack needs (as far as verified):**
  - the full decrypted value, which the attacker re-encodes back to the polynomial m+e;
  - the corresponding ciphertext;
  - ideally a single decryption.
- The abstract says the attack runs in "expected polynomial time" and recovers the key "with high probability".
- **UNVERIFIED (needs full text):**
  - how much precision loss in the floating-point decoded output the attack tolerates;
  - exact query counts per library;
  - how outputs of evaluated circuits (as opposed to fresh ciphertexts) are handled.
- **The paper does not appear to consider rounded, thresholded or 1-bit outputs.** This is not confirmed from the full text,
  and nothing in the abstract or the secondary sources suggests it does.

## Platform
- Libraries attacked (from the abstract): HEAAN, SEAL, HElib, PALISADE.
- Computations used in the experiments (from the abstract): mean, variance, and Maclaurin-series approximations of the
  logistic and exponential functions.
- Versions, parameter sets (ring dimension N, modulus), hardware and compiler flags: UNVERIFIED (Section 5 not read).
  This was presumably a PC/server experiment. No MCU or embedded platform is suggested anywhere.

## Threat model
- **Adversary:** passive. It sees ciphertexts and the decrypted results of honestly computed functions. It is not an
  active chosen-ciphertext attacker.
- **Asset:** the CKKS secret key, and with it every other ciphertext.
- **Realistic settings (per OpenFHE docs):** decryption results are shared among several parties, e.g. threshold FHE.
  OpenFHE says its default setting suffices against passive attacks where the normal protocol is followed. This is a
  library position, and the follow-up papers below dispute it in some settings.
- **Out of scope:** active or malformed ciphertexts (IND-CCA), and side channels.

## Numbers that matter
- From the abstract and Lattigo note: about one decryption, plus a few more if a is non-invertible.
- Per-library runtimes and tables: UNVERIFIED.
- Countermeasure costs (KNOWN, from other sources):
  - Lattigo DecodePublic adds noise with expected precision loss "under half a bit".
  - OpenFHE default tolerates N_d = 128 decryptions as a lower bound. CKKS_M_FACTOR raises this to
    N_d * (CKKS_M_FACTOR+1)/2, at the cost of 0.5*(log2(CKKS_M_FACTOR+1) - 1) bits of precision.
  - Ogilvie (ePrint 2025/1618) reports that about 2 bits of precision loss suffices in many practical settings.

## Convenient assumptions
- The attacker sees the full-precision decrypted vector and the exact ciphertext that was decrypted.
- Decryption output is published verbatim, with no post-processing, rounding or thresholding.
- The decrypting party is honest and simply releases the result.
- There is no notion of a device acting on the value. The setting is computation and result sharing (cloud, MPC,
  threshold), not closed-loop control.

## Stated limitations and future work (from the abstract)
- The authors propose indistinguishability-based and simulation-based definitions, restricted variants that limit adversary
  queries, and relations between the notions.
- They "discuss possible modifications to CKKS" as countermeasures. The concrete fix, Gaussian noise on decryption output
  scaled to worst-case error, is developed in the follow-up LMSS22 (ePrint 2022/816, CRYPTO 2022).

## Load-bearing references
- Cheon, Kim, Kim, Song, "Homomorphic encryption for arithmetic of approximate numbers", ASIACRYPT 2017 (CKKS). This is the
  scheme under attack.
- Li, Micciancio, Schultz, Sorrell, "Securing Approximate Homomorphic Encryption Using Differential Privacy", ePrint
  2022/816, CRYPTO 2022. This is the provable noise-flooding fix, with nearly matching lower bounds on the noise. It also
  shows that noise calibrated per ciphertext is attackable.
- Guo, Nabokov, Suvanto, Johansson, USENIX Security 2024, "Key Recovery Attacks on Approximate Homomorphic Encryption
  with ...". It breaks average-case noise estimation in OpenFHE and recovers the key from one shared decryption output.
  Verified via the USENIX PDF title and slides only; the full text was not read.

## Follow-ups (IDs verified on the eprint.iacr.org abstract pages; abstracts only)
- **2024/424:** Bergamaschi, Costache, Dachman-Soled, Kippen, LaBuff, Tang, "Revisiting the Security of Approximate FHE
  with Noise-Flooding Countermeasures". Preprint.
  - Semi-honest attacks on CKKS with noise flooding.
  - Concrete trade-off between the number of decryptions allowed before key refresh, the noise level, and the remaining
    security.
- **2024/853:** Bossuat, Costache, Mouchet, Nürnberger, Troncoso-Pastoriza, "Accurate and Composable Noise Estimates for
  CKKS with Application to Exact HE Computation". CIC 2025.
  - A noise model that matches measured noise within 0.01 bits.
  - An exact-CKKS variant whose decryption removes the noise with high probability, as an IND-CPA-D defence.
- **2024/127:** Cheon, Choe, Passelègue, Stehlé, Suvanto, "Attacks Against the INDCPA-D Security of Exact FHE Schemes".
  ACM CCS 2024.
  - Decryption-failure attacks on BFV (OpenFHE) and TFHE-rs (simulated).
  - The attacks extend to CKKS on discrete data and to threshold FHE.
- **2024/116:** Checri, Sirdey, Boudguiga, Bultel, "On the practical CPAD security of 'exact' and threshold FHE schemes and
  libraries". CRYPTO 2024.
  - Under 1 hour on a laptop, using a few thousand ciphertexts and up to about 1M evaluations or decryptions.
- **2024/203:** Alexandru, Al Badawi, Micciancio, Polyakov, "Application-Aware Approximate Homomorphic Encryption". CIC 2026.
  Application-aware security definitions and configuration guidelines for IND-CPA^D in practical CKKS deployments.
- **2025/045:** Chaturvedi, Chakraborty, Mishra, Chatterjee, Mukhopadhyay, "IND-CPA^C: ... Conditional Decryption in FHE".
  Preprint.
  - The server decrypts only a predicate output.
  - Error-recovery attack leading to key recovery on exact FHE.
  - **Closest found to "partial or thresholded decryption feedback".**
- **2025/395:** Hwang, Hwang, Kim, Lee, Song, "On the (In)security of Approximate Computation Protocols from CKKS".
  ASIACRYPT 2026. Noise smudging does not give simulation security for CKKS-based MPC; they propose collaborative sampling.
- **2025/1618:** Ogilvie, "IND-CPA-D and KR-D Security With Reduced Noise from the HintLWE Problem". ASIACRYPT 2025
  (major revision). About 2 bits of precision loss is enough in many practical settings.
- **2025/1627:** Liu, Wang, Fisch, "IND-CPA-D of Relaxed Functional Bootstrapping ...". CCS 2025.
  - Key recovery in under 10 minutes.
  - Proposes IND-CPA-DR, arguing IND-CPA-D can be insufficient for some applications.
- **2026/285:** Santato, Zanotto, "How (not) to Switch FHE Schemes ...". Preprint. Key recovery on CKKS-to-FHEW switching
  in OpenFHE.
- **2026/2490:** Bian, Choe, Kim, Lee, "Reaction Attack on CKKS Deployments with Asymptotically Optimal Query Complexity".
  Accepted to IEEE S&P 2027.
  - **The client returns a 1-bit validity response.** Combined with public coefficient-to-slot (C2S) evaluation keys, a
    malicious evaluator recovers a ternary key in O(H) queries.
  - Implemented in Lattigo v6.2.0 on four standard parameter sets.
  - No IoT, sensor or actuator setting is mentioned in the abstract.
- **2026/2546:** Choe, Kang, Stehlé, "New IND-CPA-D Attacks on BFV and BGV". ACM CCS 2026. Key recovery in HElib (BGV) and
  OpenFHE (BFV) in minutes to hours.

## Relevance to the Lane A idea (an IoT device acts on a decrypted CKKS result)
- **KNOWN:**
  - LM21 needs the full-precision decrypted value.
  - A 1-bit "accept/reject" reaction oracle on CKKS deployments already exists in 2026/2490 (S&P 2027). It is the nearest
    prior art and is a **major collision risk** for the idea.
  - 2025/045 covers predicate-only (conditional) decryption, but for exact schemes.
- **Open as far as searched:** an IoT or closed-loop actuator setting, where the observable action is a quantized or
  thresholded function of the decrypted value, appears in none of the above abstracts. Queries used:
  - "IND-CPA-D attack 2025 eprint CKKS application deployed threshold FHE decryption oracle practical"
  - "IND-CPA-D attack only one bit of decryption leaked ... CKKS key recovery application setting eprint 2025"
  - "CKKS IND-CPA-D decryption oracle IoT OR smart meter OR actuator OR control system ..."
- **ASSUMPTION:** the actuator observer can link each action to the specific ciphertext and can influence its inputs. This
  is needed for any reaction-style attack and must be stated in the threat model.

## Possible seeds (direction-miner moves)
- **Transplant setting:** reaction or IND-CPA-D attacks studied in cloud/MPC → IoT closed loop (thermostat/actuator).
  Must be differentiated from 2026/2490.
- **Weaken the oracle:** full decryption (LM21) → 1-bit (2026/2490) → quantized multi-level actuator output. Measure
  queries-to-key against the quantization level.
- **Cost the countermeasure on an MCU:** noise flooding (2022/816, 2025/1618) and exact-CKKS decoding (2024/853), costed
  in cycles and memory on a Cortex-M33-class device (EFR32xG24 in phase 2). The question is whether the device can afford
  it at decrypt time.
- **Protocol check:** does the IoT deployment publish C2S/bootstrapping keys at all? If not, does the 2026/2490 attack
  path disappear, and what remains?

## Sources
- https://eprint.iacr.org/2020/1533
- https://par.nsf.gov/servlets/purl/10340712
- https://github.com/ChainReaction-LTD/lattigo/security
- https://openfhe-development.readthedocs.io/en/latest/sphinx_rsts/intro/security.html
- https://eprint.iacr.org/2022/816
- https://eprint.iacr.org/2024/424
- https://eprint.iacr.org/2024/853
- https://eprint.iacr.org/2024/127
- https://eprint.iacr.org/2024/116
- https://eprint.iacr.org/2024/203
- https://eprint.iacr.org/2025/045
- https://eprint.iacr.org/2025/395
- https://eprint.iacr.org/2025/1618
- https://eprint.iacr.org/2025/1627
- https://eprint.iacr.org/2026/285
- https://eprint.iacr.org/2026/2490
- https://eprint.iacr.org/2026/2546
- https://usenix.org/system/files/sec24summer-prepub-822-guo.pdf
