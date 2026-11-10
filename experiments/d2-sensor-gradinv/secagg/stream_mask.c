/* C6 preliminary: secure-aggregation masking that fits in a sensor's RAM (Cortex-M33, QEMU instruction counts).
 * The client's update (16,608 float32, D2's FCN) is quantised to 32-bit integers mod 2^32 on the fly and masked with
 * K pairwise ChaCha20 masks + 1 self mask, ONE 802.15.4 FRAME AT A TIME (100 B of payload = 25 words per frame), so
 * the masked vector never exists in RAM. Three variants, one window each:
 *   W0 contiguous : expand every mask over the whole vector into a 66 kB buffer, then add (the textbook way; RAM O(d))
 *   W1 stream+buf : per frame, per peer, continue that peer's keystream; a 64-B leftover block per peer (RAM O(K))
 *   W2 stream+seek: per frame, per peer, regenerate the ChaCha blocks covering the frame's byte range from the block
 *                   counter (stateless, seekable; wastes partial blocks; also what a retransmission costs)
 * The PRG is the RFC 8439 ChaCha20 block function (self-test in dpstep.c). A non-seekable XOF (SHAKE/Ascon) can do W1
 * but not W2: a lost frame would need the XOF re-run from the start. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bench.h"

#define NP 16608
#define WORDS_PER_FRAME 25                         /* 100 B payload per 802.15.4 frame (ASSUMPTION, header budget) */
#define NFRAMES ((NP + WORDS_PER_FRAME - 1) / WORDS_PER_FRAME)
#ifndef K
#define K 15                                       /* pairwise peers (SecAgg+ at N = 100 with k = 2 log2 N) */
#endif
#define NM (K + 1)                                 /* + self mask */

static float g[NP];                                /* the DP-SGD gradient accumulator / update (exists anyway) */
static volatile uint32_t radio_sink;               /* stands in for the radio FIFO */

#define ROTL(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define QR(a, b, c, d) (a += b, d ^= a, d = ROTL(d, 16), c += d, b ^= c, b = ROTL(b, 12), \
                        a += b, d ^= a, d = ROTL(d, 8),  c += d, b ^= c, b = ROTL(b, 7))
static void chacha_block(uint32_t out[16], const uint32_t key[8], uint32_t ctr) {
    uint32_t in[16] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574}, x[16];
    memcpy(in + 4, key, 32); in[12] = ctr; in[13] = in[14] = in[15] = 0; memcpy(x, in, 64);
    for (int i = 0; i < 10; i++) {
        QR(x[0], x[4], x[8], x[12]); QR(x[1], x[5], x[9], x[13]); QR(x[2], x[6], x[10], x[14]); QR(x[3], x[7], x[11], x[15]);
        QR(x[0], x[5], x[10], x[15]); QR(x[1], x[6], x[11], x[12]); QR(x[2], x[7], x[8], x[13]); QR(x[3], x[4], x[9], x[14]);
    }
    for (int i = 0; i < 16; i++) out[i] = x[i] + in[i];
}
static uint32_t keys[NM][8]; static int32_t sign_[NM];
static inline uint32_t quant(float v) { return (uint32_t)(int32_t)(v * 65536.0f); }   /* fixed-point 16.16 (ASSUMPTION) */

#ifdef CONTIG
static uint32_t big[NP];                           /* the 66 kB buffer streaming avoids */
__attribute__((noinline)) static uint32_t contiguous(void) {
    for (int k = 0; k < NP; k++) big[k] = quant(g[k]);
    for (int m = 0; m < NM; m++) {
        uint32_t blk[16];
        for (int b = 0; b * 16 < NP; b++) { chacha_block(blk, keys[m], b);
            int n = NP - b * 16 < 16 ? NP - b * 16 : 16;
            for (int i = 0; i < n; i++) big[b * 16 + i] += sign_[m] * blk[i]; }
    }
    uint32_t acc = 0; for (int k = 0; k < NP; k++) acc ^= big[k]; radio_sink = acc; return acc;
}
#endif
static uint32_t left[NM][16]; static int lpos[NM]; static uint32_t lctr[NM];
__attribute__((noinline)) static uint32_t stream_buf(void) {
    for (int m = 0; m < NM; m++) { lpos[m] = 16; lctr[m] = 0; }
    uint32_t acc = 0;
    for (int f = 0; f < NFRAMES; f++) {
        uint32_t fr[WORDS_PER_FRAME]; int base = f * WORDS_PER_FRAME, n = NP - base < WORDS_PER_FRAME ? NP - base : WORDS_PER_FRAME;
        for (int i = 0; i < n; i++) fr[i] = quant(g[base + i]);
        for (int m = 0; m < NM; m++) for (int i = 0; i < n; i++) {
            if (lpos[m] == 16) { chacha_block(left[m], keys[m], lctr[m]++); lpos[m] = 0; }
            fr[i] += sign_[m] * left[m][lpos[m]++];
        }
        for (int i = 0; i < n; i++) acc ^= fr[i];          /* "transmit" */
    }
    radio_sink = acc; return acc;
}
__attribute__((noinline)) static uint32_t stream_seek(void) {
    uint32_t acc = 0;
    for (int f = 0; f < NFRAMES; f++) {
        uint32_t fr[WORDS_PER_FRAME], blk[16]; int base = f * WORDS_PER_FRAME, n = NP - base < WORDS_PER_FRAME ? NP - base : WORDS_PER_FRAME;
        for (int i = 0; i < n; i++) fr[i] = quant(g[base + i]);
        for (int m = 0; m < NM; m++) {
            int cur = -1;
            for (int i = 0; i < n; i++) { int w = base + i, b = w >> 4;
                if (b != cur) { chacha_block(blk, keys[m], (uint32_t)b); cur = b; }
                fr[i] += sign_[m] * blk[w & 15]; }
        }
        for (int i = 0; i < n; i++) acc ^= fr[i];
    }
    radio_sink = acc; return acc;
}

int main(void) {
    for (int m = 0; m < NM; m++) { for (int j = 0; j < 8; j++) keys[m][j] = 0x9e3779b9u * (m * 8 + j + 1); sign_[m] = (m & 1) ? -1 : 1; }
    for (int k = 0; k < NP; k++) g[k] = (float)((k * 2654435761u) % 2001) * 1e-4f - 0.1f;
    uint32_t r1, r2, r3 = 0;
#ifdef CONTIG
    bench_start(); r3 = contiguous(); bench_stop();
#endif
    bench_start(); r1 = stream_buf(); bench_stop();
    bench_start(); r2 = stream_seek(); bench_stop();
    printf("K %d frames %d words %d checks buf=%08lx seek=%08lx contig=%08lx same=%d\n", K, NFRAMES, NP,
           (unsigned long)r1, (unsigned long)r2, (unsigned long)r3, r1 == r2 && (r3 == 0 || r3 == r1));
    printf("DONE fails %d\n", r1 != r2);
    return r1 != r2;
}
