#ifndef ESINOS_H
#define ESINOS_H

#define MAX_PROCS 8

typedef struct {
    char name[8];
    int arrival_ms;
    int brust_ms;
    int priority;
} Proc;

void load_workloads(void);
void gantt_add(const char *n, int s, int e);
void gantt_finish(void);
void metrics_report(int n_procs);

void fcfs_run(void);
void sjf_run(void);
void rr_run(int quantum_ms);
void prio_run(void);


#endif
