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

