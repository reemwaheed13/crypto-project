// Stub replacing the framework's kats.h: keeps the known-answer vectors as plain data.
#pragma once
#include <cstdint>
#include <vector>
#include "Cipher.h"
template <class T>
struct KnownAnswerTestZp {
  enum Testcase { MAT, DEC, USE_CASE, PREP };
  std::vector<uint64_t> key, plaintext, ciphertext_expected;
  size_t modulus; Testcase tc; size_t N;
  KnownAnswerTestZp(std::vector<uint64_t> k, std::vector<uint64_t> p, std::vector<uint64_t> c,
                    size_t m, Testcase t = DEC, size_t n = 4)
      : key(k), plaintext(p), ciphertext_expected(c), modulus(m), tc(t), N(n) {}
};
