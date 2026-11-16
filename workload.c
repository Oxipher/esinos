#include "esinos.h"
/* CM4 example A: FCFS 17.00 | reordered P2,P3,P1 3.00 | RR q=4 5.67 */
static const Proc WL_A[] = {
 {"P1", 0, 24, 2}, {"P2", 0, 3, 2}, {"P3", 0, 3, 2},
};
/* CM4 example B: SJF 7.00 vs FCFS 10.25 on the same table */
static const Proc WL_B[] = {
 {"P1", 0, 6, 2}, {"P2", 0, 8, 2}, {"P3", 0, 7, 2}, {"P4", 0, 3, 2},
};
/* CM4 priority example: 8.20 average waiting */
static const Proc WL_P[] = {
 {"P1", 0, 10, 3}, {"P2", 0, 1, 1}, {"P3", 0, 2, 4},
 {"P4", 0, 1, 5}, {"P5", 0, 5, 2},
};
void load_workloads(void) { /* copy tables into the ready queue */ }

