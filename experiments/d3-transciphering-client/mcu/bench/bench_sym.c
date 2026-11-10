/* Baseline symmetric encryption on the client: built twice, with -DUSE_ASCON (ascon-c Ascon-AEAD128) or
 * -DUSE_AES (tiny-AES-c AES-128-CTR, software, confidentiality only, no tag).
 * Each window = key setup + encrypt of N bytes (the HE-friendly windows also include key setup / keystream init).
 * KAT: Ascon LWC_AEAD_KAT_128_128 Count 545; AES NIST SP 800-38A F.5.1 (CTR-AES128). */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "bench.h"

static const uint8_t SIZES[] = {16, 64, 128, 255};   /* bytes; 255 so it fits a uint8_t table */

#ifdef USE_ASCON
#include "api.h"
int crypto_aead_encrypt(unsigned char *c, unsigned long long *clen, const unsigned char *m, unsigned long long mlen,
                        const unsigned char *ad, unsigned long long adlen, const unsigned char *nsec,
                        const unsigned char *npub, const unsigned char *k);
static int kat(void) {
    uint8_t k[16], n[16], p[16], a[16], c[32]; unsigned long long cl;
    static const uint8_t exp[32] = {0x63,0x73,0xEB,0xB2,0x8B,0xE9,0x7C,0x9B,0xAC,0x09,0x0C,0xF3,0x99,0xC1,0x3E,0xF1,
                                    0x3A,0xBF,0xC0,0xD2,0x09,0xE8,0xF4,0x84,0x4C,0x90,0x81,0x4D,0x13,0xF3,0x2C,0x59};
    for (int i = 0; i < 16; i++) { k[i] = i; n[i] = 0x10 + i; p[i] = 0x20 + i; a[i] = 0x30 + i; }
    crypto_aead_encrypt(c, &cl, p, 16, a, 16, 0, n, k);
    return cl == 32 && !memcmp(c, exp, 32);
}
static void enc(uint8_t *out, const uint8_t *in, unsigned len) {
    static const uint8_t k[16] = {1}, n[16] = {2}; unsigned long long cl;
    crypto_aead_encrypt(out, &cl, in, len, 0, 0, 0, n, k);
}
#define NAME "ascon_aead128"
#define OVERHEAD 16   /* tag */
#endif

#ifdef USE_AES
#include "aes.h"
static int kat(void) {
    static const uint8_t key[16] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
    static const uint8_t iv[16] = {0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff};
    static const uint8_t pt[16] = {0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a};
    static const uint8_t ct[16] = {0x87,0x4d,0x61,0x91,0xb6,0x20,0xe3,0x26,0x1b,0xef,0x68,0x64,0x99,0x0d,0xb6,0xce};
    uint8_t b[16]; struct AES_ctx x; memcpy(b, pt, 16);
    AES_init_ctx_iv(&x, key, iv); AES_CTR_xcrypt_buffer(&x, b, 16);
    return !memcmp(b, ct, 16);
}
static void enc(uint8_t *out, const uint8_t *in, unsigned len) {
    static const uint8_t k[16] = {1}, iv[16] = {2}; struct AES_ctx x;
    memcpy(out, in, len); AES_init_ctx_iv(&x, k, iv); AES_CTR_xcrypt_buffer(&x, out, len);
}
#define NAME "aes128_ctr_sw"
#define OVERHEAD 0    /* nonce/IV not counted for any cipher */
#endif

int main(void) {
    static uint8_t in[256], out[256 + 16];
    int ok = kat();
    printf("KAT %s ok %d\n", NAME, ok);
    for (unsigned i = 0; i < sizeof SIZES; i++)
        for (int rep = 0; rep < 3; rep++) {
            size_t st, hp; memset(in, 0x3c, SIZES[i]);
            mem_window_begin(); bench_start(); enc(out, in, SIZES[i]); bench_stop(); mem_window_end(&st, &hp);
            printf("WIN bytes %u rep %d ct_bytes %u stack %u heap %u\n", SIZES[i], rep, SIZES[i] + OVERHEAD,
                   (unsigned)st, (unsigned)hp);
        }
    printf("DONE fails %d\n", !ok);
    return !ok;
}
