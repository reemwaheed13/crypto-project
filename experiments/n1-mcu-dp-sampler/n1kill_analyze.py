#!/usr/bin/env python3
"""n1kill_analyze.py <build_kill dir>: scores settings 1-3 of the N1 kill test (stdlib only).
Attacks for settings 2-3 (exact search infeasible, see research/n1-kill-*.md):
  LR   : Neyman-Pearson test for the Gaussian part (nearest noiseless mean)
  BITS : naive Bayes on low-order mantissa bits, trained on the first half of the trials, tested on the second half
  LR+BITS: naive Bayes over the LR margin (binned) plus the bit features
Ideal-Gaussian bound: Phi(mu/2), mu = mean shift / noise s.d. of the released value."""
import glob, math, os, re, struct, sys
from collections import Counter
B = sys.argv[1]
SIG = [0.4509, 0.7983]; C = 1.0
Phi = lambda x: 0.5 * (1 + math.erf(x / math.sqrt(2)))
f32 = lambda h: struct.unpack("<f", struct.pack("<I", int(h, 16)))[0]
bits = lambda x: struct.unpack("<I", struct.pack("<f", x))[0]
def r32(x): return struct.unpack("<f", struct.pack("<f", x))[0]

def nb_fit(X, y, nvals):
    m = []
    for j in range(len(X[0])):
        cnt = [Counter(), Counter()]
        for xi, yi in zip(X, y): cnt[yi][xi[j]] += 1
        n = [sum(c.values()) for c in cnt]
        m.append([{v: math.log((cnt[c][v] + 1) / (n[c] + nvals[j])) for v in range(nvals[j])} for c in (0, 1)])
    return m
def nb_acc(m, X, y):
    ok = 0
    for xi, yi in zip(X, y):
        s = [sum(m[j][c][xi[j]] for j in range(len(xi))) for c in (0, 1)]
        ok += (s[1] > s[0]) == yi
    return ok / len(y)
def lr_bin(margin, scale):  # 12 bins of the LR margin
    return max(0, min(11, int(6 + margin / scale * 3)))

print("## Setting 1: single-step release, exact pair-inversion test")
for f in sorted(glob.glob(f"{B}/s1_*.log")):
    m = re.search(r"S1 sigma (\d) trials (\d+) true_match (\d+) false_match (\d+) acc_x1000 (\d+)", open(f).read())
    g, n, mt, mf, acc = map(int, m.groups()); s = SIG[g]
    print(f"sigma={s}: accuracy {acc/10:.1f}% (true-hypothesis match {mt}/{n}, false {mf}/{n}); ideal Gaussian bound {100*Phi(1/s/2):.1f}%")

print("\n## Setting 2: noise-only float32 sum of 5 Box-Muller values")
for f in sorted(glob.glob(f"{B}/s2_*.log")):
    g, e = map(int, re.search(r"s2_g(\d)_e(\d)", f).groups()); s = SIG[g]
    rows = [l.split()[1:] for l in open(f) if l.startswith("S2 ")]
    shift = 5 * C if e else C; sd = math.sqrt(5) * s * C
    X, Y, M = [], [], []
    for h, gk, yk, yk1 in rows:
        h = int(h); gk, yk, yk1 = f32(gk), f32(yk), f32(yk1)
        margin = abs(yk - gk) - abs(yk - (gk + shift))           # > 0 favours "canary present"
        r0, r1 = r32(yk - gk), r32(yk - (gk + shift))
        X.append([bits(yk) & 63, bits(r0) & 63, bits(r1) & 63, bits(yk1) & 63, (bits(yk) >> 23) & 255 == (bits(r0) >> 23) & 255])
        M.append(margin); Y.append(h)
    n = len(Y); half = n // 2
    lr = sum((m > 0) == y for m, y in zip(M[half:], Y[half:])) / (n - half)
    nv = [64, 64, 64, 64, 2]
    bacc = nb_acc(nb_fit(X[:half], Y[:half], nv), X[half:], Y[half:])
    XC = [[lr_bin(m, sd)] + x for m, x in zip(M, X)]
    cacc = nb_acc(nb_fit(XC[:half], Y[:half], [12] + nv), XC[half:], Y[half:])
    print(f"sigma={s}, canary in {'all 5 steps' if e else 'step 1 only'}: LR {100*lr:.1f}% | BITS {100*bacc:.1f}% | LR+BITS {100*cacc:.1f}%"
          f" | ideal Gaussian bound {100*Phi(shift/sd/2):.1f}% (single-step bound was {100*Phi(1/s/2):.1f}%); test n = {n-half}")

print("\n## Setting 3: real 5-step DP-SGD FedAvg upload U = W5 - W0 (D2 FCN, float32)")
for f in sorted(glob.glob(f"{B}/s3_*.log")):
    g, e = map(int, re.search(r"s3_g(\d)_e(\d)", f).groups()); s = SIG[g]; txt = open(f).read()
    meta = re.search(r"S3META .* canary_raw_grad_norm_x1e4 (\d+) dist_x1e9 (\d+)", txt)
    rows = [l.split()[1:] for l in txt.splitlines() if l.startswith("S3 ")]
    if not meta or not rows: print(f"{f}: incomplete"); continue
    craw, dist = int(meta.group(1)) / 1e4, int(meta.group(2)) / 1e9
    Y = [int(r[0]) for r in rows]; G = [int(r[1]) for r in rows]; erms = [int(r[2]) / 1e9 for r in rows]
    noise_sd = sum(erms) / len(erms)
    lr = sum(a == b for a, b in zip(G, Y)) / len(Y)
    has_margin = len(rows[0]) == 36
    X = [[bits(f32(hx)) & 15 for hx in r[(4 if has_margin else 3):]] for r in rows]
    if has_margin:   # empirical separation of the LR statistic: Gaussian-optimal accuracy along it is Phi(d'/2)
        import statistics as St
        m = [int(r[3]) / 1e6 for r in rows]; a1 = [x for x, yy in zip(m, Y) if yy]; a0 = [x for x, yy in zip(m, Y) if not yy]
        dprime = (St.mean(a1) - St.mean(a0)) / math.sqrt(0.5 * (St.pvariance(a1) + St.pvariance(a0)))
        print(f"   empirical separation of the LR statistic d' = {dprime:.3f} -> Gaussian-optimal accuracy on it {100*Phi(dprime/2):.1f}%")
    half = len(Y) // 2
    bacc = nb_acc(nb_fit(X[:half], Y[:half], [16] * 32), X[half:], Y[half:])
    XC = [[gg] + x for gg, x in zip(G, X)]
    cacc = nb_acc(nb_fit(XC[:half], Y[:half], [2] + [16] * 32), XC[half:], Y[half:])
    mu = dist / noise_sd
    print(f"sigma={s}, canary in {'all 5 steps' if e else 'step 1 only'}: LR {100*lr:.1f}% (n={len(Y)}) | BITS {100*bacc:.1f}% | LR+BITS {100*cacc:.1f}%"
          f" | ideal Gaussian bound {100*Phi(mu/2):.1f}% (mu = {mu:.3f}: ||E[U|1]-E[U|0]|| = {dist:.3e}, per-coord noise s.d. {noise_sd:.3e};"
          f" canary raw grad norm {craw:.2f}, clipped to C = 1) (single-step bound was {100*Phi(1/s/2):.1f}%)")
