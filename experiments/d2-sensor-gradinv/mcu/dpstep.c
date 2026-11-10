/* D2 on-device cost: one FedAvg local step of the TS-Inverse FCN forecaster (96 -> 64 -> 64 -> 96, sigmoid hidden,
 * linear output, MSE), batch B = 4, float32, on Cortex-M33. Built twice from this file? No: one ELF, four windows:
 *   W0 plain step     : 4 x (forward + backward accumulated into one gradient buffer) + SGD update
 *   W1 DP step        : 4 x (forward + backward into a per-example buffer, L2-clip to C, accumulate)
 *                       + Gaussian noise N(0, (sigma*C)^2) on all 16,608 coordinates + SGD update  (DP-SGD, Abadi et al.)
 *   W2 noise only     : the noise part of W1 alone (ChaCha20 CSPRNG -> Box-Muller), to attribute cost
 *   W3 clip only      : norm + scale of one per-example gradient
 * Gaussian sampling is textbook floating-point Box-Muller, which is NOT a sound DP sampler (Mironov, CCS 2012): it is
 * costed here as the common baseline; a discrete Gaussian sampler is the hardened alternative.
 * ChaCha20 self-test: RFC 8439 sec. 2.3.2 block-function vector. */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bench.h"

#define H 96
#define F 96
#define HID 64
#define B 4
/* parameter layout: W1[HID][H] b1[HID] W2[HID][HID] b2[HID] W3[F][HID] b3[F] */
#define NP (HID * H + HID + HID * HID + HID + F * HID + F)
static float w[NP], g[NP], ge[NP];          /* weights, gradient accumulator, per-example gradient (DP only) */
static float xb[B][H], yb[B][F];

/* ---------- ChaCha20 (RFC 8439) as a CSPRNG ---------- */
#define ROTL(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define QR(a, b, c, d) (a += b, d ^= a, d = ROTL(d, 16), c += d, b ^= c, b = ROTL(b, 12), \
                        a += b, d ^= a, d = ROTL(d, 8),  c += d, b ^= c, b = ROTL(b, 7))
