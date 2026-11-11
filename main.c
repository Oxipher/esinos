#include "esinos.h"

int main(void) {
    load_workloads();
    printf("[1] run all schedulers [0] exit\n> ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    
    return 0;
}