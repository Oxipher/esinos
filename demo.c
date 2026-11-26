#include <stdio.h>
#include <string.h>
#include "esinos.h"
#include "mem.h"
#include "fs.h"
#include "ui.h"

/* 404 OS desktop : ./demo ouvre le bureau interactif sur un terminal.
 * Sans terminal (make test, pipes) : les 3 semaines defilent en ASCII
 * pur, sans attente -> les tests restent automatiques. */

static const char *BLK(void) { return ui_is_tty() ? "█" : "#"; }
static const char *FRG(void) { return ui_is_tty() ? "░" : "."; }

/* ---------------- WEEK 1 : CPU SCHEDULING ---------------- */

static void week1(void) {
    static const char *PAL[4] = { UI_CYAN, UI_YELLOW, UI_MAGENTA, UI_WHITE };
    int idx[MAX_PROCS], start[MAX_PROCS];
    int i, j, end_prev, span, scale;
    double sumw = 0.0, sumt = 0.0;

    load_workloads(0);
    for (i = 0; i < n_procs; i++) {
        idx[i] = i;
    }
    /* FCFS : tri stable par temps d'arrivee */
    for (i = 1; i < n_procs; i++) {
        int t = idx[i];
        for (j = i; j > 0 && procs[idx[j - 1]].arrival_ms > procs[t].arrival_ms; j--) {
            idx[j] = idx[j - 1];
        }
        idx[j] = t;
    }
    end_prev = 0;
    for (i = 0; i < n_procs; i++) {
        int a = procs[idx[i]].arrival_ms;
        start[idx[i]] = end_prev > a ? end_prev : a;
        end_prev = start[idx[i]] + procs[idx[i]].burst_ms;
    }
    span = end_prev;
    scale = span > 50 ? 2 : 1;

    ui_color(UI_GREEN);
    printf("   GANTT CHART\n");
    ui_reset();
    printf("   ");
    for (i = 0; i < n_procs; i++) {
        int s = start[idx[i]] / scale;
        int e = (start[idx[i]] + procs[idx[i]].burst_ms) / scale;
        ui_color(PAL[i % 4]);
        for (j = s; j < e; j++) {
            fputs(BLK(), stdout);
            ui_sleep(12);
        }
        ui_reset();
    }
    printf("\n   ");
    for (i = 0; i < n_procs; i++) {
        int w = procs[idx[i]].burst_ms / scale;
        int len = (int)strlen(procs[idx[i]].name);
        int left = (w - len) / 2;
        int right;
        if (left < 0) { left = 0; }
        right = w - len - left;
        if (right < 0) { right = 0; }
        printf("%*s%s%*s", left, "", procs[idx[i]].name, right, "");
    }
    printf("\n   ");
    {
        char tick[160];
        int pos = 0;
        int blen = span / scale + 8;
        memset(tick, ' ', sizeof tick);
        for (i = 0; i <= n_procs; i++) {
            int b = (i < n_procs ? start[idx[i]] : span) / scale;
            char num[8];
            int l = snprintf(num, sizeof num, "%d", b);
            if (b >= pos && b + l < blen) {
                memcpy(tick + b, num, l);
                pos = b + l;
            }
        }
        tick[blen] = '\0';
        printf("%s\n", tick);
    }

    ui_color(UI_GREEN);
    printf("\n   PROCESS TABLE\n");
    ui_reset();
    printf("   PROCESS   ARRIVAL   BURST   WAITING   TURNAROUND\n");
    for (i = 0; i < n_procs; i++) {
        int w = start[i] - procs[i].arrival_ms;
        int t = start[i] + procs[i].burst_ms - procs[i].arrival_ms;
        sumw += w;
        sumt += t;
        printf("   %-8s  %-7d  %-5d   %-8d  %d\n",
               procs[i].name, procs[i].arrival_ms, procs[i].burst_ms, w, t);
    }
    printf("   -----------------------------------------------\n");
    printf("   AVG WAITING = %.2f          AVG TURNAROUND = %.2f\n",
           sumw / n_procs, sumt / n_procs);
    ui_color(UI_DIM);
    printf("   (compare of the 4 algorithms : run ./esinos)\n");
    ui_reset();
}

