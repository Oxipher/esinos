#include <stdio.h>
#include "esinos.h"

void metrics_report(int n) {
    if (n <= 0) return;

    double sum_wait = 0, sum_turn = 0;

    for (int i = 0; i < n; i++) {
        int turnaround = procs[i].finish_ms - procs[i].arrival_ms;
        int waiting = turnaround - procs[i].burst_ms;

        sum_turn += turnaround;
        sum_wait += waiting;
    }

    printf("avg waiting time = %.2f\n", sum_wait / n);
    printf("avg turnaround time = %.2f\n", sum_turn / n);
}
