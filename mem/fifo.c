/fifo.c - FIFO pas a pas (Membre C, jour 10).
 * Evince la page chargee depuis le plus longtemps. Le curseur
 * next_victim tourne en cercle sur les cadres : c'est la meme file
 * circulaire que le RR de la semaine 1. La trace pas a pas (etat des
 * cadres, defaut, page evincee) est le scenario de la soutenance. */
ESINOS - Semaine 2 : Fiche équipe mémoire

#include <stdio.h>
#include "mem.h"
/* FIFO est une file : imprimer les cadres du plus ancien au plus
 * recent, en partant du curseur next_victim, pour que la trace soit
 * la meme que celle de la fiche ([2, 3, 4] apres l'eviction de 1). */
static void print_queue(const int *frames, int n_frames, int head) {
 printf("[");
 int first = 1;
 for (int k = 0; k < n_frames; k++) {
 int page = frames[(head + k) % n_frames];
 if (page < 0) { continue; }
 if (!first) { printf(", "); }
 printf("%d", page);
 first = 0;
 }
 printf("]");
}
int mem_run_fifo(const int *refs, int n_refs, int n_frames, int verbose) {
 if (n_frames < 1 || n_frames > MAX_FRAMES) { return -1; }
 int frames[MAX_FRAMES];
 for (int f = 0; f < n_frames; f++) { frames[f] = -1; }
 int next_victim = 0; /* le plus ancien chargement est toujours ici */
 int faults = 0;
 for (int i = 0; i < n_refs; i++) {
 int p = refs[i];
 int pos = -1;
 for (int f = 0; f < n_frames; f++) {
 if (frames[f] == p) { pos = f; break; }
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
 if (evicted < 0) { printf(" FAULT (load)\n"); }
 else { printf(" FAULT (evict %d - oldest)\n", evicted); }
 }
 }
 return faults;