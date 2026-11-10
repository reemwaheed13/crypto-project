#!/usr/bin/env python3
"""pld_trunc.py: exact-up-to-discretisation (eps, delta) of DP-SGD when the Gaussian noise is TRUNCATED at R sigma
(float32 Box-Muller from 24-bit uniforms: R = sqrt(48 ln 2) = 5.768), Poisson subsampling q, T steps.

Worst-case neighbour, 1-D: the canary's clipped gradient (norm C) lies on one coordinate, i.e. a shift mu = 1/sigma in
noise units; all other coordinates are identical under both datasets and add no privacy loss.
Per step: P = (1-q) T(0) + q T(mu)   (canary present),  Q = T(0)   (absent),
T(m) = N(m, 1) truncated to [m-R, m+R] and renormalised.
Direction "remove" (observe under P, loss ln P/Q) has an infinite-loss atom: y > R, mass q * T(mu)(y > R).
Direction "add" (observe under Q, loss ln Q/P) has none.
The privacy-loss distribution is discretised on a grid of step h, rounded UP (pessimistic) and DOWN (optimistic) to
bracket the answer, composed T times by FFT, and delta(eps) = 1 - (1-p_atom)^T + sum pmf_T(l) (1 - e^(eps-l))^+.
Sanity check: R = inf must reproduce the opacus PRV accountant (eps ~ 8 and ~ 1 at delta = 1e-5)."""
import math, sys
import numpy as np
from scipy.special import ndtr  # standard normal CDF

def step_pld(sigma, q, R, ny=400_001):
    mu = 1.0 / sigma
    Z = 1.0 - 2.0 * ndtr(-R) if math.isfinite(R) else 1.0
    lo, hi = (-R, mu + R) if math.isfinite(R) else (-12.0, mu + 12.0)
    edges = np.linspace(lo, hi, ny)
    mid = 0.5 * (edges[1:] + edges[:-1])
    def mass(m):   # mass of T(m) in each bin
        a = np.clip(edges[:-1], m - R, m + R) if math.isfinite(R) else edges[:-1]
        b = np.clip(edges[1:], m - R, m + R) if math.isfinite(R) else edges[1:]
        return np.maximum(ndtr(b - m) - ndtr(a - m), 0.0) / Z
    m0, m1 = mass(0.0), mass(mu)
    P, Q = (1 - q) * m0 + q * m1, m0
    out = {}
    # remove: loss ln(P/Q) under P; Q == 0 -> atom
    inf = Q <= 0; out["remove"] = (np.log(P[~inf] / Q[~inf]), P[~inf], P[inf].sum())
    # add: loss ln(Q/P) under Q (P > 0 wherever Q > 0)
    ok = Q > 0; out["add"] = (np.log(Q[ok] / P[ok]), Q[ok], 0.0)
    return out

def compose_delta(L, w, atom, T, eps_list, h, lo=-20.0, hi=60.0, up=True):
    n = int(round((hi - lo) / h)); idx = (np.ceil if up else np.floor)((L - lo) / h).astype(np.int64)
    pmf = np.bincount(np.clip(idx, 0, n - 1), weights=w, minlength=n)
    # shift so the per-step pmf sits near index 0 of a circular buffer, compose by FFT power
    z = int(round(-lo / h)); base = np.roll(pmf, -z)
    f = np.fft.rfft(base); comp = np.fft.irfft(f ** T, n=n); comp = np.maximum(comp, 0.0)
    loss = (np.arange(n) * h); loss[loss > hi] -= n * h          # unwrap: indices past hi are negative losses
    p_any_atom = 1.0 - (1.0 - atom) ** T
    res = {}
    for e in eps_list:
        res[e] = p_any_atom + float(np.sum(comp * np.clip(1.0 - np.exp(e - loss), 0.0, None)))
    return res, float(comp.sum()), p_any_atom

def delta_at(sigma, q, T, R, eps_list, h=2e-4):
    pl = step_pld(sigma, q, R); out = {}
    for d in ("remove", "add"):
        L, w, atom = pl[d]
        up, tot, pa = compose_delta(L, w, atom, T, eps_list, h, up=True)
        dn, _, _ = compose_delta(L, w, atom, T, eps_list, h, up=False)
        out[d] = (dn, up, tot, pa, atom)
    return out

def eps_at(sigma, q, T, R, delta=1e-5, h=2e-4):
    grid = list(np.round(np.arange(0.2, 14.0, 0.05), 3))
    r = delta_at(sigma, q, T, R, grid, h)
    def first(key, which):
        for e in grid:
            if max(r["remove"][which][e], r["add"][which][e]) <= delta: return e
        return float("inf")
    return first("x", 0), first("x", 1)   # (optimistic, pessimistic)

if __name__ == "__main__":
    q, T = 4 / 1209, 750
    R32 = math.sqrt(48 * math.log(2))
    print(f"q = {q:.6f}, T = {T}, float32 Box-Muller cap R = {R32:.4f}")
    for eps_claim, sigma in ((8.0, 0.4509), (1.0, 0.7983)):
        for name, R in (("untruncated", math.inf), ("truncated R=5.768", R32)):
            r = delta_at(sigma, q, T, R, [eps_claim])
            dmax_dn = max(r["remove"][0][eps_claim], r["add"][0][eps_claim])
            dmax_up = max(r["remove"][1][eps_claim], r["add"][1][eps_claim])
            e_dn, e_up = eps_at(sigma, q, T, R)
            print(f"sigma={sigma} (claimed eps={eps_claim}), {name}: delta(eps={eps_claim}) in [{dmax_dn:.3e}, {dmax_up:.3e}]"
                  f"; per-step atom {r['remove'][4]:.3e}, P(any atom in T steps) {r['remove'][3]:.3e}"
                  f"; eps at delta=1e-5 in [{e_dn}, {e_up}]; composed finite mass {r['remove'][2]:.6f}")
        sys.stdout.flush()
