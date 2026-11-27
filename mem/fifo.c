#include <stdio.h>
#include "mem.h"

/* Print active (non-negative) pages in FIFO order starting from `head` */
static void print_queue(const int *frames, int n_frames, int head) {
    printf("[");
    int first = 1;

    for (int k = 0; k < n_frames; k++) {
        /* Wrap around the circular frame array from the oldest slot */
        int page = frames[(head + k) % n_frames];

        /* Skip empty frame slots (-1) */
        if (page < 0) {
            continue;
        }

        if (!first) {
            printf(", ");
        }

        printf("%d", page);
        first = 0;
    }

    printf("]");
}

/* Simulate FIFO page replacement over `refs` and return total page faults */
int mem_run_fifo(const int *refs, int n_refs, int n_frames, int verbose) {
    /* Validate requested frame count against capacity bounds */
    if (n_frames < 1 || n_frames > MAX_FRAMES) {
        return -1;
    }

    /* Initialize all physical frames to -1 (empty) */
    int frames[MAX_FRAMES];

    for (int f = 0; f < n_frames; f++) {
        frames[f] = -1;
    }

    int next_victim = 0; /* Index of the oldest loaded page (next to evict) */
    int faults = 0;

    /* Process each page reference in sequence */
    for (int i = 0; i < n_refs; i++) {
        int p = refs[i];
        int pos = -1;

        /* Check if page `p` is already resident in a frame */
        for (int f = 0; f < n_frames; f++) {
            if (frames[f] == p) {
                pos = f;
                break;
            }
        }

        /* Page hit: no replacement or fault needed */
        if (pos >= 0) {
            if (verbose) {
                printf("%3d ", p);
                print_queue(frames, n_frames, next_victim);
                printf(" hit\n");
            }

            continue;
        }

        /* Page miss (fault): replace `next_victim` and advance FIFO pointer */
        int evicted = frames[next_victim];
        frames[next_victim] = p;
        next_victim = (next_victim + 1) % n_frames;
        faults++;

        if (verbose) {
            printf("%3d ", p);
            print_queue(frames, n_frames, next_victim);

            if (evicted < 0) {
                /* Slot was empty, so no resident page had to be evicted */
                printf(" FAULT (load)\n");
            } else {
                printf(" FAULT (evict %d - oldest)\n", evicted);
            }
        }
    }

    return faults;
}
