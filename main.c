#include <stdio.h>
#include "esinos.h"

int main(void) {
    printf("[1] run all schedulers [0] exit\n> ");
    int choice;
    
    if (scanf("%d", &choice) != 1) return 0;
    
    if (choice == 1) {
        load_workloads(0);
        fcfs_run();     
        
        load_workloads(1);
        sjf_run();     
        
        load_workloads(0);
        rr_run(4);    
        
        load_workloads(2);
        prio_run();    
    }
    
    return 0;
}
