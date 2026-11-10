/* N1 week-1 kill test: does the float32 Box-Muller pair-inversion attack survive D2's 5-step FedAvg upload?
 * Runs on the M33 code path (QEMU mps2-an505), same newlib float32 libm and the same noise construction as dpstep.c.
 * Compile-time switches:  -DSETTING={1,2,3}  -DSIG_IDX={0,1} (sigma 0.4509 / 0.7983)  -DEVERY={0,1}
 *   (canary in step 1 only, or in all 5 steps; ignored for setting 1).
 * Setting 1: single-step release of one pair (as attack.c); exact pair-inversion test.
 * Setting 2: noise-only sum. y = (((G + hC) + n1) + n2) ... + n5 in float32, n_s = fl(std*a_s), for coordinate k
 *            (pair partner k+1 gets the b_s without canary). EVERY=1 adds hC before every noise term.
 * Setting 3: real 5-step DP-SGD on D2's FCN (16,608 params, B = 4, lr 0.5, C = 1), model changing between steps,
 *            upload U = W5 - W0 in float32. Canary = an extra 4th example whose clipped gradient has norm C ("remove"
 *            adjacency: absent -> 3 known examples in that step; sum divided by B = 4 either way).
 * The adversary knows everything except the canary bit and the noise (worst-case DP adversary).
 * Output lines are parsed by n1kill_analyze.py. */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bench.h"

#ifndef SETTING
#define SETTING 1
#endif
#ifndef SIG_IDX
#define SIG_IDX 0
#endif
#ifndef EVERY
#define EVERY 0
#endif
static const float SIGS[2] = {0.4509f, 0.7983f};
#define C_CLIP 1.0f
#define TWO_PI 6.28318530718f

/* ---- DP noise RNG: ChaCha20 (as dpstep.c) ---- */
#define ROTL(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define QR(a, b, c, d) (a += b, d ^= a, d = ROTL(d, 16), c += d, b ^= c, b = ROTL(b, 12), \
                        a += b, d ^= a, d = ROTL(d, 8),  c += d, b ^= c, b = ROTL(b, 7))
