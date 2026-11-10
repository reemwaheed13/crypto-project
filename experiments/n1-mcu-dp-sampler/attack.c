/* N1: Mironov / Holohan-Braghin style pair-inversion attack on float32 Box-Muller DP noise, run ON the M33 build so the
 * attacker's recomputation uses the identical newlib float32 logf/cosf/sinf.
 * Release model (FedSGD-style, Jin et al. threat model, attacker-favourable): two adjacent coordinates k, k+1 receive
 * one Box-Muller pair (a, b), exactly as add_noise() in experiments/d2-sensor-gradinv/mcu/dpstep.c:
 *     y_k = fl(G_k + fl(std*a)),  y_{k+1} = fl(G_{k+1} + fl(std*b)),  std = sigma*C.
 * G is the clipped-gradient sum. The canary adds +C to coordinate k only (worst case). The attacker knows G under each
 * hypothesis (worst-case DP adversary) and must decide whether the canary was present. No lr/B scaling, no
 * multi-step FedAvg (both untested).
 * Attack: for each hypothesis, invert (a, b) -> (u1, u2) estimates, search +-W float ulps of u1 and +-W grid steps of u2,
 * recompute the pair with the sampler's own code, and test exact float equality on both coordinates. */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bench.h"

static uint64_t s64 = 0x9e3779b97f4a7c15ull;   /* trial generator (not the DP RNG): xorshift64* */
static uint32_t r32(void) { s64 ^= s64 >> 12; s64 ^= s64 << 25; s64 ^= s64 >> 27; return (uint32_t)((s64 * 0x2545F4914F6CDD1Dull) >> 32); }

#define TWO_PI 6.28318530718f
static inline void bm_from_u(float u1, float u2, float *a, float *b) {
    float r = sqrtf(-2.0f * logf(u1)), t = TWO_PI * u2; *a = r * cosf(t); *b = r * sinf(t);
}
/* the two samplers' u1 constructions (identical to sampler.c / dpstep.c) */
static float u1_base(uint32_t w) { return ((w >> 8) + 1) * (1.0f / 16777216.0f); }
static float u1_fullexp(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3) {
    uint32_t z = (w0 ? __builtin_clz(w0) : 32 + (w1 ? __builtin_clz(w1) : 32 + __builtin_clz(w2 | 1)));
    uint32_t bits = ((126u - z) << 23) | (w3 >> 9); float u; memcpy(&u, &bits, 4); return u;
}
static int u1_valid(float u, int fullexp) {      /* is u a value the sampler can emit? */
    if (!(u > 0.0f && u <= 1.0f)) return 0;
    if (fullexp) { uint32_t b; memcpy(&b, &u, 4); uint32_t e = b >> 23; return e >= 126u - 95u && e <= 126u; }
    float s = u * 16777216.0f; return s == (float)(uint32_t)s && s >= 1.0f;   /* (i+1)/2^24 exactly */
}
static float fstep(float u, int d) { uint32_t b; memcpy(&b, &u, 4); b += (uint32_t)d; memcpy(&u, &b, 4); return u; }

/* search widths adapt to how precisely y pins down (u1, u2): u1's ulp error grows ~ r, the angle error ~ 1/r */
static int wu1(float r) { return 4 + (int)(8.0f * r); }
static int wu2(float r) { int w = 4 + (int)(4.0f / (r > 1e-4f ? r : 1e-4f)); return w > 16384 ? 16384 : w; }
/* does some (u1, u2) the sampler can emit reproduce (yk, yk1) under hypothesis Gk? */
static int consistent(float yk, float yk1, float Gk, float Gk1, float std, int fullexp) {
    float a = (yk - Gk) / std, b = (yk1 - Gk1) / std;
    float u1e = expf(-0.5f * (a * a + b * b)); if (u1e > 1.0f) u1e = 1.0f;
    float th = atan2f(b, a); if (th < 0) th += TWO_PI;
    int32_t i2e = (int32_t)lrintf(th / TWO_PI * 16777216.0f);
    float r = sqrtf(a * a + b * b); int W1 = wu1(r), W2 = wu2(r);
    for (int d1 = -W1; d1 <= W1; d1++) {
        float u1 = fstep(u1e, d1); if (!u1_valid(u1, fullexp)) continue;
        for (int d2 = -W2; d2 <= W2; d2++) {
            int32_t i2 = (i2e + d2) & 0xFFFFFF; float u2 = i2 * (1.0f / 16777216.0f), pa, pb;
            bm_from_u(u1, u2, &pa, &pb);
            if (Gk + std * pa == yk && Gk1 + std * pb == yk1) return 1;
        }
    }
    return 0;
}

int main(void) {
    const float C = 1.0f; static const float SIG[2] = {0.4509f, 0.7983f};   /* D2 PRV sigma at eps = 8 and eps = 1 */
    for (int si = 0; si < 2; si++)
    for (int fullexp = 0; fullexp <= 1; fullexp++) {
        const float std = SIG[si] * C;
        int N = 2000, correct2 = 0, true_match = 0, false_match = 0;   /* correct2 counts half-points */
        bench_start();
        for (int t = 0; t < N; t++) {
            int canary = t & 1;
            float gk = (int32_t)(r32() >> 8) * (1.0f / 8388608.0f) - 1.0f, gk1 = (int32_t)(r32() >> 8) * (1.0f / 8388608.0f) - 1.0f;
            float G0 = gk, G1 = gk + C;                                  /* coordinate-k sum without / with canary */
            float u1 = fullexp ? u1_fullexp(r32(), r32(), r32(), r32()) : u1_base(r32());
            float u2 = (r32() >> 8) * (1.0f / 16777216.0f), a, b;
            bm_from_u(u1, u2, &a, &b);
            float Gt = canary ? G1 : G0, Gf = canary ? G0 : G1;
            float yk = Gt + std * a, yk1 = gk1 + std * b;               /* what the server sees */
            int mt = consistent(yk, yk1, Gt, gk1, std, fullexp), mf = consistent(yk, yk1, Gf, gk1, std, fullexp);
            true_match += mt; false_match += mf;
            correct2 += (mt && !mf) ? 2 : (mt == mf) ? 1 : 0;
        }
        bench_stop();
        printf("RES sigma_x1e4 %d sampler %s trials %d true_hyp_match %d false_hyp_match %d accuracy_x1000 %d\n",
               (int)(SIG[si] * 1e4f), fullexp ? "fullexp" : "base24", N, true_match, false_match, correct2 * 500 / N);
    }
    printf("DONE fails 0\n");
    return 0;
}
