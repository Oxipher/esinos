#include <stdio.h>
#include "esinos.h"
#include "mem.h"

static const int DEFAULT_REFS[12] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};

static void wait_step(void) {
    printf("--- Entree pour la suite de la demo ---\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static int load_refs(int *refs) {
    int n = mem_read_refs("data/references.txt", refs, MAX_REFS);
    if (n < 1) {
        printf("data/references.txt absent ou vide : chaine par defaut\n");
        for (int i = 0; i < 12; i++) { refs[i] = DEFAULT_REFS[i]; }
        n = 12;
    }
    return n;
}

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