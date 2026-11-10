#!/usr/bin/env bash
# build_hhe.sh <deps dir> <build dir> <plugin.so> <cipher dir> <zp|z2> <class>
# Builds one IAIK hybrid-HE-framework "plain" cipher for Cortex-M33 and runs it under QEMU.
# Example: ./build_hhe.sh deps build build/libwinsn.so pasta_4 zp PASTA
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd); DEPS=$(realpath "$1"); B=$(realpath -m "$2"); PLUGIN=$(realpath "$3")
C=$4; KIND=$5; CLS=$6
source "$HERE/common.sh"
H=$DEPS/hhe; W=$B/$C; rm -rf "$W"; mkdir -p "$W/ciphers" "$W/obj"
cp -r "$H/ciphers/$C" "$W/ciphers/"; cp -r "$H/ciphers/common" "$H/ciphers/common_Zp" "$W/ciphers/"
cp "$HERE/harness/stubs/common/kats.h" "$W/ciphers/common/kats.h"
cp "$HERE/harness/stubs/common_Zp/kats.h" "$W/ciphers/common_Zp/kats.h"
INC="-I$W/ciphers/$C/plain -I$H/util/keccak -I$H/util/keccak/opt64 -I$H/util/aes -I$HERE/harness ${EXTRA_INC:-}"
CF="$ARCH $OPT -include $HERE/harness/port.h $INC ${EXTRA_DEF:-}"
cipher_objs=()
for f in "$W/ciphers/$C/plain/"*.cpp; do
  o=$W/obj/$(basename "$f").o; $CXX $CF -std=gnu++17 -c "$f" -o "$o"; cipher_objs+=("$o"); done
for f in "$H/util/keccak/KeccakHash.c" "$H/util/keccak/KeccakSpongeWidth1600.c" "$H/util/keccak/opt64/KeccakP-1600-opt64.c"; do
  o=$W/obj/$(basename "$f").o; $CC $CF -c "$f" -o "$o"; cipher_objs+=("$o"); done
if grep -q osuCrypto "$W/ciphers/$C/plain/"*.h; then
  for f in "$H/util/aes/aes.cpp" "$H/util/aes/block.cpp"; do
    o=$W/obj/$(basename "$f").o; $CXX $CF -std=gnu++17 -c "$f" -o "$o"; cipher_objs+=("$o"); done
fi
$CXX $CF -std=gnu++17 -DCIPHER_CLASS=$CLS -c "$HERE/bench/bench_$KIND.cpp" -o "$W/obj/main.o"
$CC $CF -c "$HERE/harness/startup.c" -o "$W/obj/startup.o"
$CC $CF -c "$HERE/harness/bench.c" -o "$W/obj/bench.o"
$CXX $ARCH "$W/obj/main.o" "$W/obj/startup.o" "$W/obj/bench.o" "${cipher_objs[@]}" $LDFLAGS -o "$W/$C.elf"
# Code size: the cipher's own objects only (excludes libc, libstdc++ and the harness).
arm-none-eabi-size -t "${cipher_objs[@]}" | tail -1 > "$W/cipher_size.txt"
arm-none-eabi-size "$W/$C.elf" > "$W/elf_size.txt"
run_elf "$W/$C.elf" "$W/run"
tail -3 "$W/run.log"
