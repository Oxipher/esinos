#include <stdio.h>
#include <string.h>
#include "esinos.h"
#include "mem.h"
#include "fs.h"

static const int DEFAULT_REFS[12] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};

static void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static int read_int(const char *prompt) {
    int v;
    for (;;) {
        printf("%s", prompt);
        if (scanf("%d", &v) == 1) {
            flush_stdin();
            return v;
        }
        if (feof(stdin)) {
            return 0;
        }
        printf("un nombre est attendu, reessayez\n");
        flush_stdin();
        if (feof(stdin)) {
            return 0;
        }
    }
}

static int read_workload(const char *path) {
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        printf("%s: fichier introuvable\n", path);
        n_procs = 0;
        return 0;
    }

    int n = 0;
    while (n < MAX_PROCS &&
           fscanf(f, "%7s %d %d %d",
                  procs[n].name,
                  &procs[n].arrival_ms,
                  &procs[n].burst_ms,
                  &procs[n].priority) == 4) {
        procs[n].finish_ms = 0;
        n++;
    }
    fclose(f);

    if (n == 0) {
        printf("%s: charge vide, rien a ordonnancer\n", path);
    }
    n_procs = n;
    return n;
}

static void run_cm4(void) {
    load_workloads(0); fcfs_run();
    load_workloads(1); sjf_run();
    load_workloads(0); rr_run(4);
    load_workloads(2); prio_run();
}

static void run_custom(const char *path) {
    if (read_workload(path) == 0) { return; }
    fcfs_run();

    if (read_workload(path) == 0) { return; }
    sjf_run();

    if (read_workload(path) == 0) { return; }
    rr_run(4);

    if (read_workload(path) == 0) { return; }
    prio_run();
}

static void run_memory(void) {
    int refs[MAX_REFS];
    int n = mem_read_refs("data/references.txt", refs, MAX_REFS);
    if (n < 1) {
        printf("data/references.txt absent ou vide : chaine par defaut\n");
        for (int i = 0; i < 12; i++) {
            refs[i] = DEFAULT_REFS[i];
        }
        n = 12;
    }

    int c = read_int("[memoire] [1] traduction [2] FIFO pas a pas et tables [3] TLB et EAT\n> ");
    if (c == 1) {
        mem_translate_demo();
    } else if (c == 2) {
        mem_init();
        mem_run_fifo(refs, n, 3, 1);
        mem_compare_fifo_lru(refs, n, 4);
        mem_compare_all(refs, n, 4);
    } else if (c == 3) {
        mem_tlb_report(refs, n);
    } else {
        printf("choix inexistant : retour au menu\n");
    }
}

static void fs_menu(void) {
    for (;;) {
        int c = read_int("[fs] [1] format [2] create [3] write [4] read [5] delete [6] ls [0] retour\n> ");
        if (c == 0) { return; }
        if (c == 1) { fs_format(); continue; }
        if (c == 6) { fs_ls(); continue; }
        if (c < 2 || c > 5) {
            printf("choix inexistant : le sous-menu se re-propose\n");
            continue;
        }

        char name[FS_NAME_MAX + 2];
        char data[FS_MAX_FILE_SIZE + 1];

        printf("nom du fichier: ");
        if (scanf("%11s", name) != 1) {
            flush_stdin();
            continue;
        }
        flush_stdin();

        if (c == 2) {
            printf("fs_create -> %d\n", fs_create(name));
        } else if (c == 3) {
            printf("texte a ecrire (max %d) : ", FS_MAX_FILE_SIZE);
            if (fgets(data, sizeof data, stdin) == NULL) {
                continue;
            }
            data[strcspn(data, "\n")] = '\0';
            printf("fs_write -> %d octets ecrits\n",
                   fs_write(name, data, (int)strlen(data)));
        } else if (c == 4) {
            int got = fs_read(name, data, FS_MAX_FILE_SIZE);
            if (got < 0) {
                printf("fichier absent\n");
            } else {
                data[got] = '\0';
                printf("contenu: %s\n", data);
            }
        } else {
            printf("fs_delete -> %d\n", fs_delete(name));
        }
    }
}

int main(void) {
    for (;;) {
        int c = read_int("[1] ordonnancement CM4 [2] votre charge (data/) [3] memoire\n"
                         "[4] demo complete [5] filesystem [0] quitter\n> ");
        if (c == 0) {
            printf("au revoir\n");
            break;
        } else if (c == 1) {
            run_cm4();
        } else if (c == 2) {
            run_custom("data/workload.txt");
        } else if (c == 3) {
            run_memory();
        } else if (c == 4) {
            int step = read_int("[demo] pas a pas (Entree entre chaque etape) (0/1) ?\n> ");
            full_demo(step == 1);
        } else if (c == 5) {
            fs_menu();
        } else {
            printf("choix inexistant : le menu se re-propose\n");
        }
    }
    return 0;
}
