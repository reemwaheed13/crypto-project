#include <stdint.h>
#include <string.h>
extern uint32_t __stack_top, __data_start, __data_end, __data_load, __bss_start, __bss_end;
extern void __libc_init_array(void);
extern void initialise_monitor_handles(void);
extern int main(void);
extern void exit(int);
void Reset_Handler(void) {
    memcpy(&__data_start, &__data_load, (char *)&__data_end - (char *)&__data_start);
    memset(&__bss_start, 0, (char *)&__bss_end - (char *)&__bss_start);
    /* enable the FPU (CP10/CP11) — the M33 in the EFR32MG24 has one */
    *(volatile uint32_t *)0xE000ED88 |= (0xFu << 20);
    __asm volatile("dsb; isb");
    initialise_monitor_handles();
    __libc_init_array();
    exit(main());
}
static void Fault_Handler(void) { exit(99); }
__attribute__((section(".vectors"), used)) void (*const vectors[16])(void) = {
    (void (*)(void))&__stack_top, Reset_Handler, Fault_Handler, Fault_Handler, Fault_Handler,
    Fault_Handler, Fault_Handler, 0, 0, 0, 0, Fault_Handler, Fault_Handler, 0, Fault_Handler, Fault_Handler };
void _init(void) {}
void _fini(void) {}
/* normally from crtbegin.o, which -nostartfiles drops; used by C++ static destructors */
void *__dso_handle = 0;
