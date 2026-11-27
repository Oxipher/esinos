#ifndef ESINOS_H
#define ESINOS_H

/* Scheduling capacity and Gantt timeline buffer limits */
#define MAX_PROCS 8   /* Maximum number of processes in a workload */
#define QCAP 16       /* Maximum entries/capacity in scheduling/Gantt queues */

/* Process control block (PCB) entry for CPU scheduling simulations */
typedef struct {
    char name[8];     /* Short process label (up to 7 chars + '\0') */
    int arrival_ms;   /* Arrival time in milliseconds */
    int burst_ms;     /* Total CPU burst duration in milliseconds */
    int priority;     /* Scheduling priority value */
    int finish_ms;    /* Completion timestamp in milliseconds */
} Proc;

/* Active process table and count shared across scheduling and demo modules */
extern Proc procs[MAX_PROCS];
extern int n_procs;

/* Workload loading, Gantt rendering, and scheduling metrics */
void load_workloads(int set); /* 0 = WL_A, 1 = WL_B, 2 = WL_P */
void sort_by_arrival(void);   /* Stable sort of procs[] by arrival_ms */
void gantt_add(const char *n, int s, int e); /* Record a Gantt segment [s, e) for process `n` */
void gantt_finish(void);      /* Finalize and print the accumulated Gantt chart */
void metrics_report(int n_procs); /* Compute and display waiting and turnaround metrics */

/* CPU scheduling algorithms */
void fcfs_run(void);          /* First-Come, First-Served scheduler */
void sjf_run(void);           /* Shortest Job First scheduler */
void rr_run(int quantum_ms);  /* Round-Robin scheduler with time slice `quantum_ms` */
void prio_run(void);          /* Priority-based scheduler */

/* Full and visual demo entry points */
void full_demo(int step);     /* Run full scheduling and memory demonstrations */
void demo_week_visual(int which); /* 1 = sched, 2 = memoire, 3 = fs : contenu 404 OS partage par ./demo et ./esinos */

#endif