static uint32_t st[16], buf[16]; static int pos = 16;
static void chacha_block(uint32_t out[16], const uint32_t in[16]) {
    uint32_t x[16]; memcpy(x, in, 64);
    for (int i = 0; i < 10; i++) {
        QR(x[0], x[4], x[8], x[12]); QR(x[1], x[5], x[9], x[13]); QR(x[2], x[6], x[10], x[14]); QR(x[3], x[7], x[11], x[15]);
        QR(x[0], x[5], x[10], x[15]); QR(x[1], x[6], x[11], x[12]); QR(x[2], x[7], x[8], x[13]); QR(x[3], x[4], x[9], x[14]);
    }
    for (int i = 0; i < 16; i++) out[i] = x[i] + in[i];
}
static void rng_init(uint32_t seed) {
    static const uint32_t c[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
    memcpy(st, c, 16); for (int i = 4; i < 12; i++) st[i] = 0x9e3779b9u * i + seed; st[12] = 0; st[13] = seed; st[14] = st[15] = 1; pos = 16;
}
static inline uint32_t rng32(void) { if (pos == 16) { chacha_block(buf, st); st[12]++; pos = 0; } return buf[pos++]; }
static inline void bm_from_u(float u1, float u2, float *a, float *b) {
    float r = sqrtf(-2.0f * logf(u1)), t = TWO_PI * u2; *a = r * cosf(t); *b = r * sinf(t);
}
static inline void gauss2(float *a, float *b) {          /* identical to dpstep.c */
    float u1 = ((rng32() >> 8) + 1) * (1.0f / 16777216.0f), u2 = (rng32() >> 8) * (1.0f / 16777216.0f);
    bm_from_u(u1, u2, a, b);
}
/* trial-setup generator (not DP noise) */
static uint64_t s64 = 0x9e3779b97f4a7c15ull;
static uint32_t r32(void) { s64 ^= s64 >> 12; s64 ^= s64 << 25; s64 ^= s64 >> 27; return (uint32_t)((s64 * 0x2545F4914F6CDD1Dull) >> 32); }
static float unif_pm1(void) { return (int32_t)(r32() >> 8) * (1.0f / 8388608.0f) - 1.0f; }
static uint32_t fbits(float f) { uint32_t b; memcpy(&b, &f, 4); return b; }

#if SETTING == 1
/* exact pair-inversion consistency test (attack.c, adaptive windows) */
static int u1_valid(float u) { if (!(u > 0.0f && u <= 1.0f)) return 0; float s = u * 16777216.0f; return s == (float)(uint32_t)s && s >= 1.0f; }
static float fstep(float u, int d) { uint32_t b; memcpy(&b, &u, 4); b += (uint32_t)d; memcpy(&u, &b, 4); return u; }
static int consistent(float yk, float yk1, float Gk, float Gk1, float std) {
    float a = (yk - Gk) / std, b = (yk1 - Gk1) / std;
    float u1e = expf(-0.5f * (a * a + b * b)); if (u1e > 1.0f) u1e = 1.0f;
    float th = atan2f(b, a); if (th < 0) th += TWO_PI;
    int32_t i2e = (int32_t)lrintf(th / TWO_PI * 16777216.0f);
    float r = sqrtf(a * a + b * b); int W1 = 4 + (int)(8.0f * r), W2 = 4 + (int)(4.0f / (r > 1e-4f ? r : 1e-4f)); if (W2 > 16384) W2 = 16384;
    for (int d1 = -W1; d1 <= W1; d1++) {
        float u1 = fstep(u1e, d1); if (!u1_valid(u1)) continue;
        for (int d2 = -W2; d2 <= W2; d2++) {
            float u2 = ((i2e + d2) & 0xFFFFFF) * (1.0f / 16777216.0f), pa, pb; bm_from_u(u1, u2, &pa, &pb);
            if (Gk + std * pa == yk && Gk1 + std * pb == yk1) return 1;
        }
    }
    return 0;
}
int main(void) {
    const float std = SIGS[SIG_IDX] * C_CLIP; rng_init(1); int N = 2000, c2 = 0, mt_n = 0, mf_n = 0;
    for (int t = 0; t < N; t++) {
        int h = t & 1; float gk = unif_pm1(), gk1 = unif_pm1(), a, b; gauss2(&a, &b);
        float Gt = h ? gk + C_CLIP : gk, Gf = h ? gk : gk + C_CLIP;
        float yk = Gt + std * a, yk1 = gk1 + std * b;
        int mt = consistent(yk, yk1, Gt, gk1, std), mf = consistent(yk, yk1, Gf, gk1, std);
        mt_n += mt; mf_n += mf; c2 += (mt && !mf) ? 2 : (mt == mf) ? 1 : 0;
    }
    printf("S1 sigma %d trials %d true_match %d false_match %d acc_x1000 %d\n", SIG_IDX, N, mt_n, mf_n, c2 * 500 / N);
    printf("DONE fails 0\n"); return 0;
}
#endif

#if SETTING == 2
int main(void) {
    const float std = SIGS[SIG_IDX] * C_CLIP; rng_init(2); const int N = 20000;   /* 10k train / 10k test for the bit test */
    for (int t = 0; t < N; t++) {
        int h = r32() & 1; float gk = unif_pm1(), gk1 = unif_pm1();
        float yk = gk + (EVERY ? 0.0f : (h ? C_CLIP : 0.0f)), yk1 = gk1;
        for (int s = 0; s < 5; s++) {
            float a, b; gauss2(&a, &b);
            if (EVERY && h) yk = yk + C_CLIP;
            yk = yk + std * a; yk1 = yk1 + std * b;
        }
        /* attacker-known G under each hypothesis, and the observed pair */
        printf("S2 %d %08lx %08lx %08lx\n", h, (unsigned long)fbits(gk), (unsigned long)fbits(yk), (unsigned long)fbits(yk1));
    }
    printf("DONE fails 0\n"); return 0;
}
#endif

#if SETTING == 3
#define H 96
#define F 96
#define HID 64
#define B 4
#define STEPS 5
#define NP (HID * H + HID + HID * HID + HID + F * HID + F)
static float w[NP], w0[NP], g[NP], ge[NP], m0[NP], m1[NP];
static float xs[STEPS][B][H], ys[STEPS][B][F], xc[H], yc[F];
static inline float sig(float z) { return 1.0f / (1.0f + expf(-z)); }
static void fwd_bwd(const float *x, const float *y, float *gout) {   /* identical to dpstep.c */
    const float *W1 = w, *b1 = W1 + HID * H, *W2 = b1 + HID, *b2 = W2 + HID * HID, *W3 = b2 + HID, *b3 = W3 + F * HID;
    float *gW1 = gout, *gb1 = gW1 + HID * H, *gW2 = gb1 + HID, *gb2 = gW2 + HID * HID, *gW3 = gb2 + HID, *gb3 = gW3 + F * HID;
    float h1[HID], h2[HID], d3[F], d2[HID], d1[HID];
    for (int i = 0; i < HID; i++) { float s = b1[i]; for (int j = 0; j < H; j++) s += W1[i * H + j] * x[j]; h1[i] = sig(s); }
    for (int i = 0; i < HID; i++) { float s = b2[i]; for (int j = 0; j < HID; j++) s += W2[i * HID + j] * h1[j]; h2[i] = sig(s); }
    for (int i = 0; i < F; i++) { float s = b3[i]; for (int j = 0; j < HID; j++) s += W3[i * HID + j] * h2[j]; d3[i] = 2.0f * (s - y[i]) / F; }
    for (int j = 0; j < HID; j++) { float s = 0; for (int i = 0; i < F; i++) s += W3[i * HID + j] * d3[i]; d2[j] = s * h2[j] * (1 - h2[j]); }
    for (int i = 0; i < F; i++) { gb3[i] += d3[i]; for (int j = 0; j < HID; j++) gW3[i * HID + j] += d3[i] * h2[j]; }
    for (int j = 0; j < HID; j++) { float s = 0; for (int i = 0; i < HID; i++) s += W2[i * HID + j] * d2[i]; d1[j] = s * h1[j] * (1 - h1[j]); }
    for (int i = 0; i < HID; i++) { gb2[i] += d2[i]; for (int j = 0; j < HID; j++) gW2[i * HID + j] += d2[i] * h1[j]; }
    for (int i = 0; i < HID; i++) { gb1[i] += d1[i]; for (int j = 0; j < H; j++) gW1[i * H + j] += d1[i] * x[j]; }
}
static float clip_acc(float C) {
    float n2 = 0; for (int k = 0; k < NP; k++) n2 += ge[k] * ge[k];
    float s = sqrtf(n2) > C ? C / sqrtf(n2) : 1.0f;
    for (int k = 0; k < NP; k++) g[k] += s * ge[k];
    return sqrtf(n2);
}
static void add_noise(float std) {
    for (int k = 0; k < NP; k += 2) { float a, b; gauss2(&a, &b); g[k] += std * a; if (k + 1 < NP) g[k + 1] += std * b; }
}
/* 5 DP-SGD steps from w0; canary present h; returns U = W5 - W0 in out */
static float canary_norm;
static void run5(int h, float sigma, float *out) {
    const float lr = 0.5f; memcpy(w, w0, sizeof w);
    for (int s = 0; s < STEPS; s++) {
        memset(g, 0, sizeof g);
        for (int b = 0; b < B - 1; b++) { memset(ge, 0, sizeof ge); fwd_bwd(xs[s][b], ys[s][b], ge); clip_acc(C_CLIP); }
        if (h && (EVERY || s == 0)) { memset(ge, 0, sizeof ge); fwd_bwd(xc, yc, ge); canary_norm = clip_acc(C_CLIP); }
        if (sigma > 0) add_noise(sigma * C_CLIP);
        for (int k = 0; k < NP; k++) w[k] -= lr * g[k] / B;
    }
    for (int k = 0; k < NP; k++) out[k] = w[k] - w0[k];
}
int main(void) {
    const float sigma = SIGS[SIG_IDX];
    rng_init(3);
    for (int k = 0; k < NP; k++) w0[k] = ((int)(r32() % 2001) - 1000) * 1e-4f;
    for (int s = 0; s < STEPS; s++) for (int b = 0; b < B; b++) for (int j = 0; j < H; j++) {
        float ph = 0.3f * (s * B + b); xs[s][b][j] = 0.5f + 0.35f * sinf(0.065f * j + ph); ys[s][b][j] = 0.5f + 0.35f * sinf(0.065f * (j + 96) + ph); }
    for (int j = 0; j < H; j++) { xc[j] = 0.5f + 0.4f * cosf(0.2f * j); yc[j] = (j & 8) ? 1.0f : 0.0f; }   /* outlier canary */
    /* attacker's noiseless predictions under both hypotheses (same code path, sigma = 0) */
    run5(0, 0.0f, m0); run5(1, 0.0f, m1);
    double dist2 = 0; for (int k = 0; k < NP; k++) { double d = (double)m1[k] - m0[k]; dist2 += d * d; }
    /* 32 coordinates where the hypotheses differ most: exported for the low-order-bit test */
    static int top[32]; float best[32]; for (int i = 0; i < 32; i++) { best[i] = -1; top[i] = 0; }
    for (int k = 0; k < NP; k++) { float d = fabsf(m1[k] - m0[k]); int j = 31; if (d <= best[31]) continue;
        while (j > 0 && best[j - 1] < d) { best[j] = best[j - 1]; top[j] = top[j - 1]; j--; } best[j] = d; top[j] = k; }
    printf("S3META sigma %d every %d params %d canary_raw_grad_norm_x1e4 %d dist_x1e9 %ld\n", SIG_IDX, EVERY, NP,
           (int)(canary_norm * 1e4f), (long)(sqrt(dist2) * 1e9));
    static float U[NP]; const int N = 2000;
    for (int t = 0; t < N; t++) {
        int h = t & 1; run5(h, sigma, U);
        double d0 = 0, d1 = 0, e2 = 0;
        for (int k = 0; k < NP; k++) { double a = (double)U[k] - m0[k], b = (double)U[k] - m1[k]; d0 += a * a; d1 += b * b; e2 += h ? b * b : a * a; }
        int guess = d1 < d0;   /* equal-variance Gaussian likelihood ratio = nearest noiseless mean */
        printf("S3 %d %d %ld %ld", h, guess, (long)(sqrt(e2 / NP) * 1e9), (long)((d0 - d1) * 1e6));   /* margin: + favours h=1 */
        for (int i = 0; i < 32; i++) printf(" %08lx", (unsigned long)fbits(U[top[i]]));
        printf("\n");
    }
    printf("DONE fails 0\n"); return 0;
}
#endif
