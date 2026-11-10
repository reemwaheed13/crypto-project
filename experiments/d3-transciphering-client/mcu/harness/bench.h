/* Measurement helpers. Instruction counts come from the QEMU plugin (winsn), which
   counts between bench_start() and bench_stop(). Stack: painting. Heap: wrapped malloc. */
#pragma once
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void bench_start(void);
void bench_stop(void);
void mem_window_begin(void);   /* paint stack, reset heap peak */
void mem_window_end(size_t *stack_bytes, size_t *heap_peak_bytes);
size_t heap_live(void);
#ifdef __cplusplus
}
#endif
