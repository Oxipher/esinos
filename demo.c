#include <stdio.h>
#include "esinos.h"
#include "ui.h"

/* 404 OS desktop : ./demo ouvre le bureau interactif sur un terminal.
 * Sans terminal (make test, pipes) : les 3 semaines defilent en ASCII
 * pur, sans attente -> les tests restent automatiques.
 * Le contenu des semaines vit dans demo_core.c (partage avec ./esinos). */

/* Invoke the visual demo renderer for the requested week (1, 2, or 3) */
static void run_week(int which) {
    demo_week_visual(which);
}

int main(void) {
    char line[32];              /* Input buffer for interactive menu commands */
    int w1 = 0, w2 = 0, w3 = 0; /* Track which week windows have been opened */
    int first = 1;              /* Skip clearing screen on the very first loop iteration */

    /* Detect whether stdout is attached to an interactive terminal */
    ui_init();

    /* Non-interactive mode (pipes, make test): run all weeks sequentially without pausing */
    if (!ui_is_tty()) {
        run_week(1);
        run_week(2);
        run_week(3);
        return 0;
    }

    /* Interactive desktop loop */
    ui_boot();
    for (;;) {
        if (!first) {
            ui_clear();
        }
        first = 0;

        /* Render taskbar state and command prompt */
        ui_taskbar(w1, w2, w3);
        printf("\n");
        ui_prompt();
        fflush(stdout);

        /* Handle EOF (Ctrl+D) gracefully as shutdown */
        if (fgets(line, sizeof line, stdin) == NULL) {
            ui_shutdown();
            break;
        }

        /* Ignore empty lines */
        if (line[0] == '\n' || line[0] == '\0') {
            continue;
        }

        /* Dispatch user command */
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
            /* Mark all modules visited and step through Weeks 1, 2, and 3 */
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
            /* Report unrecognized input and pause briefly before redrawing */
            ui_color(UI_RED);
            printf("  unknown command : %c (try 1 2 3 A 0)\n", line[0]);
            ui_reset();
            ui_sleep(600);
        }
    }
    return 0;
}