/* ---------------- WEEK 2 : MEMORY PAGING ---------------- */

static void bar(const char *label, int value, int max, int winner) {
    const int W = 22;
    int fill = value * W / max;
    int i;

    printf("   %-6s", label);
    ui_color(winner ? UI_GREEN : UI_WHITE);
    for (i = 0; i < fill; i++) {
        fputs(BLK(), stdout);
        ui_sleep(8);
    }
    ui_color(UI_DIM);
    for (i = fill; i < W; i++) {
        fputs(FRG(), stdout);
    }
    ui_reset();
    printf("  %2d faults  hit %5.1f%%", value, (max - value) * 100.0 / max);
    if (winner) {
        ui_color(UI_GREEN);
        printf("  <-- WINNER");
        ui_reset();
    }
    printf("\n");
}

static void week2(void) {
    static const int BELADY[12] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};
    int refs[MAX_REFS];
    int n, F = 3, fifo, lru, opt, best, i;
    const char *wname;

    n = mem_read_refs("data/references.txt", refs, MAX_REFS);
    if (n < 1) {
        printf("   (data/references.txt absent : Belady string)\n");
        for (n = 0; n < 12; n++) {
            refs[n] = BELADY[n];
        }
        n = 12;
    }
    fifo = mem_run_fifo(refs, n, F, 0);
    lru = mem_run_lru(refs, n, F, 0);
    opt = mem_run_optimal(refs, n, F, 0);
    best = fifo;
    if (lru < best) { best = lru; }
    if (opt < best) { best = opt; }

    printf("   reference string : ");
    for (i = 0; i < n; i++) {
        printf("%d ", refs[i]);
    }
    printf("\n   frames : %d\n\n", F);

    ui_color(UI_GREEN);
    printf("   PAGE FAULTS\n");
    ui_reset();
    bar("FIFO", fifo, n, fifo == best);
    ui_sleep(140);
    bar("LRU", lru, n, lru == best);
    ui_sleep(140);
    bar("OPT", opt, n, opt == best);
    printf("\n");

    wname = (best == opt) ? "OPT" : (best == lru ? "LRU" : "FIFO");
    ui_color(UI_GREEN);
    printf("   RESULT : %s keeps the most pages in memory.\n", wname);
    ui_reset();
}

/* ---------------- WEEK 3 : FILE SYSTEM ---------------- */

static void join_blocks(char *out, size_t cap, const int *blocks) {
    size_t used = 0;
    int b, first = 1;
    out[0] = '\0';
    for (b = 0; b < FS_MAX_BLOCKS; b++) {
        if (blocks[b] < 0) {
            continue;
        }
        used += (size_t)snprintf(out + used, cap - used,
                                 first ? "%d" : ",%d", blocks[b]);
        first = 0;
    }
}

static void show_file(const char *name) {
    int size, blocks[FS_MAX_BLOCKS];
    char list[64];
    if (fs_stat_get(name, &size, blocks) != 0) {
        return;
    }
    join_blocks(list, sizeof list, blocks);
    printf("   %-10s  %4d B    %s\n", name, size, list);
}

