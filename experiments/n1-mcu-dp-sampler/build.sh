#!/usr/bin/env bash
# build.sh <build dir> [source stem, default sampler]: builds <stem>.c for Cortex-M33 and runs it under QEMU with the many-window plugin.
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd); D3=$(realpath "$HERE/../d3-transciphering-client/mcu"); B=$(realpath -m "$1")
mkdir -p "$B"; gcc -O2 -shared -fPIC -I"$D3/deps/qemu" "$HERE/winsn_many.c" -o "$B/libwinsn_many.so"; PLUGIN=$B/libwinsn_many.so
HERE_SAVE=$HERE; HERE=$D3; source "$D3/common.sh"; HERE=$HERE_SAVE
CF="$ARCH $OPT -I$D3/harness"
$CC $CF -c "$HERE/${2:-sampler}.c" -o "$B/sampler.o"
$CC $CF -c "$D3/harness/startup.c" -o "$B/startup.o"
$CC $CF -c "$D3/harness/bench.c" -o "$B/bench.o"
$CC $ARCH "$B/sampler.o" "$B/startup.o" "$B/bench.o" $LDFLAGS -lm -o "$B/sampler.elf"
run_elf "$B/sampler.elf" "$B/run"
grep -E "^(A|B|C) |^VC|DONE|exit=" "$B/run.log"; wc -l "$B/run.insn"
