#include <stdio.h>
#include <string.h>
#include "fs.h"

static const char MSG[] = "le fichier survit au redemarrage";

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "write") == 0) {
        fs_format();
        if (fs_create("notes") != 0) { return 1; }
        int len = (int)strlen(MSG);
        if (fs_write("notes", MSG, len) != len) { return 1; }
        printf("disque formate, notes ecrit : %d octets\n", len);
        return 0;
    }
    if (argc == 2 && strcmp(argv[1], "read") == 0) {
        char buf[FS_MAX_FILE_SIZE + 1];
        int got = fs_read("notes", buf, FS_MAX_FILE_SIZE);
        if (got != (int)strlen(MSG)) {
            printf("KO : %d octets lus au lieu de %d\n", got, (int)strlen(MSG));
            return 1;
        }
        buf[got] = '\0';
        if (strcmp(buf, MSG) != 0) {
            printf("KO : contenu altere\n");
            return 1;
        }
        printf("relance : notes = \"%s\" (%d octets)\n", buf, got);
        printf("FS WEEK 3 GREEN\n");
        return 0;
    }
    printf("usage : fs_check write|read\n");
    return 1;
}
