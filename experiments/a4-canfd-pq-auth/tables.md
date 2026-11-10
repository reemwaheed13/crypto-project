## Table A. Fastest possible authentication (N = 1, any period) vs the 10 ms deadline

| Scheme | Clock | Sign ms (avg / worst seen) | Verify ms | Sig frames | Sig on bus ms (2 Mb/s, 40% bg → 5 Mb/s, 40%) | Min latency ms (best case: 5 Mb/s, 40% bg) | Meets 10 ms? |
|---|---|---|---|---|---|---|---|
| ML-DSA-44 (pqm4 m4f) | 80 MHz | 49.29 / 212.61 | 17.77 | 38 | 25.78 → 12.89 | 79.95 | no |
| ML-DSA-44 (pqm4 m4f) | 240 MHz | 16.43 / 70.87 | 5.92 | 38 | 25.78 → 12.89 | 35.25 | no |
| FN-DSA-512 (pqm4 m4f) | 80 MHz | 280.87 / 282.43 | 4.96 | 11 | 7.46 → 3.73 | 289.57 | no |
| FN-DSA-512 (pqm4 m4f) | 240 MHz | 93.62 / 94.14 | 1.65 | 11 | 7.46 → 3.73 | 99.01 | no |
| FN-DSA-512 low-RAM (2026/1915)* | 80 MHz | 168.12 / n/a | 4.96 | 11 | 7.46 → 3.73 | 176.82 | no |
| FN-DSA-512 low-RAM (2026/1915)* | 240 MHz | 56.04 / n/a | 1.65 | 11 | 7.46 → 3.73 | 61.43 | no |
| SLH-DSA-SHA2-128f (pqm4 clean) | 80 MHz | 4607.19 / 4607.19 | 274.05 | 267 | 181.12 → 90.6 | 4971.84 | no |
| SLH-DSA-SHA2-128f (pqm4 clean) | 240 MHz | 1535.73 / 1535.73 | 91.35 | 267 | 181.12 → 90.6 | 1717.68 | no |

## Table B. Largest window N* meeting the deadline, and its extra bus load (5 Mb/s data phase, 40% background)

| Scheme | Clock | Period ms | N* @ 10 ms | N* @ 50 ms (extra load) | N* @ 100 ms (extra load) |
|---|---|---|---|---|---|
| ML-DSA-44 (pqm4 m4f) | 80 MHz | 1 | none | none | none |
| ML-DSA-44 (pqm4 m4f) | 80 MHz | 10 | none | none | none |
| ML-DSA-44 (pqm4 m4f) | 80 MHz | 100 | none | none | 1 (7.74%) |
| ML-DSA-44 (pqm4 m4f) | 240 MHz | 1 | none | none | 65 (11.9%) |
| ML-DSA-44 (pqm4 m4f) | 240 MHz | 10 | none | 2 (38.68%) | 7 (11.05%) |
| ML-DSA-44 (pqm4 m4f) | 240 MHz | 100 | none | 1 (7.74%) | 1 (7.74%) |
| FN-DSA-512 (pqm4 m4f) | 80 MHz | 1 | none | none | none |
| FN-DSA-512 (pqm4 m4f) | 80 MHz | 10 | none | none | none |
| FN-DSA-512 (pqm4 m4f) | 80 MHz | 100 | none | none | none |
| FN-DSA-512 (pqm4 m4f) | 240 MHz | 1 | none | none | none |
| FN-DSA-512 (pqm4 m4f) | 240 MHz | 10 | none | none | none |
| FN-DSA-512 (pqm4 m4f) | 240 MHz | 100 | none | none | 1 (2.24%) |
| FN-DSA-512 low-RAM (2026/1915)* | 80 MHz | 1 | none | none | none |
| FN-DSA-512 low-RAM (2026/1915)* | 80 MHz | 10 | none | none | none |
| FN-DSA-512 low-RAM (2026/1915)* | 80 MHz | 100 | none | none | none |
| FN-DSA-512 low-RAM (2026/1915)* | 240 MHz | 1 | none | none | none |
| FN-DSA-512 low-RAM (2026/1915)* | 240 MHz | 10 | none | none | none |
| FN-DSA-512 low-RAM (2026/1915)* | 240 MHz | 100 | none | none | 1 (2.24%) |
| SLH-DSA-SHA2-128f (pqm4 clean) | 80 MHz | 1 | none | none | none |
| SLH-DSA-SHA2-128f (pqm4 clean) | 80 MHz | 10 | none | none | none |
| SLH-DSA-SHA2-128f (pqm4 clean) | 80 MHz | 100 | none | none | none |
| SLH-DSA-SHA2-128f (pqm4 clean) | 240 MHz | 1 | none | none | none |
| SLH-DSA-SHA2-128f (pqm4 clean) | 240 MHz | 10 | none | none | none |
| SLH-DSA-SHA2-128f (pqm4 clean) | 240 MHz | 100 | none | none | none |

One 64-B CAN FD frame: 407 µs at 2 Mb/s, 204 µs at 5 Mb/s (worst-case stuffing).
Configurations meeting the 10 ms deadline: 0 of 96.
