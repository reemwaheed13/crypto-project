#!/usr/bin/env bash
# n1kill.sh <build dir> <setting> <sig_idx> <every>: builds n1kill.c with those switches and runs it under QEMU
# (no plugin: these runs need outputs, not instruction counts). Output: <build>/s<setting>_g<sig>_e<every>.log
set -euo pipefail
HERE=$(cd "$(dirname "$0")" && pwd); D3=$(realpath "$HERE/../d3-transciphering-client/mcu"); B=$(realpath -m "$1")
S=$2; G=$3; E=$4; T=s${S}_g${G}_e${E}; mkdir -p "$B/$T"
HERE_SAVE=$HERE; HERE=$D3; source "$D3/common.sh"; HERE=$HERE_SAVE
CF="$ARCH $OPT -I$D3/harness -DSETTING=$S -DSIG_IDX=$G -DEVERY=$E"
$CC $CF -c "$HERE/n1kill.c" -o "$B/$T/n1kill.o"
$CC $CF -c "$D3/harness/startup.c" -o "$B/$T/startup.o"
$CC $CF -c "$D3/harness/bench.c" -o "$B/$T/bench.o"
$CC $ARCH "$B/$T/n1kill.o" "$B/$T/startup.o" "$B/$T/bench.o" $LDFLAGS -lm -o "$B/$T/n1kill.elf"
timeout "${QEMU_TIMEOUT:-7200}" qemu-system-arm -M mps2-an505 -cpu cortex-m33 -nographic -monitor none -serial none \
  -semihosting-config enable=on,target=native -kernel "$B/$T/n1kill.elf" > "$B/$T.log" 2>&1 || echo "exit=$?" >> "$B/$T.log"
