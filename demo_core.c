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
    /* Run each scheduling algorithm with its corresponding workload preset */
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

    /* Run page replacement comparisons, Belady check, and TLB effective access time report */
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

/* Render a visual Gantt chart and waiting/turnaround metrics table for FCFS workload 0 */
static void week1(void) {
    static const char *PAL[4] = { UI_CYAN, UI_YELLOW, UI_MAGENTA, UI_WHITE };
    int idx[MAX_PROCS], start[MAX_PROCS];
    int i, j, end_prev, span, scale;
    double sumw = 0.0, sumt = 0.0;

    load_workloads(0);
    for (i = 0; i < n_procs; i++) {
        idx[i] = i;
    }
    /* FCFS : tri stable par temps d'arrivee */
    for (i = 1; i < n_procs; i++) {
        int t = idx[i];
        for (j = i; j > 0 && procs[idx[j - 1]].arrival_ms > procs[t].arrival_ms; j--) {
            idx[j] = idx[j - 1];
        }
        idx[j] = t;
    }

    /* Compute non-preemptive start times for each scheduled process */
    end_prev = 0;
    for (i = 0; i < n_procs; i++) {
        int a = procs[idx[i]].arrival_ms;
        start[idx[i]] = end_prev > a ? end_prev : a;
        end_prev = start[idx[i]] + procs[idx[i]].burst_ms;
    }

    /* Scale wide timelines down so the Gantt bar stays compact */
    span = end_prev;
    scale = span > 50 ? 2 : 1;

    /* Draw colored Gantt bar segments with subtle step delay */
    ui_color(UI_GREEN);
    printf("   GANTT CHART\n");
}