static void week3(void) {
    char buf[256], list[64], back[FS_MAX_FILE_SIZE + 1];
    const char *msgA = "hello 404 OS";
    const char *msgB = "Ceci est le fichier B de la demo 404 OS!";
    unsigned char bits[(DISK_BLOCKS + 7) / 8];
    int sA, bA[FS_MAX_BLOCKS], sB, bB[FS_MAX_BLOCKS];
    int row, i, got, used = 0;

    fs_format();
    ui_sleep(250);
    ui_ok("format : 64 blocks created");

    fs_create("a.txt");
    fs_write("a.txt", msgA, (int)strlen(msgA));
    fs_stat_get("a.txt", &sA, bA);
    join_blocks(list, sizeof list, bA);
    snprintf(buf, sizeof buf, "write  : a.txt -> %d bytes -> block %s", sA, list);
    ui_ok(buf);
    ui_sleep(200);

    fs_create("b.txt");
    fs_write("b.txt", msgB, (int)strlen(msgB));
    fs_stat_get("b.txt", &sB, bB);
    join_blocks(list, sizeof list, bB);
    snprintf(buf, sizeof buf, "write  : b.txt -> %d bytes -> block %s", sB, list);
    ui_ok(buf);
    ui_sleep(200);

    got = fs_read("a.txt", back, FS_MAX_FILE_SIZE);
    if (got >= 0) {
        back[got] = '\0';
    } else {
        back[0] = '\0';
    }
    snprintf(buf, sizeof buf, "read   : a.txt -> \"%s\"", back);
    ui_ok(buf);
    ui_sleep(250);

    ui_color(UI_GREEN);
    printf("\n   FILES\n");
    ui_reset();
    printf("   NAME       SIZE      BLOCKS\n");
    show_file("a.txt");
    show_file("b.txt");

    ui_color(UI_GREEN);
    printf("\n   DISK MAP\n");
    ui_reset();
    if (fs_bitmap_get(bits, sizeof bits) != 0) {
        memset(bits, 0, sizeof bits);
    }
    for (i = 0; i < DISK_BLOCKS; i++) {
        if (bits[i / 8] & (unsigned char)(1 << (i % 8))) {
            used++;
        }
    }
    for (row = 0; row < DISK_BLOCKS / 16; row++) {
        printf("   ");
        for (i = row * 16; i < row * 16 + 16; i++) {
            if (bits[i / 8] & (unsigned char)(1 << (i % 8))) {
                ui_color(UI_GREEN);
                fputs(BLK(), stdout);
            } else {
                ui_color(UI_DIM);
                fputs(FRG(), stdout);
            }
            ui_sleep(10);
        }
        ui_reset();
        printf("\n");
        ui_sleep(60);
    }
    ui_color(UI_GREEN);
    printf("   %s", BLK());
    ui_color(UI_DIM);
    printf(" used   %s", FRG());
    ui_reset();
    printf(" free    disk.img : %d blocks (%d used)\n", DISK_BLOCKS, used);
}

/* ---------------- OS MAIN ---------------- */

static void run_week(int which) {
    if (which == 1) {
        ui_frame_begin("WEEK 1 :: CPU SCHEDULING");
        printf("\n");
        week1();
        printf("\n");
        ui_frame_end();
    } else if (which == 2) {
        ui_frame_begin("WEEK 2 :: MEMORY PAGING");
        printf("\n");
        week2();
        printf("\n");
        ui_frame_end();
    } else {
        ui_frame_begin("WEEK 3 :: FILE SYSTEM");
        printf("\n");
        week3();
        printf("\n");
        ui_frame_end();
    }
}

int main(void) {
    char line[32];
    int w1 = 0, w2 = 0, w3 = 0, first = 1;

    ui_init();

    if (!ui_is_tty()) {
        run_week(1);
        run_week(2);
        run_week(3);
        return 0;
    }

    ui_boot();
    for (;;) {
        if (!first) {
            ui_clear();
        }
        first = 0;
        ui_taskbar(w1, w2, w3);
        printf("\n");
        ui_prompt();
        fflush(stdout);
        if (fgets(line, sizeof line, stdin) == NULL) {
            ui_shutdown();
            break;
        }
        if (line[0] == '\n' || line[0] == '\0') {
            continue;
        }
        if (line[0] == '1') {
            w1 = 1;
            run_week(1);
            ui_wait_enter();
        } else if (line[0] == '2') {
            w2 = 1;
            run_week(2);
            ui_wait_enter();
        } else if (line[0] == '3') {
            w3 = 1;
            run_week(3);
            ui_wait_enter();
        } else if (line[0] == 'A' || line[0] == 'a') {
            w1 = w2 = w3 = 1;
            run_week(1);
            ui_wait_enter();
            run_week(2);
            ui_wait_enter();
            run_week(3);
            ui_wait_enter();
        } else if (line[0] == '0') {
            ui_shutdown();
            break;
        } else {
            ui_color(UI_RED);
            printf("  unknown command : %c (try 1 2 3 A 0)\n", line[0]);
            ui_reset();
            ui_sleep(600);
        }
    }
    return 0;
}
