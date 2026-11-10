# Collision check r2 / B3: PCS healing in MLS/TreeKEM under IoT duty cycling

Date: 2026-11-05. Checker: collision subagent.

## Candidate
A fleet of duty-cycled Thread / LoRaWAN class-A nodes shares one MLS (RFC 9420) group. Question: how long does the group stay compromised after one node's state is stolen? Healing time depends on the sleepiest member. We compare update policies (forced periodic update, quarantine, removal) by their energy cost on 802.15.4 and on a Cortex-M33.

## Queries run
1. "MLS TreeKEM post-compromise security IoT duty-cycled nodes healing time update frequency" (extended)
2. "how long does group stay compromised after device key stolen sleeping IoT devices group key update" (plain language; FAILED, rate-limited, no results)
3. "eprint.iacr.org TreeKEM offline members post-compromise security inactive members removal" (ePrint-focused; FAILED, rate-limited, no results)
4. "MLS messaging layer security constrained devices 802.15.4 LoRaWAN evaluation energy commit size" (embedded/networking)
5. "Group OSCORE rekeying sleepy nodes group manager ACE key provisioning post-compromise" (IoT-community group rekeying)
6. "continuous group key agreement IoT constrained devices TreeKEM 2025" (date-restricted)
7. "MLS IoT IETF draft constrained devices \"Messaging Layer Security\" CoAP"
8. "secure group messaging inactive users post-compromise security healing delay simulation \"inactive\""
9. "MLS RFC 9420 Internet of Things sensor network evaluation microcontroller TreeKEM 2026" (extended, date-restricted)
10. "MLS TreeKEM drones swarm OR vehicular OR sensor network group key \"post-compromise\" offline nodes evaluation"
11. "\"MLS\" \"IoT\" group key agreement intermittent connectivity sleepy devices paper 2025 OR 2026 post-compromise security" (date-restricted)
12. "Bluetooth mesh key refresh OR LoRaWAN multicast rekeying low power nodes compromised node exclusion delay energy" (neighbouring embedded/networking community)

Because queries 2 and 3 failed, the plain-language and ePrint-specific angles are only partly covered. Queries 8 and 11 make up for some of that gap.

## Papers / documents opened
| ID | Verified | Depth | Relevance |
|---|---|---|---|
| ePrint 2023/1903, Quarantined-TreeKEM (CCS 2024 extended abstract) | Yes (ePrint abstract page) | Abstract only (the PDF returned 403) | Closest neighbour. It covers inactive users in TreeKEM and how a single inactive corrupted member breaks PCS for the group, and it proposes a quarantine mechanism. The abstract has no healing-time metric, no IoT setting, no duty cycling and no energy cost. Its setting is messaging. |
| arXiv 2502.18303, Experimental Analysis of Efficiency of MLS for Multiple Delivery Services (revised Feb 2026) | Yes (arXiv abstract) | Abstract only | Simulates MLS cost across delivery services (MQTT is mentioned in a snippet). The abstract does not mention inactive members, update policy, PCS healing time or MCUs. |
| arXiv 2601.21966, Secure Group Key Agreement on CPS Buses (GRACYBUS) | Yes (arXiv abstract) | Abstract only | TreeKEM-based GKA for wired broadcast buses, with dynamic membership and unannounced leaves. It does not cover duty cycling, PCS healing time or 802.15.4/LoRa. |
| draft-fondevik-mls-pairedmls (IETF individual draft, expired) | Yes (datatracker page) | Abstract only | Covers PCS for devices that cannot self-update, using a trusted paired device. This is a mechanism-level neighbour, but the abstract gives no quantification. |
| ePrint 2025/229, ETK (Eurocrypt 2026) | Seen in search results only | Snippet | Formal security of RFC 9420 with external operations. It does not address duty cycling. |
| draft-ietf-ace-key-groupcomm-oscore (several versions) | Seen in search results only | Snippet | Group OSCORE key management: the Group Manager rekeys on membership change, and members can retrieve keys on request. No PCS-healing-time analysis turned up in the snippets. Not read in depth. |
| DTIC AD1173274, MLS in the ARSENL UAV swarm | Seen in search results only | Snippet | MLS on a physical UAV swarm. Its setting is not duty-cycled. |
| Cremers–Hale–Kohbrok, ePrint 2019/477 (USENIX Sec 2021) | Seen in search results only | Snippet | PCS healing limits across groups. Theoretical. |
| Lebrun, PhD thesis (ANSSI, Dec 2025) on Quarantined-TreeKEM | Seen in search results only | Snippet | Extends the work on inactive users. Its full content was not checked, so it is a RISK that needs checking. |
| Klein et al., Tainted TreeKEM (S&P 2021) | Seen in search results only | Snippet | Simulations of efficiency under different group-operation distributions. Not IoT. |

Not found: an IETF MLS-for-IoT draft, a paper that measures MLS commit or update cost over 802.15.4/LoRaWAN, or a paper that models PCS healing time as a function of duty-cycle period.

## Verdict: Partial overlap
- KNOWN: Quarantined-TreeKEM (ePrint 2023/1903) and RFC 9420 already identify the problem: inactive members undermine PCS for the whole group. Both offer policies (quarantine or removal). Lebrun's 2025 thesis extends this work. Paired-MLS also addresses devices that cannot update themselves.
- Open as far as searched:
  - (a) a quantitative healing-time model that is driven by IoT duty-cycle distributions (Thread SED/SSED, LoRaWAN class A);
  - (b) the per-node energy, airtime and duty-cycle-regulation cost of each policy, counting commit/update path bytes over 802.15.4/LoRa plus HPKE on Cortex-M33;
  - (c) whether the policy ranking differs from the one for messaging apps.
- The novelty therefore lies in the IoT-specific cost/healing trade-off, not in the observation that inactive members hurt PCS.
- RISKS:
  - The Lebrun thesis and the Quarantined-TreeKEM full PDF were not read; they may include simulations of inactivity.
  - The Group OSCORE / ACE rekeying literature was only skimmed; IoT group-rekeying with a Group Manager may already cover the "bounded exposure window" question for sleepy nodes. It is not MLS/PCS, though.
  - Two queries failed because of rate limits.
