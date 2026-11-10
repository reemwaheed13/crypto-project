/* N1 preliminary: DP noise samplers on Cortex-M33 (QEMU mps2-an505, instruction counts).
 * One window per sample (needs the many-window plugin winsn_many.c):
 *   A. float32 Box-Muller pair (baseline from experiments/d2-sensor-gradinv/mcu/dpstep.c): 24-bit uniform u1, newlib logf/cosf/sinf
 *   B. Box-Muller with a full-exponent u1 (fixes the 5.77-sigma tail cut; still float32, still not FP-hole-safe)
 *   C. constant-time CDT discrete Gaussian base sampler, 64-bit precision, full table scan, sigma0 in {2, 8, 32}
 * Prints per-sample values so instruction count can be correlated with the noise value (timing-leak proxy:
 * the M33 is in-order with no data cache, so the executed-instruction path is the dominant timing source; FP/integer
 * divide and sqrt latencies are not modelled). */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bench.h"

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
static void rng_init(void) {
    static const uint32_t c[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
    memcpy(st, c, 16); for (int i = 4; i < 12; i++) st[i] = 0x9e3779b9u * i; st[12] = 0; st[13] = st[14] = st[15] = 1;
}
static inline uint32_t rng32_chacha(void) { if (pos == 16) { chacha_block(buf, st); st[12]++; pos = 0; } return buf[pos++]; }
/* samplers read pre-generated words (ChaCha20 cost is measured separately, per 64-B block) */
#define POOL 32768
static uint32_t pool[POOL]; static const uint32_t *src;
static inline uint32_t rng32(void) { return *src++; }
static void refill(void) { for (int i = 0; i < POOL; i++) pool[i] = rng32_chacha(); src = pool; }

/* A: baseline, identical to dpstep.c */
__attribute__((noinline)) static void bm_pair(float *a, float *b) {
    float u1 = ((rng32() >> 8) + 1) * (1.0f / 16777216.0f), u2 = (rng32() >> 8) * (1.0f / 16777216.0f);
    float r = sqrtf(-2.0f * logf(u1)), t = 6.28318530718f * u2;
    *a = r * cosf(t); *b = r * sinf(t);
}
/* B: u1 = (1 + m/2^23) * 2^-(1+z), z = leading zeros over up to 3 extra words (branch-free clz chain), so u1 reaches
 * 2^-97 and r reaches ~11.6 sigma. Exponent built with integer ops, no branches on secret data. */
__attribute__((noinline)) static void bm_pair_fullexp(float *a, float *b) {
    uint32_t w0 = rng32(), w1 = rng32(), w2 = rng32(), m = rng32() >> 9;
    uint32_t z0 = __builtin_clz(w0 | 1), z1 = __builtin_clz(w1 | 1), z2 = __builtin_clz(w2 | 1);
    uint32_t all0 = -(uint32_t)(w0 == 0), all1 = -(uint32_t)(w1 == 0);   /* compiled to flag-setting ops, no branch */
    uint32_t z = (w0 ? z0 : 32 + ((w1 ? z1 : 32 + z2)));
    (void)all0; (void)all1;
    uint32_t bits = ((126u - z) << 23) | m;             /* value in [2^-(1+z), 2^-z) */
    float u1; memcpy(&u1, &bits, 4);
    float u2 = (rng32() >> 8) * (1.0f / 16777216.0f);
    float r = sqrtf(-2.0f * logf(u1)), t = 6.28318530718f * u2;
    *a = r * cosf(t); *b = r * sinf(t);
}
/* C: constant-time CDT. T[i] = floor(2^63 * P(|X| > i)) for the one-sided folded distribution; sample = number of
 * entries with r < T[i] (full scan, branch-free 64-bit compare), sign from one bit, zero counted once. */
#define TMAX 2000
static uint64_t T[TMAX]; static int TL;
static void cdt_init(double sigma) {
    double p[TMAX + 1], tot = 0; TL = (int)ceil(12 * sigma); if (TL > TMAX) TL = TMAX;
    for (int i = 0; i <= TL; i++) { p[i] = exp(-(double)i * i / (2 * sigma * sigma)) * (i ? 2 : 1); tot += p[i]; }
    double tail = 1.0;
    for (int i = 0; i < TL; i++) { tail -= p[i] / tot; T[i] = (uint64_t)(tail > 0 ? tail * 9223372036854775808.0 : 0); }
}
__attribute__((noinline)) static int32_t cdt_sample(void) {
    uint64_t r = ((uint64_t)rng32() << 32 | rng32()) >> 1; uint32_t s = rng32() & 1; int32_t mag = 0;
    for (int i = 0; i < TL; i++) mag += (int32_t)((r - T[i]) >> 63);   /* 1 iff r < T[i] (both < 2^63) */
    return (mag ^ -(int32_t)s) + (int32_t)s;                           /* conditional negate */
}

#define NS 4000
static float va[NS]; static int32_t vc[NS];
int main(void) {
    rng_init(); float a, b;
    printf("K chacha20 block x8\n");
    static volatile uint32_t sink; for (int i = 0; i < 8; i++) { uint32_t o[16]; bench_start(); chacha_block(o, st); bench_stop(); st[12]++; sink ^= o[i]; }
    refill();
    printf("A bm_pair\n");
    for (int i = 0; i < NS / 2; i++) { bench_start(); bm_pair(&a, &b); bench_stop(); va[2 * i] = a; va[2 * i + 1] = b; }
    for (int i = 0; i < NS; i += 2) printf("VA %d %d\n", (int)(va[i] * 1e6f), (int)(va[i + 1] * 1e6f));
    /* forced extreme inputs: smallest u1 (all-zero words), u2 = 0, to show the hard tail cut on the device */
    { static const uint32_t z[8] = {0}; src = z; bm_pair(&a, &b); printf("XA rmax_x1e6 %d\n", (int)(a * 1e6f));
      src = z; bm_pair_fullexp(&a, &b); printf("XB rmax_x1e6 %d\n", (int)(a * 1e6f)); }
    refill();
    printf("B bm_pair_fullexp\n");
    for (int i = 0; i < NS / 2; i++) { bench_start(); bm_pair_fullexp(&a, &b); bench_stop(); va[2 * i] = a; va[2 * i + 1] = b; }
    for (int i = 0; i < NS; i += 2) printf("VB %d %d\n", (int)(va[i] * 1e6f), (int)(va[i + 1] * 1e6f));
    static const double SIG[3] = {46, 73, 146};   /* per-client sigma_int at N = 100 / 40 / 10 (C7) */
    for (int k = 0; k < 3; k++) {
        refill(); cdt_init(SIG[k]); printf("C cdt sigma %d entries %d\n", (int)SIG[k], TL);
        for (int i = 0; i < 1000; i++) { bench_start(); vc[i] = cdt_sample(); bench_stop(); }
        long s2 = 0; for (int i = 0; i < 1000; i++) s2 += (long)vc[i] * vc[i];
        printf("VC sigma %d empirical_var_x100 %ld\n", (int)SIG[k], s2 / 10);
    }
    printf("DONE fails 0\n");
    return 0;
}
