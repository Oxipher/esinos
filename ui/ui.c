#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "ui.h"

static int g_tty = 0;

void ui_init(void) {
    g_tty = isatty(1);
}

int ui_is_tty(void) {
    return g_tty;
}

void ui_clear(void) {
    if (g_tty) {
        fputs("\033[2J\033[H", stdout);
    }
}

void ui_color(const char *code) {
    if (g_tty) {
        fputs(code, stdout);
    }
}

void ui_reset(void) {
    if (g_tty) {
        fputs("\033[0m", stdout);
    }
}

void ui_sleep(int ms) {
    struct timespec ts;
    if (!g_tty) {
        return;
    }
    fflush(stdout);
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

void ui_ok(const char *msg) {
    ui_color(UI_GREEN);
    fputs("[ OK ] ", stdout);
    ui_reset();
    printf("%s\n", msg);
}

void ui_title(const char *title) {
    int k;
    ui_color(UI_GREEN);
    printf("\n  +");
    for (k = 0; k < 44; k++) {
        putchar('-');
    }
    printf("+\n");
    printf("  | %-42s |\n", title);
    printf("  +");
    for (k = 0; k < 44; k++) {
        putchar('-');
    }
    printf("+\n");
    ui_reset();
}

void ui_boot(void) {
    static const char *BOOT[4] = {
        "loading scheduler module .............",
        "loading memory management unit .......",
        "loading file system driver ...........",
        "mounting disk.img ...................."
    };
    int i;

    ui_clear();
    ui_color(UI_GREEN);
    fputs("\n"
"  ███╗   ██╗  ██████╗  ███╗   ██╗\n"
"  ████╗  ██║ ██╔═══██╗ ████╗  ██║\n"
"  ██╔██╗ ██║ ██║   ██║ ██╔██╗ ██║\n"
"  ██║╚██╗██║ ██║   ██║ ██║╚██╗██║\n"
"  ██║ ╚████║ ╚██████╔╝ ██║ ╚████║\n"
"  ╚═╝  ╚═══╝  ╚═════╝  ╚═╝  ╚═══╝\n"
"\n", stdout);
    ui_reset();
    ui_color(UI_DIM);
    fputs("      the OS that never finds its page\n\n", stdout);
    ui_reset();

    ui_color(UI_GREEN);
    printf("  [");
    for (i = 0; i < 40; i++) {
        fputs("█", stdout);
        if ((i + 1) % 2 == 0) {
            ui_sleep(30);
        }
    }
    ui_reset();
    printf("] 100%%\n\n");

    for (i = 0; i < 4; i++) {
        ui_color(UI_DIM);
        printf("  %s ", BOOT[i]);
        ui_sleep(120);
        ui_color(UI_GREEN);
        printf("[ OK ]\n");
        ui_reset();
    }
    printf("\n");
}

static void taskbar_border(int close) {
    int i;
    if (close) {
        printf(" ╚");
    } else {
        printf(" ╔");
    }
    for (i = 0; i < 70; i++) {
        fputs(g_tty ? "═" : "=", stdout);
    }
    if (close) {
        printf("╝\n");
    } else {
        printf("╗\n");
    }
}

static void btn(int open, const char *label) {
    ui_color(open ? UI_GREEN : UI_DIM);
    printf(" %s ", label);
}

void ui_taskbar(int w1, int w2, int w3) {
    taskbar_border(0);
    printf(" ║");
    ui_color(UI_GREEN);
    printf(" 404 OS ");
    ui_color(UI_DIM);
    printf("│");
    btn(w1, "[1] Scheduling");
    ui_color(UI_DIM);
    printf("│");
    btn(w2, "[2] Memory");
    ui_color(UI_DIM);
    printf("│");
    btn(w3, "[3] Files");
    ui_color(UI_DIM);
    printf("│");
    btn(0, "[A] All");
    ui_color(UI_DIM);
    printf("│");
    btn(0, "[0] OFF");
    ui_color(UI_GREEN);
    printf("║\n");
    taskbar_border(1);
    ui_reset();
}

void ui_prompt(void) {
    ui_color(UI_GREEN);
    printf(" 404:~$ ");
    ui_reset();
}

void ui_wait_enter(void) {
    char tmp[16];
    if (!g_tty) {
        return;
    }
    ui_color(UI_DIM);
    printf("  press ENTER to return to desktop ...");
    ui_reset();
    fflush(stdout);
    if (fgets(tmp, sizeof tmp, stdin) == NULL) {
        return;
    }
}

void ui_frame_begin(const char *title) {
    int i, fill = 54 - (int)strlen(title);
    if (fill < 0) {
        fill = 0;
    }
    ui_color(UI_GREEN);
    if (g_tty) {
        printf(" ╔══[ %s ]", title);
        for (i = 0; i < fill; i++) { fputs("═", stdout); }
        printf("╗\n");
    } else {
        printf(" +--[ %s ]", title);
        for (i = 0; i < fill; i++) { putchar('-'); }
        printf("+\n");
    }
    ui_reset();
}

void ui_frame_end(void) {
    int i;
    ui_color(UI_GREEN);
    if (g_tty) {
        printf(" ╚");
        for (i = 0; i < 60; i++) { fputs("═", stdout); }
        printf("╝\n");
    } else {
        printf(" +");
        for (i = 0; i < 60; i++) { putchar('-'); }
        printf("+\n");
    }
    ui_reset();
}

void ui_shutdown(void) {
    static const char *L[4] = {
        "saving disk.img .....................",
        "closing scheduler ...................",
        "closing memory ......................",
        "closing file system ................."
    };
    int i;

    printf("\n");
    ui_color(UI_DIM);
    printf("  shutting down 404 OS ...\n");
    ui_reset();
    for (i = 0; i < 4; i++) {
        ui_color(UI_DIM);
        printf("  %s ", L[i]);
        ui_sleep(110);
        ui_color(UI_GREEN);
        printf("[ OK ]\n");
        ui_reset();
    }
    printf("\n");
    ui_color(UI_GREEN);
    printf("  goodbye. see you next week.\n\n");
    ui_reset();
}
