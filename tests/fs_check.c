#include <stdio.h>  
#include <string.h>  /* String utilities (strcmp, strlen) */
#include "fs.h"      /* File system interface and constants */

/* Test payload written to disk and verified across runs */
static const char MSG[] = "le fichier survit au redemarrage";

int main(int argc, char **argv) {
    /* Write mode: format disk, create "notes", and write MSG */
    if (argc == 2 && strcmp(argv[1], "write") == 0) {
        fs_format();
        if (fs_create("notes") != 0) {
            return 1;
        }
        int len = (int)strlen(MSG);
        if (fs_write("notes", MSG, len) != len) {
            return 1;
        }
        printf("disque formate, notes ecrit : %d octets\n", len);
        return 0;
    }

    /* Read mode: read "notes" back and verify byte count and content */
    if (argc == 2 && strcmp(argv[1], "read") == 0) {
        char buf[FS_MAX_FILE_SIZE + 1];
        int got = fs_read("notes", buf, FS_MAX_FILE_SIZE);
        if (got != (int)strlen(MSG)) {
            printf("KO : %d octets lus au lieu de %d\n", got, (int)strlen(MSG));
            return 1;
        }
        /* Null-terminate buffer before string comparison and printing */
        buf[got] = '\0';
        if (strcmp(buf, MSG) != 0) {
            printf("KO : contenu altere\n");
            return 1;
        }
        printf("relance : notes = \"%s\" (%d octets)\n", buf, got);
        printf("FS WEEK 3 GREEN\n");
        return 0;
    }

    /* Invalid or missing CLI argument */
    printf("usage : fs_check write|read\n");
    return 1;
}
