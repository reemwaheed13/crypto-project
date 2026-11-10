#!/usr/bin/env python3
"""analyze.py <build dir>: per-sample instruction counts vs noise values for the N1 samplers (stdlib only)."""
import math, re, statistics as S, sys
B = sys.argv[1]
ins = [int(l.split()[3]) for l in open(f"{B}/run.insn")]
log = open(f"{B}/run.log").read()
va = [tuple(int(x) / 1e6 for x in m) for m in re.findall(r"^VA (-?\d+) (-?\d+)", log, re.M)]
vb = [tuple(int(x) / 1e6 for x in m) for m in re.findall(r"^VB (-?\d+) (-?\d+)", log, re.M)]
iK, ins = ins[:8], ins[8:]
print(f"ChaCha20 block (64 B = 16 words): {S.mean(iK):.0f} instr -> {S.mean(iK)/16:.1f} instr per 32-bit word")
for m in re.findall(r"^(X[AB]) rmax_x1e6 (-?\d+)", log, re.M): print(f"{m[0]} forced smallest-u1 output (max |noise| in sigma units): {int(m[1])/1e6:.3f}")
iA, iB, iC = ins[:2000], ins[2000:4000], [ins[4000:5000], ins[5000:6000], ins[6000:7000]]
def corr(x, y):
    mx, my = S.mean(x), S.mean(y); sx, sy = S.pstdev(x), S.pstdev(y)
    return sum((a - mx) * (b - my) for a, b in zip(x, y)) / (len(x) * sx * sy) if sx and sy else 0.0
def report(name, ii, vv):
    r = [math.hypot(a, b) for a, b in vv]; ang = [abs(math.atan2(b, a)) for a, b in vv]
    print(f"{name}: instr/pair mean {S.mean(ii):.1f} sd {S.pstdev(ii):.1f} min {min(ii)} max {max(ii)} distinct {len(set(ii))}")
    print(f"   corr(instr, r=|pair|) = {corr(ii, r):+.3f}; corr(instr, |angle|) = {corr(ii, ang):+.3f}; max |noise| seen {max(max(abs(a), abs(b)) for a, b in vv):.3f}")
    # bucket by r: does the instruction count separate small vs large noise?
    q = sorted(range(len(r)), key=lambda i: r[i]); lo, hi = q[: len(q) // 10], q[-len(q) // 10:]
    print(f"   mean instr, smallest 10% of r: {S.mean(ii[i] for i in lo):.1f}; largest 10% of r: {S.mean(ii[i] for i in hi):.1f}")
    paths = {}
    for i, n in enumerate(ii): paths.setdefault(n, []).append(r[i])
    top = sorted(paths.items(), key=lambda kv: -len(kv[1]))[:6]
    print("   most common instruction counts (count: n_samples, r range):", "; ".join(f"{k}: {len(v)}, {min(v):.2f}-{max(v):.2f}" for k, v in top))
report("A float32 Box-Muller (24-bit u1)", iA, va)
report("B float32 Box-Muller (full-exponent u1)", iB, vb)
for s, ii in zip((2, 8, 32), iC):
    print(f"C CT-CDT sigma0={s}: instr/sample mean {S.mean(ii):.1f} sd {S.pstdev(ii):.2f} distinct {len(set(ii))}")

# how much does the instruction count (timing proxy) reveal about the SIGNS of the two noise values?
def sign_leak(name, ii, vv):
    from collections import Counter, defaultdict
    g = defaultdict(list)
    for n, (a, b) in zip(ii, vv): g[n].append((a > 0) * 2 + (b > 0))
    tot = len(ii); H = lambda c: -sum(v / sum(c.values()) * math.log2(v / sum(c.values())) for v in c.values())
    h_prior = H(Counter(x for v in g.values() for x in v)); h_post = sum(len(v) / tot * H(Counter(v)) for v in g.values())
    acc = sum(max(Counter(v).values()) for v in g.values()) / tot
    print(f"{name}: sign-pair entropy {h_prior:.3f} bits -> {h_post:.3f} bits given instr count "
          f"(leak {h_prior - h_post:.3f} bits/pair); best guess of both signs from instr count: {100*acc:.1f}% (chance 25%)")
sign_leak("A", iA, va); sign_leak("B", iB, vb)
