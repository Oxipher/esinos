#include "esinos.h"
void sjf_run(void) {
/* SJF non preemptif: le plus court burst d'abord. Semaine 1 :
toutes les arrivees valent 0, donc trier par burst suffit. */
for (int i=0; i < n_procs - 1; i++)
for (int j=0; j < n_procs - 1 - i; j++)
if (procs[j].burst_ms > procs[j + 1].burst_ms) {
Proc tmp = procs[j]; procs[j] = procs[j + 1]; procs[j + 1] = tmp;
}
int t=0;
for (int i=0; i < n_procs; i++) {
int start = (t > procs[i].arrival_ms) ? t : procs[i].arrival_ms;
gantt_add(procs[i].name, start, start + procs[i].burst_ms);
t = start + procs[i].burst_ms;
procs[i].finish_ms = t;
}
gantt_finish();
metrics_report(n_procs);
}
