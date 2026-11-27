#include "esinos.h"

void rr_run(int quantum_ms) {
    int q[QCAP];
    int head = 0, count = 0;
    int remaining[MAX_PROCS];
    int t = 0, done = 0, next = 0;

    sort_by_arrival();

    for (int i = 0; i < n_procs; i++)
        remaining[i] = procs[i].burst_ms;

    while (done < n_procs) {
        /* enqueue every process that has arrived by time t */
        while (next < n_procs && procs[next].arrival_ms <= t) {
            if (count < QCAP) {
                q[(head + count++) % QCAP] = next++;
            }
        }

        if (count == 0) {
            if (next < n_procs) {
                t = procs[next].arrival_ms;
                continue;
            } else {
                break;
            }
        }

        int p = q[head];
        head = (head + 1) % QCAP;
        count--;

        int run = (quantum_ms < remaining[p]) ? quantum_ms : remaining[p];
        gantt_add(procs[p].name, t, t + run);
        t += run;
        remaining[p] -= run;

        /* processes that arrived during this slice go before the preempted one */
        while (next < n_procs && procs[next].arrival_ms <= t) {
            if (count < QCAP) {
                q[(head + count++) % QCAP] = next++;
            }
        }

        if (remaining[p] > 0) {
            /* quantum expired: re-queue at the back */
            if (count < QCAP) {
                q[(head + count++) % QCAP] = p;
            }
        } else {
            procs[p].finish_ms = t;   /* required by metrics_report */
            done++;
        }
    }

    gantt_finish();
    metrics_report(n_procs);
}
