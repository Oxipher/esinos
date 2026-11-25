#include <stdio.h>
#include "mem.h"

PageTableEntry mem_table[NUM_PAGES];

void mem_init(void) {
    for (int p = 0; p < NUM_PAGES; p++) {
        mem_table[p].frame = -1;
        mem_table[p].valid = 0;
        mem_table[p].last_access = 0;
    }
}

int mem_translate(int logical_addr, int *phys_addr) {
    int page = logical_addr / PAGE_SIZE;
    int offset = logical_addr % PAGE_SIZE;

    if (logical_addr < 0 || page >= NUM_PAGES) {
        return MEM_BAD_ADDR;
    }
    if (!mem_table[page].valid) {
        return MEM_PAGE_FAULT;
    }
    *phys_addr = mem_table[page].frame * PAGE_SIZE + offset;
    return MEM_OK;
}

int mem_load(int page) {
    if (page < 0 || page >= NUM_PAGES) {
        return -1;
    }
    if (mem_table[page].valid) {
        return mem_table[page].frame;
    }
    for (int f = 0; f < NUM_FRAMES; f++) {
        int taken = 0;
        for (int p = 0; p < NUM_PAGES; p++) {
            if (mem_table[p].valid && mem_table[p].frame == f) {
                taken = 1;
                break;
            }
        }
        if (!taken) {
            mem_table[page].frame = f;
            mem_table[page].valid = 1;
            mem_table[page].last_access = 0;
            return f;
        }
    }
    return -1;
}

void mem_print_table(void) {
    printf("page | cadre | valide | dernier acces\n");
    printf("-----+-------+--------+--------------\n");
    for (int p = 0; p < NUM_PAGES; p++) {
        printf("%4d | %5d | %6d | %13d\n",
               p, mem_table[p].frame, mem_table[p].valid, mem_table[p].last_access);
    }
}

void mem_print_frames(const int *frames, int n_frames) {
    printf("[");
    for (int f = 0; f < n_frames; f++) {
        printf(" %2d", frames[f]);
    }
    printf(" ]");
}

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
