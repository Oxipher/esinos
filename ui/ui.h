#ifndef UI_H
#define UI_H

/* 404 OS - CRT green retro interface (design B+C+D valide).
 * Tout retombe en ASCII pur quand stdout n'est pas un terminal :
 * pas de couleurs, pas de delais, pas d'effacement d'ecran ->
 * make test, run.sh et les pipes restent 100 % automatiques. */

#define UI_GREEN  "\033[1;32m"  /* vert vif   : chrome OS, verdicts      */
#define UI_DIM    "\033[32m"    /* vert normal: etiquettes, secondaire   */
#define UI_WHITE  "\033[1;37m"
#define UI_YELLOW "\033[1;33m"
#define UI_CYAN   "\033[1;36m"
#define UI_MAGENTA "\033[1;35m"
#define UI_RED    "\033[1;31m"

void ui_init(void);            /* a appeler une fois au demarrage        */
int  ui_is_tty(void);          /* 1 = terminal interactif                */
void ui_clear(void);           /* efface l'ecran (tty uniquement)        */
void ui_color(const char *code); /* code ANSI brut (ignore si pipe)      */
void ui_reset(void);           /* retour a la couleur par defaut         */
void ui_sleep(int ms);         /* flush + pause (tty uniquement)         */

void ui_ok(const char *msg);     /* ligne [ OK ] msg                     */
void ui_title(const char *t);    /* petit titre encadre (demo_core)      */
void ui_boot(void);              /* 404 geant + barre + lignes BIOS      */
void ui_taskbar(int w1, int w2, int w3); /* barre de taches du bureau    */
void ui_prompt(void);            /* 404:~$                               */
void ui_wait_enter(void);        /* press ENTER ... (tty uniquement)     */
void ui_frame_begin(const char *title); /* bord haut de fenetre         */
void ui_frame_end(void);                /* bord bas de fenetre          */
void ui_shutdown(void);          /* ecran de fin                         */

#endif
