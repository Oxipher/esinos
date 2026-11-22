/* mem/translate.c - la traduction d'adresses (Membre B, jour 9).
 * Traduit une adresse logique (page, decalage) en adresse physique
 * (cadre, decalage) a travers la table de pages, imprime la table,
 * et repere les deux cas : page invalide (hors espace d'adressage)
 * et page valide mais non chargee : c'est le defaut de page. */
#include "esinos.h"
#include "mem.h"
PageTableEntry mem_table[NUM_PAGES];
void mem_init(void) {
 for (int p = 0; p < NUM_PAGES; p++) {
 mem_table[p].frame = -1; /* aucune page chargee au depart */
 mem_table[p].valid = 1; /* pages 0..NUM_PAGES-1 mapppees */
 mem_table[p].last_access = -1; /* jamais accedee */
 }
}
int mem_translate(int logical_addr, int *phys_addr) {
 if (logical_addr < 0) {
 return MEM_BAD_ADDR; /* adresse negative : invalide */
 }
 int page = logical_addr / PAGE_SIZE;
 int offset = logical_addr % PAGE_SIZE;
 if (page >= NUM_PAGES || !mem_table[page].valid) {
 return MEM_BAD_ADDR; /* hors espace d'adressage */
 }
 if (mem_table[page].frame < 0) {
 return MEM_PAGE_FAULT; /* valide mais non chargee */
 }
 *phys_addr = mem_table[page].frame * PAGE_SIZE + offset;
 return MEM_OK;
}
/* Charge une page dans le premier cadre libre. Renvoie le numero de
* cadre, ou -1 si la page est invalide ou si plus aucun cadre n'est
 * libre (dans ce cas, une politique de remplacement doit choisir). */
int mem_load(int page) {
 if (page < 0 || page >= NUM_PAGES || !mem_table[page].valid) {
 return -1;
 }
 if (mem_table[page].frame >= 0) {
 return mem_table[page].frame; /* deja chargee */
 }
 for (int f = 0; f < NUM_FRAMES; f++) {
 int taken = 0;
 for (int p = 0; p < NUM_PAGES; p++) {
 if (mem_table[p].frame == f) { taken = 1; break; }
 }
 if (!taken) {
 mem_table[page].frame = f;
 return f;
 }
 }
 return -1;
}
void mem_print_table(void) {
 printf("page | frame | valid | last_access\n");
 printf("-----+-------+-------+------------\n");
 for (int p = 0; p < NUM_PAGES; p++) {
 printf("%4d | %5d | %5d | %11d\n", p, mem_table[p].frame,
 mem_table[p].valid, mem_table[p].last_access);
 }
}
/* Imprime l'etat des cadres sous la forme [1, 2, 3] : les pages
 * chargees seulement, dans l'ordre des cadres. Partage par les trois
 * politiques pour que leurs traces pas a pas soient comparables. */
void mem_print_frames(const int *frames, int n_frames) {
 printf("[");
 int first = 1;
 for (int f = 0; f < n_frames; f++) {
 if (frames[f] < 0) { continue; }
 if (!first) { printf(", "); }
 printf("%d", frames[f]);
 first = 0;
 }
 printf("]");
}
/* Demo du jour 9 : charge les trois premieres pages puis traduit une
 * douzaine d'adresses. Renvoie le nombre de traductions reussies. */
int mem_translate_demo(void) {
 mem_init();
 for (int p = 0; p < NUM_FRAMES; p++) { mem_load(p); }
 int addrs[12] = {0, 100, 4095, 4096, 8191, 12288,
 5000, -1, 65536, 70000, 9000, 300};
int ok = 0;
for (int i = 0; i < 12; i++) {
 int phys = -1;
 int st = mem_translate(addrs[i], &phys;);
 printf("addr %6d -> ", addrs[i]);
 if (st == MEM_OK) {
 printf("page %d, cadre %d, physique %d\n",
 addrs[i] / PAGE_SIZE, phys / PAGE_SIZE, phys);
 ok++;
 } else if (st == MEM_PAGE_FAULT) {
 printf("DEFAUT DE PAGE (valide, non chargee)\n");
 } else {
 printf("ADRESSE INVALIDE\n");
 }
 }
 return ok;
}
/* Lit data/references.txt (entiers separes par espaces ou retours de
 * ligne). Renvoie le nombre de references lues, ou -1 si le fichier
 * manque : l'appelant retombe alors sur la chaine classique en dur. */
int mem_read_refs(const char *path, int *refs, int max_refs) {
 FILE *f = fopen(path, "r");
 if (f == NULL) { return -1; }
 int n = 0;
 while (n < max_refs && fscanf(f, "%d", &refs;[n]) == 1) { n++; }
 fclose(f);
 return n;
}