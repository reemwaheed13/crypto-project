#!/usr/bin/env bash
# build.sh <build dir>: streaming-mask preliminary (C6) on the D3 QEMU harness, K in {8, 15}, with and without the
# contiguous 66 kB buffer variant. Prints instruction counts per window and static RAM (.bss) per build.
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd); D3=$(realpath "$HERE/../../d3-transciphering-client/mcu"); B=$(realpath -m "$1")
mkdir -p "$B"; gcc -O2 -shared -fPIC -I"$D3/deps/qemu" "$D3/plugin/winsn.c" -o "$B/libwinsn.so"; PLUGIN=$B/libwinsn.so
HS=$HERE; HERE=$D3; source "$D3/common.sh"; HERE=$HS
for K in 8 15; do for V in stream contig; do
  T=$B/k${K}_$V; mkdir -p "$T"; DEF="-DK=$K"; [ $V = contig ] && DEF="$DEF -DCONTIG"
  CF="$ARCH $OPT -I$D3/harness $DEF"
  $CC $CF -c "$HERE/stream_mask.c" -o "$T/m.o"; $CC $CF -c "$D3/harness/startup.c" -o "$T/s.o"; $CC $CF -c "$D3/harness/bench.c" -o "$T/b.o"
  $CC $ARCH "$T/m.o" "$T/s.o" "$T/b.o" $LDFLAGS -o "$T/m.elf"
  arm-none-eabi-size -A "$T/m.o" | awk '$1 ~ /^\.(bss|data)/ {s+=$2} END{print "'"k${K}_$V"' static_ram_bytes", s}'
  run_elf "$T/m.elf" "$T/run"; echo "k${K}_$V $(grep -E 'checks|exit=' $T/run.log | tr '\n' ' ')"; cat "$T/run.insn"
done; done
