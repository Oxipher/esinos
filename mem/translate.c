#include <stdio.h>
#include "mem.h"

/* Global page table mapping virtual pages to physical frames */
PageTableEntry mem_table[NUM_PAGES];

/* Mark all page table entries as invalid and unmapped */
void mem_init(void) {
    for (int p = 0; p < NUM_PAGES; p++) {
        mem_table[p].frame = -1;
        mem_table[p].valid = 0;
        mem_table[p].last_access = 0;
    }
}

/* Translate a logical address into a physical address via the page table */
int mem_translate(int logical_addr, int *phys_addr) {
    /* Split logical address into page number and byte offset within the page */
    int page = logical_addr / PAGE_SIZE;
    int offset = logical_addr % PAGE_SIZE;

    /* Reject negative or out-of-range logical addresses */
    if (logical_addr < 0 || page >= NUM_PAGES) {
        return MEM_BAD_ADDR;
    }
    /* Signal a page fault if the target page is not currently mapped */
    if (!mem_table[page].valid) {
        return MEM_PAGE_FAULT;
    }
    /* Combine mapped physical frame base and page offset */
    *phys_addr = mem_table[page].frame * PAGE_SIZE + offset;
    return MEM_OK;
}

/* Load a virtual page into the first available physical frame */
int mem_load(int page) {
    if (page < 0 || page >= NUM_PAGES) {
        return -1;
    }
    /* Already resident: return its existing frame index */
    if (mem_table[page].valid) {
        return mem_table[page].frame;
    }
    /* Scan physical frames for one not currently claimed by any valid page */
    for (int f = 0; f < NUM_FRAMES; f++) {
        int taken = 0;
        for (int p = 0; p < NUM_PAGES; p++) {
            if (mem_table[p].valid && mem_table[p].frame == f) {
                taken = 1;
                break;
            }
        }
        /* Claim free frame `f` for `page` */
        if (!taken) {
            mem_table[page].frame = f;
            mem_table[page].valid = 1;
            mem_table[page].last_access = 0;
            return f;
        }
    }
    /* No free frame available */
    return -1;
}

/* Print current contents of the page table */
void mem_print_table(void) {
    printf("page | cadre | valide | dernier acces\n");
    printf("-----+-------+--------+--------------\n");
    for (int p = 0; p < NUM_PAGES; p++) {
        printf("%4d | %5d | %6d | %13d\n",
               p, mem_table[p].frame, mem_table[p].valid, mem_table[p].last_access);
    }
}

/* Print current values stored in `frames[0..n_frames - 1]` */
void mem_print_frames(const int *frames, int n_frames) {
    printf("[");
    for (int f = 0; f < n_frames; f++) {
        printf(" %2d", frames[f]);
    }
    printf(" ]");
}

/* Read up to `max_refs` integer page numbers from file at `path` */
int mem_read_refs(const char *path, int *refs, int max_refs) {
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        return 0;
    }
    int n = 0;
    while (n < max_refs && fscanf(f, "%d", &refs[n]) == 1) {
        n++;
    }
    fclose(f);
    return n;
}

/* Interactive loop demonstrating page loading and logical-to-physical translation */
int mem_translate_demo(void) {
    mem_init();
    mem_load(0);
    mem_load(1);
    mem_print_table();
    for (;;) {
        int addr;
        printf("adresse logique (negative pour quitter) : ");
        if (scanf("%d", &addr) != 1) {
            break;
        }
        if (addr < 0) {
            break;
        }
        int phys = -1;
        int rc = mem_translate(addr, &phys);
        if (rc == MEM_OK) {
            printf("page %d, offset %d, cadre %d -> physique %d\n",
                   addr / PAGE_SIZE, addr % PAGE_SIZE,
                   mem_table[addr / PAGE_SIZE].frame, phys);
        } else if (rc == MEM_PAGE_FAULT) {
            printf("page %d : defaut de page (page non chargee)\n", addr / PAGE_SIZE);
        } else {
            printf("adresse hors espace d'adressage : %d pages x %d octets\n",
                   NUM_PAGES, PAGE_SIZE);
        }
    }
    return 0;
}
