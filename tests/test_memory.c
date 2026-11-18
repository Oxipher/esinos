/* tests/test_memory.c - les assertions de la semaine 2 (jour 14).
 * Chaque compte de defauts de la fiche devient une assertion : un
 * nombre qui derive casse le build, pas la veille de la soutenance.
 * Succes complet : imprime WEEK 2 MEMORY GREEN et sort avec 0. */
#include <stdio.h>
#include "mem.h"

/* La chaine de reference classique du Silberschatz, celle de la
 * fiche, utilisee si data/references.txt n'existe pas. */
static const int DEFAULT_REFS[12] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};

/* Arrondi au ns le plus proche : le calcul flottant de l'EAT peut
 * tomber a 139.999999 ou 140.000001 selon l'ordre des operations. */
static int eat_ns(double hit_rate) {
    return (int)(mem_eat(hit_rate) + 0.5);
}

int main(void) {
    int refs[MAX_REFS];
    int n = mem_read_refs("data/references.txt", refs, MAX_REFS);
    if (n < 1) {
        for (int i = 0; i < 12; i++) { refs[i] = DEFAULT_REFS[i]; }
        n = 12;
    }

    int fails = 0;

    /* FIFO : 9 defauts a 3 cadres, 10 a 4 (anomalie de Belady). */
    if (mem_run_fifo(refs, n, 3, 0) != 9) {
        printf("KO fifo: attendu 9 defauts a 3 cadres\n"); fails++;
    }
    if (mem_run_fifo(refs, n, 4, 0) != 10) {
        printf("KO fifo: attendu 10 defauts a 4 cadres (Belady)\n"); fails++;
    }

    /* LRU : 10 defauts a 3 cadres, 8 a 4. */
    if (mem_run_lru(refs, n, 3, 0) != 10) {
        printf("KO lru: attendu 10 defauts a 3 cadres\n"); fails++;
    }
    if (mem_run_lru(refs, n, 4, 0) != 8) {
        printf("KO lru: attendu 8 defauts a 4 cadres\n"); fails++;
    }

    /* Optimal : 7 defauts a 3 cadres, 6 a 4, jamais pire que les autres. */
    if (mem_run_optimal(refs, n, 3, 0) != 7) {
        printf("KO optimal: attendu 7 defauts a 3 cadres\n"); fails++;
    }
    if (mem_run_optimal(refs, n, 4, 0) != 6) {
        printf("KO optimal: attendu 6 defauts a 4 cadres\n"); fails++;
    }

    /* EAT : sans TLB 200 ns ; h = 80% -> 140, 90% -> 130, 100% -> 120. */
    if (eat_ns(0.80) != 140 || eat_ns(0.90) != 130 || eat_ns(1.0) != 120) {
        printf("KO eat: attendu 140 / 130 / 120 ns\n"); fails++;
    }

    /* Traduction : une adresse valide, un defaut, une adresse invalide. */
    mem_init();
    mem_load(0);
    int phys = -1;
    if (mem_translate(300, &phys) != MEM_OK || phys != 300) {
        printf("KO translate: 300 doit valoir physique 300\n"); fails++;
    }
    if (mem_translate(12288, &phys) != MEM_PAGE_FAULT) {
        printf("KO translate: 12288 doit produire un defaut de page\n"); fails++;
    }
    if (mem_translate(65536, &phys) != MEM_BAD_ADDR) {
        printf("KO translate: 65536 est hors espace d'adressage\n"); fails++;
    }

    if (fails == 0) {
        printf("WEEK 2 MEMORY GREEN\n");
        return 0;
    }
    printf("WEEK 2 MEMORY RED: %d assertion(s) en echec\n", fails);
    return 1;
}
