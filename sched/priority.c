#include "esinos.h"

/* Non-preemptive priority scheduling: lower number = higher priority */
void prio_run(void) {
    int done[MAX_PROCS] = {0};
    int t = 0;

    for (int k = 0; k < n_procs; k++) {
        int best = -1;

        for (int i = 0; i < n_procs; i++) {
            if (!done[i] && procs[i].arrival_ms <= t &&
                (best < 0 || procs[i].priority < procs[best].priority)) {
                best = i;
            }
        }

        /* CPU idle: jump to the earliest next arrival */
        if (best < 0) {
            for (int i = 0; i < n_procs; i++) {
                if (!done[i] && (best < 0 || procs[i].arrival_ms < procs[best].arrival_ms)) {
                    best = i;
                }
            }
            t = procs[best].arrival_ms;
        }

        gantt_add(procs[best].name, t, t + procs[best].burst_ms);
        t += procs[best].burst_ms;
        procs[best].finish_ms = t;
        done[best] = 1;
    }

    gantt_finish();
    metrics_report(n_procs);
}
