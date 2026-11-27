#include <stdio.h>
#include <string.h>
#include "esinos.h"
#include "mem.h"
#include "fs.h"
#include "ui.h"

/* Fallback reference string used when data/references.txt cannot be read */
static const int DEFAULT_REFS[12] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};

/* Pause execution until the user presses Enter (draining remaining input on the line) */
static void wait_step(void) {
    printf("--- Entree pour la suite de la demo ---\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* Load page references from disk trace file, falling back to DEFAULT_REFS if missing/empty */
static int load_refs(int *refs) {
    int n = mem_read_refs("data/references.txt", refs, MAX_REFS);
    if (n < 1) {
        printf("data/references.txt absent ou vide : chaine par defaut\n");
        for (int i = 0; i < 12; i++) { refs[i] = DEFAULT_REFS[i]; }
        n = 12;
    }
    return n;
}

/* Run full non-visual demos across CPU scheduling and memory replacement/TLB modules */
void full_demo(int step) {
    printf("=== DEMO COMPLETE : ordonnancement ===\n");
    load_workloads(0);
    fcfs_run();
    if (step) { wait_step(); }
    load_workloads(1);
    sjf_run();
    if (step) { wait_step(); }
    load_workloads(0);
    rr_run(4);
    if (step) { wait_step(); }
    load_workloads(2);
    prio_run();
    if (step) { wait_step(); }

    printf("=== DEMO COMPLETE : memoire ===\n");
    int refs[MAX_REFS];
    int n = load_refs(refs);

    mem_init();
    mem_run_fifo(refs, n, 3, 1);
    if (step) { wait_step(); }

    int f3 = mem_run_fifo(refs, n, 3, 0);
    int f4 = mem_run_fifo(refs, n, 4, 0);
    printf("anomalie de Belady : FIFO %d defauts a 3 cadres, %d a 4\n", f3, f4);
    if (step) { wait_step(); }
    mem_compare_fifo_lru(refs, n, 4);
    if (step) { wait_step(); }
    mem_compare_all(refs, n, 4);
    if (step) { wait_step(); }
    mem_tlb_report(refs, n);
}

/* ================= DEMO 404 OS PARTAGEES (./demo + ./esinos) ================= */

/* Choose Unicode block/shade glyphs in TTY mode or plain ASCII fallbacks otherwise */
static const char *BLK(void) { return ui_is_tty() ? "█" : "#"; }
static const char *FRG(void) { return ui_is_tty() ? "░" : "."; }

/* ---------------- WEEK 1 : CPU SCHEDULING ---------------- */

static void week1(void) {
    static const char *PAL[4] = { UI_CYAN, UI_YELLOW, UI_MAGENTA, UI_WHITE };
    int idx[MAX_PROCS], start[MAX_PROCS];
    int i, j, end_prev, span, scale, cursor;
    double sumw = 0.0, sumt = 0.0;

    load_workloads(0);
    for (i = 0; i < n_procs; i++) { idx[i] = i; }
    /* FCFS : tri stable par temps d'arrivee */
    for (i = 1; i < n_procs; i++) {
        int t = idx[i];
        for (j = i; j > 0 && procs[idx[j - 1]].arrival_ms > procs[t].arrival_ms; j--) {
            idx[j] = idx[j - 1];
        }
        idx[j] = t;
    }

    /* Compute non-preemptive start and finish times */
    end_prev = 0;
    for (i = 0; i < n_procs; i++) {
        int k = idx[i];
        int a = procs[k].arrival_ms;
        start[k] = end_prev > a ? end_prev : a;
        end_prev = start[k] + procs[k].burst_ms;
        procs[k].finish_ms = end_prev;
    }

    span = end_prev;
    scale = span > 50 ? 2 : 1;

    ui_frame_begin("SCHEDULING - FCFS");
    ui_color(UI_GREEN);
    printf("   GANTT CHART\n");
    ui_reset();
    printf("   ");
    cursor = 0;
    for (i = 0; i < n_procs; i++) {
        int k = idx[i], x;
        int gap = (start[k] - cursor) / scale;
        int w = procs[k].burst_ms / scale;
        if (w < 1) { w = 1; }
        for (x = 0; x < gap; x++) { fputs(FRG(), stdout); }
        ui_color(PAL[i % 4]);
        printf("%s", procs[k].name);
        for (x = 0; x < w; x++) { fputs(BLK(), stdout); }
        ui_reset();
        ui_sleep(80);
        cursor = start[k] + procs[k].burst_ms;
    }
    printf("\n   0%*d ms\n\n", span / scale + 2, span);

    printf("   %-6s %7s %7s %7s %7s\n", "proc", "arrive", "burst", "wait", "turn");
    for (i = 0; i < n_procs; i++) {
        int turn = procs[i].finish_ms - procs[i].arrival_ms;
        int wait = turn - procs[i].burst_ms;
        sumt += turn;
        sumw += wait;
        printf("   %-6s %7d %7d %7d %7d\n", procs[i].name,
               procs[i].arrival_ms, procs[i].burst_ms, wait, turn);
    }
    if (n_procs > 0) {
        printf("   avg waiting = %.2f   avg turnaround = %.2f\n",
               sumw / n_procs, sumt / n_procs);
    }
    ui_frame_end();
}

/* ---------------- WEEK 2 : MEMORY ---------------- */

static void bar(const char *label, const char *color, int faults) {
    int x;
    printf("   %-8s ", label);
    ui_color(color);
    for (x = 0; x < faults; x++) { fputs(BLK(), stdout); ui_sleep(40); }
    ui_reset();
    printf(" %d\n", faults);
}

static void week2(void) {
    int refs[MAX_REFS];
    int n = load_refs(refs);

    ui_frame_begin("MEMORY - PAGE REPLACEMENT");
    printf("   %d references, 3 frames\n", n);
    bar("FIFO", UI_YELLOW,  mem_run_fifo(refs, n, 3, 0));
    bar("LRU",  UI_CYAN,    mem_run_lru(refs, n, 3, 0));
    bar("OPT",  UI_MAGENTA, mem_run_optimal(refs, n, 3, 0));
    printf("   Belady : FIFO %d defauts (3 cadres) -> %d (4 cadres)\n",
           mem_run_fifo(refs, n, 3, 0), mem_run_fifo(refs, n, 4, 0));
    ui_frame_end();
}

/* ---------------- WEEK 3 : FILE SYSTEM ---------------- */

static void week3(void) {
    static const char MSG[] = "demo 404 OS";
    unsigned char bits[(DISK_BLOCKS + 7) / 8];
    int size = 0, blocks[FS_MAX_BLOCKS];
    int b;

    ui_frame_begin("FILE SYSTEM");
    fs_format();
    fs_create("notes");
    fs_write("notes", MSG, (int)strlen(MSG));
    fs_ls();

    if (fs_bitmap_get(bits, (int)sizeof bits) == 0) {
        printf("   DISK MAP (%s = used, %s = free)\n   ", BLK(), FRG());
        for (b = 0; b < DISK_BLOCKS; b++) {
            int used = (bits[b / 8] >> (b % 8)) & 1;
            ui_color(used ? UI_GREEN : UI_DIM);
            fputs(used ? BLK() : FRG(), stdout);
            ui_reset();
            if ((b + 1) % 32 == 0) { printf("\n   "); }
            ui_sleep(10);
        }
        printf("\n");
    }
    if (fs_stat_get("notes", &size, blocks) == 0) {
        printf("   notes : %d octets, blocs", size);
        for (b = 0; b < FS_MAX_BLOCKS && blocks[b] >= 0; b++) { printf(" %d", blocks[b]); }
        printf("\n");
    }
    ui_frame_end();
}

/* Entry point shared by ./demo and ./esinos */
void demo_week_visual(int which) {
    if (which == 1)      { week1(); }
    else if (which == 2) { week2(); }
    else if (which == 3) { week3(); }
}
