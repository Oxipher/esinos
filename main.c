#include <stdio.h>
#include "esinos.h"
int main(void) {
printf("[1] run all schedulers [0] exit\n> ");
int choice;
if (scanf("%d", &choice) != 1) return 0;
if (choice == 1) {
load_workloads (0);
fcfs_run(); /* WL_A -> FCFS 17.00 */
load_workloads (1);
sjf_run(); /* WL_B -> SJF 7.00 */
load_workloads (0);
rr_run(4); /* WL_A -> RR q=4 5.67 */
load_workloads (2);
prio_run(); /* WL_P -> prio 8.20 */
}
return 0;
}
