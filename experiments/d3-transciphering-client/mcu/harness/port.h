/* port.h: force-included (-include) into every IAIK hybrid-HE-framework source when building for Cortex-M33.
 * The reference code assumes x86-64 Linux. Two gaps on arm-none-eabi:
 *  1. htobe64 / be64toh (glibc <endian.h>) do not exist in newlib: map them to the GCC byte-swap builtin
 *     (the M33 is little-endian, like x86).
 *  2. 32-bit ARM GCC has no __int128. The code only uses uint128_t as "(uint128_t)a * b % p" with a, b < p.
 *     For p < 2^32 the product fits in 64 bits, so uint64_t is EXACT there. Every known-answer test in the
 *     framework uses p = 65537 (17 bits). Moduli >= 2^32 are NOT supported by this shim: the KAT check would fail.
 * The "% p" on a 64-bit value then compiles to the libgcc helper __aeabi_uldivmod (software 64-bit division),
 * which the QEMU plugin attributes separately. That is what the unmodified reference code costs on this core. */
#pragma once
#include <stdint.h>
#define PORTABLE_ENDIAN_H__ 1   /* skip util/keccak/portable_endian.h, which #errors on bare metal */
#define htobe64(x) __builtin_bswap64((uint64_t)(x))
#define be64toh(x) __builtin_bswap64((uint64_t)(x))
#define htobe32(x) __builtin_bswap32((uint32_t)(x))
#define be32toh(x) __builtin_bswap32((uint32_t)(x))
#define htobe16(x) __builtin_bswap16((uint16_t)(x))
#define be16toh(x) __builtin_bswap16((uint16_t)(x))
#define htole64(x) ((uint64_t)(x))
#define le64toh(x) ((uint64_t)(x))
#define htole32(x) ((uint32_t)(x))
#define le32toh(x) ((uint32_t)(x))
#define htole16(x) ((uint16_t)(x))
#define le16toh(x) ((uint16_t)(x))
#ifndef __SIZEOF_INT128__
typedef uint64_t uint128_t;
typedef uint64_t __uint128_t;
#endif