static void chacha_block(uint32_t out[16], const uint32_t in[16]) {
    uint32_t x[16]; memcpy(x, in, 64);
    for (int i = 0; i < 10; i++) {
        QR(x[0], x[4], x[8], x[12]); QR(x[1], x[5], x[9], x[13]); QR(x[2], x[6], x[10], x[14]); QR(x[3], x[7], x[11], x[15]);
        QR(x[0], x[5], x[10], x[15]); QR(x[1], x[6], x[11], x[12]); QR(x[2], x[7], x[8], x[13]); QR(x[3], x[4], x[9], x[14]);
    }
    for (int i = 0; i < 16; i++) out[i] = x[i] + in[i];
}
static uint32_t st[16], buf[16]; static int pos = 16;
static void rng_init(const uint8_t key[32], const uint8_t nonce[12], uint32_t ctr) {
    static const uint32_t c[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
    memcpy(st, c, 16); memcpy(st + 4, key, 32); st[12] = ctr; memcpy(st + 13, nonce, 12); pos = 16;
}
static inline uint32_t rng32(void) {
    if (pos == 16) { chacha_block(buf, st); st[12]++; pos = 0; }
    return buf[pos++];
}
static int chacha_kat(void) {   /* RFC 8439 2.3.2: key 00..1f, nonce 00:00:00:09:00:00:00:4a:00:00:00:00, counter 1 */
    uint8_t k[32], n[12] = {0, 0, 0, 9, 0, 0, 0, 0x4a, 0, 0, 0, 0}; uint32_t o[16];
    for (int i = 0; i < 32; i++) k[i] = i;
    rng_init(k, n, 1); chacha_block(o, st);
    return o[0] == 0xe4e7f110 && o[1] == 0x15593bd1 && o[12] == 0xd19c12b5 && o[15] == 0x4e3c50a2;
}
/* Box-Muller: two uniforms in (0,1] -> two N(0,1) */
static inline void gauss2(float *a, float *b) {
    float u1 = ((rng32() >> 8) + 1) * (1.0f / 16777216.0f), u2 = (rng32() >> 8) * (1.0f / 16777216.0f);
    float r = sqrtf(-2.0f * logf(u1)), t = 6.28318530718f * u2;
    *a = r * cosf(t); *b = r * sinf(t);
}

/* ---------- FCN forward/backward for one example, gradient ADDED into gout ---------- */
static inline float sig(float z) { return 1.0f / (1.0f + expf(-z)); }
static void fwd_bwd(const float *x, const float *y, float *gout) {
    const float *W1 = w, *b1 = W1 + HID * H, *W2 = b1 + HID, *b2 = W2 + HID * HID, *W3 = b2 + HID, *b3 = W3 + F * HID;
    float *gW1 = gout, *gb1 = gW1 + HID * H, *gW2 = gb1 + HID, *gb2 = gW2 + HID * HID, *gW3 = gb2 + HID, *gb3 = gW3 + F * HID;
    float h1[HID], h2[HID], d3[F], d2[HID], d1[HID];
    for (int i = 0; i < HID; i++) { float s = b1[i]; for (int j = 0; j < H; j++) s += W1[i * H + j] * x[j]; h1[i] = sig(s); }
    for (int i = 0; i < HID; i++) { float s = b2[i]; for (int j = 0; j < HID; j++) s += W2[i * HID + j] * h1[j]; h2[i] = sig(s); }
    for (int i = 0; i < F; i++) { float s = b3[i]; for (int j = 0; j < HID; j++) s += W3[i * HID + j] * h2[j];
                                  d3[i] = 2.0f * (s - y[i]) / F; }                 /* dMSE/dout */
    for (int j = 0; j < HID; j++) { float s = 0; for (int i = 0; i < F; i++) s += W3[i * HID + j] * d3[i]; d2[j] = s * h2[j] * (1 - h2[j]); }
    for (int i = 0; i < F; i++) { gb3[i] += d3[i]; for (int j = 0; j < HID; j++) gW3[i * HID + j] += d3[i] * h2[j]; }
    for (int j = 0; j < HID; j++) { float s = 0; for (int i = 0; i < HID; i++) s += W2[i * HID + j] * d2[i]; d1[j] = s * h1[j] * (1 - h1[j]); }
    for (int i = 0; i < HID; i++) { gb2[i] += d2[i]; for (int j = 0; j < HID; j++) gW2[i * HID + j] += d2[i] * h1[j]; }
    for (int i = 0; i < HID; i++) { gb1[i] += d1[i]; for (int j = 0; j < H; j++) gW1[i * H + j] += d1[i] * x[j]; }
}
__attribute__((noinline)) static void clip_acc(float C) {
    float n2 = 0; for (int k = 0; k < NP; k++) n2 += ge[k] * ge[k];
    float s = sqrtf(n2) > C ? C / sqrtf(n2) : 1.0f;
    for (int k = 0; k < NP; k++) g[k] += s * ge[k];
}
__attribute__((noinline)) static void add_noise(float std) {
    for (int k = 0; k < NP; k += 2) { float a, b; gauss2(&a, &b); g[k] += std * a; if (k + 1 < NP) g[k + 1] += std * b; }
}
static void update(float lr) { for (int k = 0; k < NP; k++) w[k] -= lr * g[k] / B; }

__attribute__((noinline)) static void step_plain(float lr) {
    memset(g, 0, sizeof g);
    for (int b = 0; b < B; b++) fwd_bwd(xb[b], yb[b], g);
    update(lr);
}
__attribute__((noinline)) static void step_dp(float lr, float C, float sigma) {
    memset(g, 0, sizeof g);
    for (int b = 0; b < B; b++) { memset(ge, 0, sizeof ge); fwd_bwd(xb[b], yb[b], ge); clip_acc(C); }
    add_noise(sigma * C);
    update(lr);
}

int main(void) {
    int ok = chacha_kat();
    printf("KAT chacha20 ok %d params %d\n", ok, NP);
    uint8_t key[32] = {7}, nonce[12] = {1};
    rng_init(key, nonce, 0);
    for (int k = 0; k < NP; k++) w[k] = ((int)(rng32() % 2001) - 1000) * 1e-4f;
    for (int b = 0; b < B; b++) for (int j = 0; j < H; j++) { xb[b][j] = 0.5f + 0.4f * sinf(0.065f * (j + 17 * b)); yb[b][j] = xb[b][j]; }
    size_t s_, h_;
    for (int rep = 0; rep < 3; rep++) {
        mem_window_begin(); bench_start(); step_plain(0.05f); bench_stop(); mem_window_end(&s_, &h_);
        printf("WIN plain rep %d stack %u\n", rep, (unsigned)s_);
        mem_window_begin(); bench_start(); step_dp(0.05f, 1.0f, 1.0f); bench_stop(); mem_window_end(&s_, &h_);
        printf("WIN dp rep %d stack %u\n", rep, (unsigned)s_);
        mem_window_begin(); bench_start(); add_noise(1.0f); bench_stop(); mem_window_end(&s_, &h_);
        printf("WIN noise rep %d stack %u\n", rep, (unsigned)s_);
        mem_window_begin(); bench_start(); clip_acc(1.0f); bench_stop(); mem_window_end(&s_, &h_);
        printf("WIN clip rep %d stack %u\n", rep, (unsigned)s_);
    }
    printf("CHECK w0 %d\n", (int)(w[0] * 1e6f));
    printf("DONE fails %d\n", !ok);
    return !ok;
}
