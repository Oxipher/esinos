#include <stdio.h>
#include "mem.h"

/* Find the earliest index >= `from` where `page` appears in `refs`, or `n_refs` if never used again */
static int next_use(const int *refs, int n_refs, int from, int page) {
    for (int i = from; i < n_refs; i++) {
        if (refs[i] == page) {
            return i;
        }
    }
    return n_refs;
}

/* Simulate Belady's Optimal page replacement over `refs` and return total page faults */
int mem_run_optimal(const int *refs, int n_refs, int n_frames, int verbose) {
    /* Validate requested frame count against capacity bounds */
    if (n_frames < 1 || n_frames > MAX_FRAMES) {
        return -1;
    }

    int frames[MAX_FRAMES]; /* Physical frames holding resident page numbers (-1 if empty) */
    int used = 0;           /* Number of currently occupied frames */
    int faults = 0;         /* Total page fault counter */

    /* Initialize all physical frame slots to -1 (empty) */
    for (int f = 0; f < n_frames; f++) {
        frames[f] = -1;
    }

    /* Process each page reference in sequence */
    for (int i = 0; i < n_refs; i++) {
        int p = refs[i];

        /* Check if page `p` is already resident in one of the active frames */
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

        /* Page hit: page is already in memory; no replacement needed */
        if (pos >= 0) {
            if (verbose) {
                mem_print_frames(frames, n_frames);
                printf(" hit\n");
            }
            continue;
        }

        /* Page miss: increment fault counter */
        faults++;

        if (used < n_frames) {
            /* Empty frame available: load page `p` into the next free slot */
            frames[used] = p;
            used++;

            if (verbose) {
                mem_print_frames(frames, n_frames);
                printf(" FAULT (load)\n");
            }
        } else {
            /* All frames full: evict the page whose next reference is furthest in the future */
            int victim = 0;
            int best = -1; /* Largest next-use index found so far (`n_refs` means never used again) */

            for (int f = 0; f < n_frames; f++) {
                int nu = next_use(refs, n_refs, i + 1, frames[f]);
                if (nu > best) {
                    best = nu;
                    victim = f;
                }
            }

            /* Replace the chosen victim page with the newly referenced page `p` */
            int old = frames[victim];
            frames[victim] = p;

            if (verbose) {
                mem_print_frames(frames, n_frames);
                printf(" FAULT (evict %d - farthest next use)\n", old);
            }
        }
    }

    return faults;
}

/* Print a side-by-side fault table comparing FIFO, LRU, and Optimal for frame counts 1..n_frames */
void mem_compare_all(const int *refs, int n_refs, int n_frames) {
    printf("cadres | FIFO | LRU | Optimal\n");
    printf("-------+------+-----+--------\n");

    for (int f = 1; f <= n_frames; f++) {
        printf("%6d | %4d | %3d | %7d\n", f,
               mem_run_fifo(refs, n_refs, f, 0),
               mem_run_lru(refs, n_refs, f, 0),
               mem_run_optimal(refs, n_refs, f, 0));
    }
}
