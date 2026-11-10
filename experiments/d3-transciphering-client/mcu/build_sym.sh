#!/usr/bin/env bash
# build_sym.sh <deps dir> <build dir> <plugin.so>: builds + runs the AEAD / AES baselines with the same flags.
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd); DEPS=$(realpath "$1"); B=$(realpath -m "$2"); PLUGIN=$(realpath "$3")
source "$HERE/common.sh"
build_one() { # name defs srcdir sources...
  local n=$1 defs=$2 inc=$3; shift 3; local W=$B/$n; rm -rf "$W"; mkdir -p "$W/obj"; local objs=()
  local CF="$ARCH $OPT -I$HERE/harness -I$inc -I$DEPS/ascon/tests $defs"
  for f in "$@"; do o=$W/obj/$(basename "$f").o; $CC $CF -c "$f" -o "$o"; objs+=("$o"); done
  $CC $CF -c "$HERE/bench/bench_sym.c" -o "$W/obj/main.o"
  $CC $CF -c "$HERE/harness/startup.c" -o "$W/obj/startup.o"
  $CC $CF -c "$HERE/harness/bench.c" -o "$W/obj/bench.o"
  $CC $ARCH "$W/obj/main.o" "$W/obj/startup.o" "$W/obj/bench.o" "${objs[@]}" $LDFLAGS -o "$W/$n.elf"
  arm-none-eabi-size -t "${objs[@]}" | tail -1 > "$W/cipher_size.txt"
  arm-none-eabi-size "$W/$n.elf" > "$W/elf_size.txt"
  run_elf "$W/$n.elf" "$W/run"; tail -2 "$W/run.log"
}
A=$DEPS/ascon/crypto_aead/asconaead128
build_one ascon_armv7m "-DUSE_ASCON" "$A/armv7m" "$A/armv7m/aead.c" "$A/armv7m/permutations.c"
build_one ascon_opt32  "-DUSE_ASCON" "$A/opt32"  "$A/opt32/aead.c"  "$A/opt32/permutations.c"
build_one aes_ctr_sw   "-DUSE_AES -DCTR=1 -DECB=0 -DCBC=0 -DAES128=1" "$DEPS/tinyaes" "$DEPS/tinyaes/aes.c"
