#ifndef MEM_H
#define MEM_H

#define PAGE_SIZE 4096
#define NUM_FRAMES 3
#define MAX_FRAMES 8
#define NUM_PAGES 16
#define TLB_SIZE 3
#define TLB_LOOKUP_NS 20
#define MEM_ACCESS_NS 100
#define MAX_REFS 512

typedef struct {
    int frame;
    int valid;
    int last_access;
} PageTableEntry;

#define MEM_OK 0
#define MEM_PAGE_FAULT 1
#define MEM_BAD_ADDR 2

extern PageTableEntry mem_table[NUM_PAGES];

void mem_init(void);
int mem_translate(int logical_addr, int *phys_addr);
int mem_load(int page);
void mem_print_table(void);
void mem_print_frames(const int *frames, int n_frames);
int mem_translate_demo(void);
int mem_read_refs(const char *path, int *refs, int max_refs);

int mem_run_fifo(const int *refs, int n_refs, int n_frames, int verbose);
int mem_run_lru(const int *refs, int n_refs, int n_frames, int verbose);
int mem_run_optimal(const int *refs, int n_refs, int n_frames, int verbose);

void mem_compare_fifo_lru(const int *refs, int n_refs, int n_frames);
void mem_compare_all(const int *refs, int n_refs, int n_frames);

double mem_eat(double hit_rate);
void mem_tlb_report(const int *refs, int n_refs);

#endif
