#include <stdio.h>
#include "mem.h"

static void print_queue(const int *frames, int n_frames, int head) {
    printf("[");
    int first = 1;

    for (int k = 0; k < n_frames; k++) {
        int page = frames[(head + k) % n_frames];

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

int mem_run_fifo(const int *refs, int n_refs, int n_frames, int verbose) {
    if (n_frames < 1 || n_frames > MAX_FRAMES) {
        return -1;
    }

    int frames[MAX_FRAMES];

    for (int f = 0; f < n_frames; f++) {
        frames[f] = -1;
    }

    int next_victim = 0;
    int faults = 0;

    for (int i = 0; i < n_refs; i++) {
        int p = refs[i];
        int pos = -1;

        for (int f = 0; f < n_frames; f++) {
            if (frames[f] == p) {
                pos = f;
                break;
            }
        }

        if (pos >= 0) {
            if (verbose) {
                printf("%3d ", p);
                print_queue(frames, n_frames, next_victim);
                printf(" hit\n");
            }

            continue;
        }

        int evicted = frames[next_victim];
        frames[next_victim] = p;
        next_victim = (next_victim + 1) % n_frames;
        faults++;

        if (verbose) {
            printf("%3d ", p);
            print_queue(frames, n_frames, next_victim);

            if (evicted < 0) {
                printf(" FAULT (load)\n");
            } else {
                printf(" FAULT (evict %d - oldest)\n", evicted);
            }
        }
    }

    return faults;
}
