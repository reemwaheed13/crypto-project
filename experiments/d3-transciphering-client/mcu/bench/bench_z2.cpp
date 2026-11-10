// Generic client-side benchmark for a Z_2 HE-friendly cipher from IAIK hybrid-HE-framework.
#include <cstdio>
#include <vector>
#include "testvectors.h"
#include "bench.h"

// Upstream bug (hybrid-HE-framework @dafd7e7): the *_packed ciphers' encrypt(pt, n) computes
// num_block = ceil(n * 8 / plain_size_bits), i.e. it reads n as BYTES, while decrypt reads n as bits.
// The framework's own DEC KATs only call decrypt, so this was never exercised. Build packed ciphers with
// -DENC_LEN_IS_BYTES so encrypt produces exactly the intended number of blocks.
#ifdef ENC_LEN_IS_BYTES
#define ENC_LEN(bits) ((bits) / 8)
#else
#define ENC_LEN(bits) (bits)
#endif

int main() {
  int idx = 0, fails = 0;
  for (auto &t : KNOWN_ANSWER_TESTS) {
    if (t.tc != decltype(t.tc)::DEC) { idx++; continue; }
    CIPHER_CLASS c(t.key);
    size_t pb = c.get_plain_size_bits(), cb = c.get_cipher_size_bits();
    bool enc_ok = c.encrypt(t.plaintext, ENC_LEN(pb)) == t.ciphertext_expected;
    bool dec_ok = c.decrypt(t.ciphertext_expected, cb) == t.plaintext;
    if (!enc_ok || !dec_ok) fails++;
    std::printf("KAT %d plain_bits %u enc_ok %d dec_ok %d\n", idx, (unsigned)pb, enc_ok, dec_ok);
    std::vector<uint8_t> pt = t.plaintext, ct;
    for (int rep = 0; rep < 3; rep++) {
      size_t st, hp;
      mem_window_begin(); bench_start(); ct = c.encrypt(pt, ENC_LEN(pb)); bench_stop(); mem_window_end(&st, &hp);
      std::printf("WIN kat %d blocks 1 rep %d stack %u heap %u\n", idx, rep, (unsigned)st, (unsigned)hp);
    }
    std::vector<uint8_t> pt4(4 * c.get_plain_size_bytes(), 0x3c);
    size_t st, hp;
    mem_window_begin(); bench_start(); ct = c.encrypt(pt4, ENC_LEN(4 * pb)); bench_stop(); mem_window_end(&st, &hp);
    std::printf("WIN kat %d blocks 4 rep 0 stack %u heap %u\n", idx, (unsigned)st, (unsigned)hp);
    idx++;
  }
  std::printf("DONE fails %d\n", fails);
  return fails;
}
