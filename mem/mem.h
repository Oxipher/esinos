#ifndef MEM_H
#define MEM_H

/* Paging, frame, and reference trace capacities */
#define PAGE_SIZE 4096          /* Bytes per virtual page and physical frame */
#define NUM_FRAMES 3          /* Default number of physical frames */
#define MAX_FRAMES 8          /* Upper bound on frames in simulations */
#define NUM_PAGES 16          /* Total virtual pages in address space */
#define TLB_SIZE 3            /* Number of entries in the translation lookaside buffer */
#define TLB_LOOKUP_NS 20      /* TLB hit lookup latency in nanoseconds */
#define MEM_ACCESS_NS 100     /* Physical memory access latency in nanoseconds */
#define MAX_REFS 512          /* Maximum page references read from a trace */

/* Per-page metadata stored in the page table */
typedef struct {
    int frame;                /* Assigned physical frame index (-1 if unmapped) */
    int valid;                /* 1 if page is currently mapped in memory, 0 otherwise */
    int last_access;          /* Logical timestamp of most recent access (used by LRU) */
} PageTableEntry;

/* Return/status codes for address translation */
#define MEM_OK 0              /* Address translated successfully */
#define MEM_PAGE_FAULT 1      /* Target page is not currently valid/resident */
#define MEM_BAD_ADDR 2        /* Logical address is out of valid range */

/* Global page table defined in the memory module */
extern PageTableEntry mem_table[NUM_PAGES];

/* Initialization, address translation, and display helpers */
void mem_init(void);                                      /* Reset page table entries to initial state */
int mem_translate(int logical_addr, int *phys_addr);      /* Translate logical address to physical address */
int mem_load(int page);                                   /* Load a virtual page into an available/evicted frame */
void mem_print_table(void);                               /* Print current page table mappings */
void mem_print_frames(const int *frames, int n_frames);   /* Print current contents of physical frames */
int mem_translate_demo(void);                             /* Run interactive/sample address translation demo */
int mem_read_refs(const char *path, int *refs, int max_refs); /* Load page reference sequence from file */

/* Page replacement algorithms (return total page faults) */
int mem_run_fifo(const int *refs, int n_refs, int n_frames, int verbose);    /* First-In, First-Out */
int mem_run_lru(const int *refs, int n_refs, int n_frames, int verbose);     /* Least Recently Used */
int mem_run_optimal(const int *refs, int n_refs, int n_frames, int verbose); /* Belady's Optimal (furthest-next-use) */

/* Comparison helpers across frame counts */
void mem_compare_fifo_lru(const int *refs, int n_refs, int n_frames); /* Compare FIFO vs. LRU faults */
void mem_compare_all(const int *refs, int n_refs, int n_frames);      /* Compare FIFO, LRU, and Optimal faults */

/* Effective Access Time (EAT) and TLB performance reporting */
double mem_eat(double hit_rate);                          /* Compute effective access time for a given TLB hit rate */
void mem_tlb_report(const int *refs, int n_refs);         /* Report TLB hits/misses and timing over a trace */

#endif
