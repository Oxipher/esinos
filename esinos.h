#ifndef ESINOS_H
#define ESINOS_H
#define MAX_PROCS 8
#define QCAP 16

typedef struct {
char name[8];
int arrival_ms;
int burst_ms;
int priority;
int finish_ms;
} Proc;


extern Proc procs[MAX_PROCS];
extern int n_procs;

void load_workloads(int set); /* 0 = WL_A, 1 = WL_B, 2 = WL_P */
void gantt_add(const char *n, int s, int e);
void gantt_finish(void);
void metrics_report(int n_procs);
void fcfs_run(void);
void sjf_run(void);
void rr_run(int quantum_ms);
void prio_run(void);
void full_demo(int step);

#endif
