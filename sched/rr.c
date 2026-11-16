#include "esinos.h"

#define QCAP MAX_PROCS   /* one slot per process is enough: a process
                            is never twice in the queue at the same time */

void rr_run(int quantum_ms) {
    int q[QCAP];
    int head = 0, count = 0;
    int remaining[MAX_PROCS];
    int t = 0, done = 0, next = 0;   /* 'next' walks the arrival-ordered table */

    for (int i = 0; i < n_procs; i++)
        remaining[i] = procs[i].burst_ms;

    while (done < n_procs) {
        /* FIRST: enqueue every process arrived by time t */
        while (next < n_procs && procs[next].arrival_ms <= t)
            q[(head + count++) % QCAP] = next++;

        if (count == 0) {                /* CPU idle: jump to next arrival */
            t = procs[next].arrival_ms;
            continue;
        }

        int p = q[head]; head = (head + 1) % QCAP; count--;   /* dequeue */
        int run = (quantum_ms < remaining[p]) ? quantum_ms : remaining[p];
        gantt_add(procs[p].name, t, t + run);
        t += run;
        remaining[p] -= run;

        /* THEN: requeue the current process if not finished */
        if (remaining[p] > 0) q[(head + count++) % QCAP] = p;
        else done++;
    }
    gantt_finish();
    metrics_report(n_procs);
}