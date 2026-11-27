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

typedef struct { int p, s, e; } Seg;   /* process index, start, end */

/* Simulate one algorithm (0=FCFS 1=SJF 2=RR 3=PRIO), fill seg[], return count.
 * procs[] is loaded and finish_ms is set as a side effect. */
static int sched_sim(int algo, Seg *seg) {
    static const int WL[4] = { 0, 1, 0, 2 };   /* same workloads as full_demo */
    int ns = 0, t = 0, k;

    load_workloads(WL[algo]);

    if (algo == 2) {                            /* Round Robin, quantum 4 */
        int q[QCAP], head = 0, count = 0, rem[MAX_PROCS], done = 0, next = 0;
        sort_by_arrival();
        for (k = 0; k < n_procs; k++) { rem[k] = procs[k].burst_ms; }
        while (done < n_procs) {
            while (next < n_procs && procs[next].arrival_ms <= t && count < QCAP) {
                q[(head + count++) % QCAP] = next++;
            }
            if (count == 0) {
                if (next < n_procs) { t = procs[next].arrival_ms; continue; }
                break;
            }
            int p = q[head];
            head = (head + 1) % QCAP;
            count--;
            int run = (4 < rem[p]) ? 4 : rem[p];
            if (ns < 64) { seg[ns].p = p; seg[ns].s = t; seg[ns].e = t + run; ns++; }
            t += run;
            rem[p] -= run;
            while (next < n_procs && procs[next].arrival_ms <= t && count < QCAP) {
                q[(head + count++) % QCAP] = next++;
            }
            if (rem[p] > 0) {
                if (count < QCAP) { q[(head + count++) % QCAP] = p; }
            } else {
                procs[p].finish_ms = t;
                done++;
            }
        }
        return ns;
    }

    /* Non-preemptive: pick the best key among arrived processes */
    int fin[MAX_PROCS] = {0};
    for (k = 0; k < n_procs; k++) {
        int best = -1, i;
        for (i = 0; i < n_procs; i++) {
            int key, bkey;
            if (fin[i] || procs[i].arrival_ms > t) { continue; }
            key = (algo == 0) ? procs[i].arrival_ms
                : (algo == 1) ? procs[i].burst_ms : procs[i].priority;
            if (best >= 0) {
                bkey = (algo == 0) ? procs[best].arrival_ms
                     : (algo == 1) ? procs[best].burst_ms : procs[best].priority;
                if (key >= bkey) { continue; }
            }
            best = i;
        }
        if (best < 0) {                         /* CPU idle: jump to next arrival */
            for (i = 0; i < n_procs; i++) {
                if (!fin[i] && (best < 0 || procs[i].arrival_ms < procs[best].arrival_ms)) {
                    best = i;
                }
            }
            t = procs[best].arrival_ms;
        }
        seg[ns].p = best; seg[ns].s = t; seg[ns].e = t + procs[best].burst_ms; ns++;
        t += procs[best].burst_ms;
        procs[best].finish_ms = t;
        fin[best] = 1;
    }
    return ns;
}

/* Draw one framed Gantt chart + metrics table */
static void week1_one(int algo) {
    static const char *PAL[4] = { UI_CYAN, UI_YELLOW, UI_MAGENTA, UI_WHITE };
    static const char *TITLE[4] = {
        "SCHEDULING - FCFS", "SCHEDULING - SJF",
        "SCHEDULING - ROUND ROBIN (q=4)", "SCHEDULING - PRIORITY"
    };
    Seg seg[64];
    int ns = sched_sim(algo, seg);
    int i, x, cursor = 0, span = 0, scale;
    double sumw = 0.0, sumt = 0.0;

    for (i = 0; i < ns; i++) { if (seg[i].e > span) { span = seg[i].e; } }
    scale = span > 50 ? 2 : 1;

    ui_frame_begin(TITLE[algo]);
    ui_color(UI_GREEN);
    printf("   GANTT CHART\n");
    ui_reset();
    printf("   ");
    for (i = 0; i < ns; i++) {
        int gap = (seg[i].s - cursor) / scale;
        int w = (seg[i].e - seg[i].s) / scale;
        if (w < 1) { w = 1; }
        for (x = 0; x < gap; x++) { fputs(FRG(), stdout); }
        ui_color(PAL[seg[i].p % 4]);
        printf("%s", procs[seg[i].p].name);
        for (x = 0; x < w; x++) { fputs(BLK(), stdout); }
        ui_reset();
        ui_sleep(80);
        cursor = seg[i].e;
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

static void week1(void) {
    for (int a = 0; a < 4; a++) { week1_one(a); }
}

/* ---------------- WEEK 2 : MEMORY ---------------- */

static void bar(const char *label, const char *color, int faults) {
    int x;
    printf("   %-8s ", label);
    ui_color(color);
    for (x = 0; x
