#include "esinos.h"

Proc procs[MAX_PROCS];
int n_procs = 0;

static const Proc WL_A[] = {
    {"P1", 0, 24, 2},
    {"P2", 0, 3, 2},
    {"P3", 0, 3, 2},
};

static const Proc WL_B[] = {
    {"P1", 0, 6, 2},
    {"P2", 0, 8, 2},
    {"P3", 0, 7, 2},
    {"P4", 0, 3, 2},
};

static const Proc WL_P[] = {
    {"P1", 0, 10, 3},
    {"P2", 0, 1, 1},
    {"P3", 0, 2, 4},
    {"P4", 0, 1, 5},
    {"P5", 0, 5, 2},
};

void load_workloads(int set) {
    const Proc *src;
    int count;

    if (set == 1) { 
        src = WL_B; 
        count = 4; 
    } else if (set == 2) { 
        src = WL_P; 
        count = 5; 
    } else { 
        src = WL_A; 
        count = 3; 
    }

    for (int i = 0; i < count; i++) {
        procs[i] = src[i];
        procs[i].finish_ms = 0;
    }
    
    n_procs = count;
}
