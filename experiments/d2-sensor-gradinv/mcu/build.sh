#!/usr/bin/env bash
# build.sh <build dir>: builds dpstep.c for Cortex-M33 and runs it under QEMU with the D3 harness and plugin.
# Needs the D3 deps fetched (for qemu-plugin.h): ../../d3-transciphering-client/mcu/fetch_deps.sh
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd); D3=$(realpath "$HERE/../../d3-transciphering-client/mcu"); B=$(realpath -m "$1")
mkdir -p "$B"; gcc -O2 -shared -fPIC -I"$D3/deps/qemu" "$D3/plugin/winsn.c" -o "$B/libwinsn.so"; PLUGIN=$B/libwinsn.so
HERE_D3=$D3; HERE=$D3 source "$D3/common.sh"; HERE=$(cd "$(dirname "$0")" && pwd)
CF="$ARCH $OPT -I$D3/harness"
$CC $CF -c "$HERE/dpstep.c" -o "$B/dpstep.o"
$CC $CF -c "$D3/harness/startup.c" -o "$B/startup.o"
$CC $CF -c "$D3/harness/bench.c" -o "$B/bench.o"
$CC $ARCH "$B/dpstep.o" "$B/startup.o" "$B/bench.o" $LDFLAGS -lm -o "$B/dpstep.elf"
arm-none-eabi-size "$B/dpstep.elf" | tee "$B/elf_size.txt"
run_elf "$B/dpstep.elf" "$B/run"
cat "$B/run.log" "$B/run.insn"
