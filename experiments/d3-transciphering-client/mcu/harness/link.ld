/* QEMU mps2-an505 (Cortex-M33), secure aliases. Code in SSRAM1, data/heap/stack in SSRAM2/3.
   QEMU's RAM is far larger than the EFR32MG24's 256 kB; the harness REPORTS RAM use, it does not cap it. */
MEMORY { CODE (rx) : ORIGIN = 0x10000000, LENGTH = 4M
         RAM (rwx) : ORIGIN = 0x38000000, LENGTH = 4M }
ENTRY(Reset_Handler)
_stack_size = 0x40000;   /* 256 kB stack region, painted before each window */
SECTIONS {
  .text : { KEEP(*(.vectors)) *(.text*) *(.rodata*)
            . = ALIGN(4); __init_array_start = .; KEEP(*(.init_array*)) __init_array_end = .;
            KEEP(*(.init)) KEEP(*(.fini)) } > CODE
  .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } > CODE
  .ARM.exidx : { __exidx_start = .; *(.ARM.exidx*) __exidx_end = .; } > CODE
  .data : { __data_start = .; *(.data*) . = ALIGN(4); __data_end = .; } > RAM AT > CODE
  __data_load = LOADADDR(.data);
  .bss (NOLOAD) : { __bss_start = .; *(.bss*) *(COMMON) . = ALIGN(8); __bss_end = .; } > RAM
  .heap (NOLOAD) : { __heap_start = .; end = .; . = ORIGIN(RAM) + LENGTH(RAM) - _stack_size; __heap_end = .; } > RAM
  .stack (NOLOAD) : { __stack_limit = .; . += _stack_size; __stack_top = .; } > RAM
}
