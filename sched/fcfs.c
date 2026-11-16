#include "esinos.h"

void fcfs_run(void) {
    int t = 0;
    for (int i = 0; i < n_procs; i++) {            /* table is in arrival order */
        Proc *p = &procs[i];
        int start = (t > p->arrival_ms) ? t : p->arrival_ms;
        gantt_add(p->name, start, start + p->burst_ms);
        t = start + p->burst_ms;
    }
    gantt_finish();
    metrics_report(n_procs);
}