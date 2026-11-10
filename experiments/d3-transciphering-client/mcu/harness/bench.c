#include "bench.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
extern uint32_t __stack_limit, __heap_start, __heap_end;
__attribute__((noinline)) void bench_start(void) { __asm volatile("" ::: "memory"); }
__attribute__((noinline)) void bench_stop(void)  { __asm volatile("" ::: "memory"); }

/* ---- heap: sbrk + malloc wrapper tracking live and peak bytes ---- */
static char *brk_ptr;
void *_sbrk(ptrdiff_t inc) {
    if (!brk_ptr) brk_ptr = (char *)&__heap_start;
    if (brk_ptr + inc > (char *)&__heap_end) { errno = ENOMEM; return (void *)-1; }
    char *old = brk_ptr; brk_ptr += inc; return old;
}
static size_t live, peak;
void *__real_malloc(size_t); void __real_free(void *); void *__real_realloc(void *, size_t);
void *__wrap_malloc(size_t n) {
    size_t *p = __real_malloc(n + 8); if (!p) return 0;
    p[0] = n; live += n; if (live > peak) peak = live; return (char *)p + 8;
}
void __wrap_free(void *q) { if (!q) return; size_t *p = (size_t *)((char *)q - 8); live -= p[0]; __real_free(p); }
void *__wrap_calloc(size_t a, size_t b) { void *q = __wrap_malloc(a * b); if (q) memset(q, 0, a * b); return q; }
void *__wrap_realloc(void *q, size_t n) {
    if (!q) return __wrap_malloc(n);
    size_t old = ((size_t *)((char *)q - 8))[0];
    void *r = __wrap_malloc(n); if (!r) return 0;
    memcpy(r, q, old < n ? old : n); __wrap_free(q); return r;
}
size_t heap_live(void) { return live; }

/* ---- stack painting ---- */
#define PAINT 0xA5A5A5A5u
static uint32_t *paint_top;
static size_t live_at_begin;
__attribute__((noinline)) void mem_window_begin(void) {
    uint32_t *sp; __asm volatile("mov %0, sp" : "=r"(sp));
    paint_top = sp - 64;                       /* leave this frame alone */
    for (uint32_t *p = &__stack_limit; p < paint_top; p++) *p = PAINT;
    live_at_begin = live; peak = live;
}
__attribute__((noinline)) void mem_window_end(size_t *stack_bytes, size_t *heap_peak_bytes) {
    uint32_t *p = &__stack_limit;
    while (p < paint_top && *p == PAINT) p++;
    *stack_bytes = (size_t)((char *)paint_top - (char *)p) + 64 * 4;  /* upper bound: includes 256 B margin */
    *heap_peak_bytes = peak - live_at_begin;
}
