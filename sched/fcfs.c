#include "esinos.h"
void fcfs_run(void) {
    int t = 0;
    for (int i = 0; i < n_procs; i++) { 
        Proc *p = &procs;[i];
        int start = (t > p->arrival_ms) ? t : p->arrival_ms;
        gantt_add(p->name, start, start + p->burst_ms);
        t = start + p->burst_ms;
        p->finish_ms = t; /* ajout : pour metrics */
    }
    gantt_finish();
    metrics_report(n_procs);
}
