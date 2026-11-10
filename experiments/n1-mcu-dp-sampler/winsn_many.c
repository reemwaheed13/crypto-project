/* winsn: QEMU TCG plugin counting executed guest instructions inside measurement
 * windows delimited by two marker functions (bench_start / bench_stop), plus a
 * breakdown by named address ranges (e.g. Keccak permutation, 64-bit division).
 * Args: start=<hex>,stop=<hex>,out=<file>[,range=<name>:<lo hex>:<hi hex>]...
 * Counts are INSTRUCTIONS, not cycles. Attribution is per instruction address. */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qemu-plugin.h"

QEMU_PLUGIN_EXPORT int qemu_plugin_version = QEMU_PLUGIN_VERSION;

#define MAXR 8
#define MAXW (1 << 20)   /* many short windows: one per noise sample */
static uint64_t a_start, a_stop;
static int nr;
static char rname[MAXR][32];
static uint64_t rlo[MAXR], rhi[MAXR];
static int counting;
static uint64_t cur_total, cur_r[MAXR];
static int nw;
static uint64_t *w_total; static uint64_t (*w_r)[MAXR];
static FILE *out;

struct tbinfo { uint32_t n; uint32_t nr_[MAXR]; };

static void tb_exec(unsigned int vcpu, void *ud) {
    struct tbinfo *t = ud;
    if (!counting) return;
    cur_total += t->n;
    for (int i = 0; i < nr; i++) cur_r[i] += t->nr_[i];
}
static void on_start(unsigned int vcpu, void *ud) {
    counting = 1; cur_total = 0; memset(cur_r, 0, sizeof cur_r);
}
static void on_stop(unsigned int vcpu, void *ud) {
    if (!counting) return;
    counting = 0;
    if (nw < MAXW) { w_total[nw] = cur_total; memcpy(w_r[nw], cur_r, sizeof cur_r); nw++; }
}
static void tb_trans(qemu_plugin_id_t id, struct qemu_plugin_tb *tb) {
    size_t n = qemu_plugin_tb_n_insns(tb);
    struct tbinfo *t = calloc(1, sizeof *t);   /* leaked deliberately: lives as long as the TB */
    t->n = n;
    for (size_t i = 0; i < n; i++) {
        struct qemu_plugin_insn *in = qemu_plugin_tb_get_insn(tb, i);
        uint64_t a = qemu_plugin_insn_vaddr(in);
        for (int r = 0; r < nr; r++) if (a >= rlo[r] && a < rhi[r]) t->nr_[r]++;
        if (a == a_start) qemu_plugin_register_vcpu_insn_exec_cb(in, on_start, QEMU_PLUGIN_CB_NO_REGS, NULL);
        if (a == a_stop)  qemu_plugin_register_vcpu_insn_exec_cb(in, on_stop,  QEMU_PLUGIN_CB_NO_REGS, NULL);
    }
    qemu_plugin_register_vcpu_tb_exec_cb(tb, tb_exec, QEMU_PLUGIN_CB_NO_REGS, t);
}
static void at_exit(qemu_plugin_id_t id, void *p) {
    FILE *f = out ? out : stderr;
    for (int w = 0; w < nw; w++) {
        fprintf(f, "window %d total %" PRIu64, w, w_total[w]);
        for (int r = 0; r < nr; r++) fprintf(f, " %s %" PRIu64, rname[r], w_r[w][r]);
        fprintf(f, "\n");
    }
    if (out) fclose(out);
}
QEMU_PLUGIN_EXPORT int qemu_plugin_install(qemu_plugin_id_t id, const qemu_info_t *info, int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        char *v = strchr(argv[i], '='); if (!v) return -1; *v++ = 0;
        if (!strcmp(argv[i], "start")) a_start = strtoull(v, NULL, 16);
        else if (!strcmp(argv[i], "stop")) a_stop = strtoull(v, NULL, 16);
        else if (!strcmp(argv[i], "out")) out = fopen(v, "w");
        else if (!strcmp(argv[i], "range") && nr < MAXR) {
            char *c1 = strchr(v, ':'); char *c2 = c1 ? strchr(c1 + 1, ':') : NULL;
            if (!c2) return -1; *c1 = 0;
            snprintf(rname[nr], 32, "%s", v); rlo[nr] = strtoull(c1 + 1, NULL, 16); rhi[nr] = strtoull(c2 + 1, NULL, 16); nr++;
        } else return -1;
    }
    /* Thumb: function symbols have bit0 set; instruction addresses do not. */
    a_start &= ~1ULL; a_stop &= ~1ULL;
    w_total = calloc(MAXW, sizeof *w_total); w_r = calloc(MAXW, sizeof *w_r);
    qemu_plugin_register_vcpu_tb_trans_cb(id, tb_trans);
    qemu_plugin_register_atexit_cb(id, at_exit, NULL);
    return 0;
}
