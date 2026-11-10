# Shared toolchain settings. Same flags for every cipher. Sourced by build_hhe.sh.
CC=arm-none-eabi-gcc; CXX=arm-none-eabi-g++
ARCH="-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16"
OPT="-O3 -ffunction-sections -fdata-sections -fstack-usage"
LDFLAGS="-T $HERE/harness/link.ld -nostartfiles ${LIBC_SPECS:---specs=nano.specs} --specs=rdimon.specs -Wl,--gc-sections \
 -Wl,--wrap=malloc,--wrap=free,--wrap=calloc,--wrap=realloc"

# run_elf <elf> <outprefix>: run under QEMU mps2-an505 (Cortex-M33) with the instruction-window plugin.
# Ranges attribute instructions to the Keccak permutation, the 64-bit division helpers and Ascon.
run_elf() {
  local elf=$1 out=$2 ranges="" a n
  sym() { arm-none-eabi-nm -S "$elf" | awk -v n="$1" '$4==n{printf "%x:%x", strtonum("0x"$1), strtonum("0x"$1)+strtonum("0x"$2)}'; }
  for n in KeccakP1600_Permute_24rounds KeccakP1600_Permute_Nrounds __aeabi_uldivmod __udivmoddi4; do
    a=$(sym $n); [ -n "$a" ] && ranges="$ranges,range=$n:$a"
  done
  local st sp
  st=$(arm-none-eabi-nm "$elf" | awk '$3=="bench_start"{print $1}')
  sp=$(arm-none-eabi-nm "$elf" | awk '$3=="bench_stop"{print $1}')
  local rc=0
  timeout "${QEMU_TIMEOUT:-3000}" qemu-system-arm -M mps2-an505 -cpu cortex-m33 -nographic -monitor none -serial none \
    -semihosting-config enable=on,target=native -kernel "$elf" \
    -plugin "$PLUGIN,start=$st,stop=$sp,out=$out.insn$ranges" > "$out.log" 2>&1 || rc=$?
  echo "exit=$rc" >> "$out.log"   # 124 = timeout
}
