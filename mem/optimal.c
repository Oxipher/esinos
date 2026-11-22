#include "esinos.h"
#include "mem.h"
static int next_use(const int *refs, int n_refs, int from, int page) {
 for (int i = from; i < n_refs; i++) {
 if (refs[i] == page) { return i; }
 }
 return n_refs;
}
int mem_run_optimal(const int *refs, int n_refs, int n_frames, int verbose) {
 if (n_frames < 1 || n_frames > MAX_FRAMES) { return -1; }
 int frames[MAX_FRAMES];
 int used = 0;
 int faults = 0;
 for (int f = 0; f < n_frames; f++) { frames[f] = -1; }
 for (int i = 0; i < n_refs; i++) {
 int p = refs[i];
 int pos = -1;
 for (int f = 0; f < used; f++) {
 if (frames[f] == p) { pos = f; break; }
 }
 if (verbose) { printf("%3d ", p); }
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
 used++;
 if (verbose) {
 mem_print_frames(frames, n_frames);
 printf(" FAULT (load)\n");
 }
 } else {
 int victim = 0;
 int best = -1;
 for (int f = 0; f < n_frames; f++) {
 int nu = next_use(refs, n_refs, i + 1, frames[f]);
 if (nu > best) { best = nu; victim = f; }
 }
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
