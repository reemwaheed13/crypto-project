// Generic client-side benchmark for an F_p HE-friendly cipher from IAIK hybrid-HE-framework.
// CIPHER_CLASS is the ZpCipher subclass (e.g. PASTA_4::PASTA). testvectors.h supplies the KATs.
#include <cstdio>
#include <vector>
#include "testvectors.h"
#include "bench.h"

static int bitlen(uint64_t p) { int b = 0; while (p) { b++; p >>= 1; } return b; }

int main() {
  int idx = 0, fails = 0;
  for (auto &t : KNOWN_ANSWER_TESTS) {
    if (t.tc != decltype(t.tc)::DEC) { idx++; continue; }
    CIPHER_CLASS c(t.key, t.modulus);
    bool enc_ok = c.encrypt(t.plaintext) == t.ciphertext_expected;
    bool dec_ok = c.decrypt(t.ciphertext_expected) == t.plaintext;
    if (!enc_ok || !dec_ok) fails++;
    std::printf("KAT %d modulus %llu bits %d t %u enc_ok %d dec_ok %d\n", idx, (unsigned long long)t.modulus,
                bitlen(t.modulus), (unsigned)c.get_plain_size(), enc_ok, dec_ok);
    std::vector<uint64_t> pt = t.plaintext, ct;
    for (int rep = 0; rep < 3; rep++) {
      size_t st, hp;
      mem_window_begin(); bench_start(); ct = c.encrypt(pt); bench_stop(); mem_window_end(&st, &hp);
      std::printf("WIN kat %d blocks 1 rep %d stack %u heap %u\n", idx, rep, (unsigned)st, (unsigned)hp);
    }
    std::vector<uint64_t> pt4; for (int b = 0; b < 4; b++) pt4.insert(pt4.end(), pt.begin(), pt.end());
    size_t st, hp;
    mem_window_begin(); bench_start(); ct = c.encrypt(pt4); bench_stop(); mem_window_end(&st, &hp);
    std::printf("WIN kat %d blocks 4 rep 0 stack %u heap %u\n", idx, (unsigned)st, (unsigned)hp);
    idx++;
  }
  std::printf("DONE fails %d\n", fails);
  return fails;
}
