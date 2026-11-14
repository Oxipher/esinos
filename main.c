#include <stdio.h>
#include "esinos.h"

int main(void) {
    load_workloads();
    printf("[1] run all schedulers [0] exit\n> ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) {
        fcfs_run();
        sjf_run();
        rr_run(4);
        prio_run();
    }
    return 0;
}
