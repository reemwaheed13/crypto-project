// Stub replacing the framework's kats.h: keeps the known-answer vectors as plain data.
#pragma once
#include <cstdint>
#include <vector>
#include "Cipher.h"
template <class T>
struct KnownAnswerTest {
  enum Testcase { MAT, DEC, USE_CASE, PREP };
  std::vector<uint8_t> key, plaintext, ciphertext_expected;
  Testcase tc; size_t N, bits;
  KnownAnswerTest(std::vector<uint8_t> k, std::vector<uint8_t> p, std::vector<uint8_t> c,
                  Testcase t = DEC, size_t n = 4, size_t b = 16)
      : key(k), plaintext(p), ciphertext_expected(c), tc(t), N(n), bits(b) {}
};
