# Quarantined-TreeKEM (ePrint 2023/1903, ACM CCS 2024)

Lane: B

## Citation
- Title: "Quarantined-TreeKEM: a Continuous Group Key Agreement for MLS, Secure in Presence of Inactive Users"
- Authors: Céline Chevalier (ENS-PSL, Univ. Paris Panthéon-Assas), Guirec Lebrun (ENS-PSL, ANSSI), Ange Martinelli (ANSSI), Abdul Rahman Taleb (ANSSI)
- Status (verified): **published**. The extended abstract appeared at ACM CCS 2024 (DOI 10.1145/3658644.3690265, ACM DL dated 9 Dec 2024). ePrint 2023/1903 is the full version: received 2023-12-11, last revised 2025-04-09, 5 revisions. A HAL copy exists as hal-05026639 (deposited 9 Apr 2025).
- Code: Kotlin proof of concept, a fork of mls-kotlin: https://github.com/AbdulRahmanTaleb/Quarantined-TreeKEM (MIT licence)

## Reading depth
**Abstract + snippets + code README.** The full text could NOT be opened. ePrint PDF, ACM DL and ACM PDF returned HTTP 403. The HAL PDF hit an Anubis bot wall. The Semantic Scholar API returned 429. Everything below comes from the ePrint abstract page, search-engine snippets of the PDF, and the GitHub README. **No cost numbers from the paper's tables were seen.** A human should read sections on the inactivity model and the evaluation before anyone relies on this note.

## Core claim
If MLS's TreeKEM puts members who stop updating into "quarantine", it can heal the group despite inactive users. Committers refresh those members' keys on their behalf and Shamir-share the new secrets among the other members. The authors say this costs very little extra communication, and sometimes less than plain TreeKEM.

## Mechanism and inactivity model (from abstract, snippets, README)
- Inactivity is counted in **epochs, not wall-clock time**. A user becomes a "ghost" once its key age exceeds an inactivity delay. Source: snippet, "Users whose key age exceeds the inactivity delay are declared ghost users", plus the README parameter `INACTIVITY_DELAY` = number of epochs without a key update.
- During quarantine a committer generates fresh keys for the ghost's leaf. The ghost's key material is secret-shared with a (t, m) perfect secret sharing scheme; the experiments use Shamir. Shares go to other members. The README also mentions a fallback "horizontal" share distribution when there are fewer than `MINIMUM_SECRET_SHARING_NB` shares.
- More parameters (README): `UPDATE_QUARANTINE_KEYS_DELAY` sets how often ghost keys are renewed, and `DELETE_FROM_QUARANTINE_DELAY` sets how many epochs pass before a ghost is removed. **The README gives no default values.** Whether the paper fixes or sweeps them is NOT VERIFIED.
- Leaving quarantine: when a ghost reconnects, members return shares, and the ghost rebuilds its keys and can decrypt the messages it missed. Alternatively it rejoins through a `WelcomeBackGhostMessage` without access to past messages (README).
- A snippet shows the paper has a figure ("Figure 5") comparing "critical windows" (vulnerability windows) per user type, TreeKEM vs Quarantined-TreeKEM. Its contents were not seen.
- Whether activity is modelled as a probability distribution (e.g., per-user update probability) or as a fixed trace is NOT VERIFIED.

## Platform
- The PoC is Kotlin on the JVM (Kotlin 1.9.22 / Java 21, inherited from mls-kotlin). The README reports **no benchmarks**.
- Whatever cost evaluation the paper has: the abstract speaks of "communication overhead", and the snippets mention "experiments" using Shamir. Whether it is simulation, analysis or measurement, and on what host, is NOT VERIFIED.
- **No constrained-device, MCU, radio or energy measurement was found** anywhere in the material seen. Read this as a PC/JVM-level, communication-focused evaluation at best. Treat it as "no IoT measurement as far as seen".

## Threat model (partly inferred; mark as ASSUMPTION until full text is read)
- KNOWN (abstract): the asset is group key secrecy, i.e. forward security and post-compromise security of the CGKA epoch secrets. The problem is that inactive users' stale leaf keys keep the group exposed. The abstract frames it as inactive users who stop updating "weaken security for the whole group".
- ASSUMPTION: a standard CGKA adversary that adaptively corrupts member states, against an untrusted delivery service, in line with MLS. Whether the model covers an active or malicious DS, and whether t colluding share-holders are excluded, was not verified. Collusion of share-holders is an obvious new surface the quarantine creates.
- Out of scope (as far as seen): energy, radio cost, device constraints, side channels.

## Numbers that matter
- From the abstract only: "very limited – and sometimes negative – communication overhead" relative to TreeKEM. **No concrete numbers were seen. Do not cite any.**
- The cost tables and figures are in the full text, not read.

