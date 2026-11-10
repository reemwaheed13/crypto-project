#!/usr/bin/env bash
# run_all.sh <deps> <build> [jobs] [cipher ...]: builds + runs IAIK hybrid-HE-framework plain ciphers under QEMU.
# With no cipher names, runs all of them except he_only* and the m4ri-dependent builds (see SKIP below).
set -uo pipefail
HERE=$(cd "$(dirname "$0")" && pwd); DEPS=$(realpath "$1"); B=$(realpath -m "$2"); J=${3:-6}; shift 3 || shift $#
mkdir -p "$B"; gcc -O2 -shared -fPIC -I"$DEPS/qemu" "$HERE/plugin/winsn.c" -o "$B/libwinsn.so"
# Need the external M4RI library (not ported). Their *_packed builds implement the same ciphers without it,
# so only LowMC is lost.
SKIP="agrasta dasta_5 dasta_6 rasta_5 rasta_6 lowmc"
for h in "$DEPS"/hhe/ciphers/*/plain/*_plain.h; do
  d=$(basename "$(dirname "$(dirname "$h")")"); case $d in he_only*) continue;; esac
  ns=$(grep -oP '^namespace \K\w+' "$h"); cls=$(grep -oP 'class \K\w+(?= *: *public (BlockCipher|ZpCipher))' "$h")
  grep -q 'public ZpCipher' "$h" && k=zp || k=z2
  echo "$d $k $ns::$cls"
done > "$B/ciphers.txt"
if [ $# -gt 0 ]; then sel=" $* "; else sel=""; fi
awk -v skip=" $SKIP " -v sel="$sel" '{ if (sel != "" ? index(sel, " "$1" ") : !index(skip, " "$1" ")) print $1, $2, $3 }' \
  "$B/ciphers.txt" |
xargs -P "$J" -L 1 sh -c '
  case $3 in
    *_packed) export EXTRA_DEF=-DENC_LEN_IS_BYTES ;;      # upstream encrypt() length bug, see bench_z2.cpp
    filip_*)  export EXTRA_DEF=-DOC_ENABLE_PORTABLE_AES ;; # util/aes: no AES-NI on Cortex-M
    pasta2_*) export LIBC_SPECS=" " ;;                     # debug std::cout needs wide printf: full newlib
  esac
  "$0/build_hhe.sh" "$1" "$2" "$2/libwinsn.so" "$3" "$4" "$5" > "$2/$3.buildlog" 2>&1; echo "$3 rc=$?"' \
  "$HERE" "$DEPS" "$B"
