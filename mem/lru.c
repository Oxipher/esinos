#include <stdio.h>
#include "mem.h"

/* Simulate LRU page replacement over `refs` and return total page faults */
int mem_run_lru(const int *refs, int n_refs, int n_frames, int verbose) {
    /* Validate requested frame count against capacity bounds */
    if (n_frames < 1 || n_frames > MAX_FRAMES) {
        return -1;
    }

    int frames[MAX_FRAMES]; /* Physical frames holding resident page numbers (-1 if empty) */
    int used = 0;           /* Number of currently occupied frames */
    int faults = 0;         /* Total page fault counter */

    /* Reset page table state before starting the simulation */
    mem_init();

    /* Initialize all physical frame slots to -1 (empty) */
    for (int f = 0; f < n_frames; f++) {
        frames[f] = -1;
    }

    /* Process each page reference in sequence */
    for (int i = 0; i < n_refs; i++) {
        int p = refs[i];

        /* mem_table has only NUM_PAGES entries: reject out-of-range pages */
        if (p < 0 || p >= NUM_PAGES) {
            return -1;
        }

        /* Record logical timestamp (1-based) for this access */
        mem_table[p].last_access = i + 1;

        /* Search active frames to see if page `p` is already resident */
        int pos = -1;
        for (int f = 0; f < used; f++) {
            if (frames[f] == p) {
                pos = f;
                break;
            }
        }

        if (verbose) {
            printf("%3d ", p);
        }

        /* Page hit: `last_access` is already updated; no replacement needed */
        if (pos >= 0) {
            if (verbose) {
                mem_print_frames(frames, n_frames);
                printf(" hit\n");
            }
            continue;
        }

        /* Page miss: increment fault count */
        faults++;

        if (used < n_frames) {
            /* Free frame available: load page `p` into the next open slot */
            frames[used] = p;
            mem_table[p].frame = used;
            used++;

            if (verbose) {
                mem_print_frames(frames, n_frames);
                printf(" FAULT (load)\n");
            }
        } else {
            /* All frames occupied: find the frame with the smallest `last_access` */
            int victim = 0;
            for (int f = 1; f < n_frames; f++) {
                if (mem_table[frames[f]].last_access < mem_table[frames[victim]].last_access) {
                    victim = f;
                }
            }

            /* Evict the least recently used page and install page `p` in its frame */
            int old = frames[victim];
            mem_table[old].frame = -1;
            frames[victim] = p;
            mem_table[p].frame = victim;

            if (verbose) {
                mem_print_frames(frames, n_frames);
                printf(" FAULT (evict %d - least recent)\n", old);
            }
        }
    }

    return faults;
}

/* Print a side-by-side fault count table for FIFO and LRU across frame counts 1..n_frames */
void mem_compare_fifo_lru(const int *refs, int n_refs, int n_frames) {
    printf("cadres | FIFO | LRU\n");
    printf("-------+------+----\n");

    for (int f = 1; f <= n_frames; f++) {
        printf("%6d | %4d | %3d\n", f,
               mem_run_fifo(refs, n_refs, f, 0),
               mem_run_lru(refs, n_refs, f, 0));
    }
}