## Convenient assumptions (as far as visible)
- Inactivity is measured in epochs. This hides wall-clock healing time, which for duty-cycled nodes depends on commit frequency.
- Other members are assumed online enough to hold and return shares.
- No radio, energy, RAM or flash cost. The target is messaging (phones/PCs), not sensors.

## Stated limitations / future work
Not recoverable without the full text. NOT VERIFIED. The related seminar abstract (Lebrun, LIP6 ALMASTY, 2025) says TreeKEM's logarithmic cost is a lower bound rarely reached in practice. Lebrun's PhD thesis (defended 1 Dec 2025, ANSSI page "A Tale of Trees or Security and Efficiency of Secure Group Messaging Protocols") covers Q-TreeKEM. These come from search snippets only.

## Load-bearing references (expected; specific citation list not seen)
- RFC 9420, The Messaging Layer Security (MLS) Protocol
- TreeKEM (Bhargavan, Barnes, Rescorla, 2018 proposal)
- Alwen, Coretti, Dodis, Tselekounis, CGKA security of TreeKEM (CRYPTO 2020) and related CGKA analyses
- Alwen et al., "Keep the Dirt: Tainted TreeKEM" (an alternative way to handle refreshes on behalf of others)
- ePrint 2023/1761, "Messaging Layer Security under Restricted Key Updates" (closely related; surfaced alongside in search)

## Related items (verified, one line each)
- **arXiv 2502.18303.** Soler, Dafonte, Fernández-Veiga, Fernández Vilas, Nóvoa. v1 title "Experimental Analysis of Efficiency of the Messaging Layer Security for Multiple Delivery Services"; v2 (27 Feb 2026) title "Analysis of Efficiency of the Messaging Layer Security protocol in Experimental Settings". Submitted to IEEE Open J. Computer Society (preprint, arXiv-only). OpenMLS-based Rust clients on a 16-core/128 GB VM, groups up to 10,000. Reports computation scaling linearly in practice and about 0.1 s to process a commit at 10k members. **No IoT/MCU measurement**; Q-TreeKEM appears only in related work.
- **arXiv 2601.21966.** Peters, Lautenschlager, Emeis, Lochert, "Secure Group Key Agreement on Cyber-Physical System Buses" (29 Jan 2026, arXiv-only, no venue listed). TreeKEM-based GKA for broadcast/half-duplex buses (CAN-FD/PROFIBUS/industrial Ethernet class). Kotlin/Java PoC with Kyber, hybrid ECDSA-P521+Dilithium, AES-256. **Analytical evaluation only, no MCU named, no timings.** Inactive nodes are removed after an update window (requirement R05). Q-TreeKEM is not cited.
- **ePrint 2025/229.** Confirmed as the Cremers, Günsay, Wesselkamp, Zhao paper: "ETK: External-Operations TreeKEM and the Security of MLS in RFC 9420", EUROCRYPT 2026 per ePrint (v2, 11 Apr 2026). It shows that external commits/proposals weaken MLS PCS, and fixes this with ETK^PSK using the PSK mechanism.

## Possible seeds (direction-miner moves)
- **Transplant to a constrained setting:** Q-TreeKEM's quarantine, applied to duty-cycled Thread/LoRaWAN fleets. Measure the extra per-commit bytes and the Shamir share-handling energy on a Cortex-M33 over 802.15.4. No such measurement was found (as far as searched).
- **Change the unit of measure:** convert epoch-based inactivity into wall-clock PCS healing time as a function of duty cycle and commit rate. The paper (as far as seen) counts in epochs.
- **Stress an assumption:** share-holders are assumed to be reachable. In sleepy fleets, the share-holders are themselves often asleep, so reconnection latency and the choice of t trade off against collusion risk.
- **Combine with a neighbour:** ETK (2025/229) shows external commits weaken PCS. IoT provisioning often uses external joins, so the question is how quarantine and external operations interact on fleets.
- **Fill an evaluation gap:** both 2502.18303 and 2601.21966 stop at VM or analytical cost. A calibrated MCU cost model (cycles, RAM, bytes on air) for MLS commit/update with and without quarantine fills it.
- **Caveat before committing:** the full text MUST be read first. Q-TreeKEM may already sweep inactivity rates and give communication numbers that pre-empt part of the "quantify healing vs duty cycle" question.

## Search log (for "open as far as searched")
- "Quarantined-TreeKEM inactive users MLS CCS 2024 Chevalier Lebrun Martinelli Taleb"
- "\"Quarantined-TreeKEM\" pdf"
- "Quarantined-TreeKEM ghost users Shamir secret sharing inactivity delay simulation communication cost results"
- "\"Quarantined-TreeKEM\" inactive users \"communication cost\" OR \"overhead\" OR \"simulation\" Lebrun Taleb"
