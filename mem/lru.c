#include <stdio.h>
#include "mem.h"

int mem_run_lru(const int *refs, int n_refs, int n_frames, int verbose) {
    if (n_frames < 1 || n_frames > MAX_FRAMES) {
        return -1;
    }

    int frames[MAX_FRAMES];
    int used = 0;
    int faults = 0;

    mem_init();

    for (int f = 0; f < n_frames; f++) {
        frames[f] = -1;
    }

    for (int i = 0; i < n_refs; i++) {
        int p = refs[i];
        mem_table[p].last_access = i + 1;

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

        if (pos >= 0) {
            if (verbose) {
                mem_print_frames(frames, n_frames);
                printf(" hit\n");
            }
            continue;
        }

        faults++;

        if (used < n_frames) {
            frames[used] = p;
            mem_table[p].frame = used;
            used++;

            if (verbose) {
                mem_print_frames(frames, n_frames);
                printf(" FAULT (load)\n");
            }
        } else {
            int victim = 0;
            for (int f = 1; f < n_frames; f++) {
                if (mem_table[frames[f]].last_access < mem_table[frames[victim]].last_access) {
                    victim = f;
                }
            }

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

void mem_compare_fifo_lru(const int *refs, int n_refs, int n_frames) {
    printf("cadres | FIFO | LRU\n");
    printf("-------+------+----\n");

    for (int f = 1; f <= n_frames; f++) {
        printf("%6d | %4d | %3d\n", f,
               mem_run_fifo(refs, n_refs, f, 0),
               mem_run_lru(refs, n_refs, f, 0));
    }
}