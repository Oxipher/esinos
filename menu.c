#include <stdio.h>
#include <string.h>
#include "fs/fs.h"

static void fs_menu(void)
{
    char name[FS_NAME_MAX + 2] = "";
    char data[FS_MAX_FILE_SIZE + 1];
    int choice;

    do {
        printf("\n=== FS MENU ===\n");
        printf("1. format\n");
        printf("2. create\n");
        printf("3. write\n");
        printf("4. read\n");
        printf("5. delete\n");
        printf("6. ls\n");
        printf("0. back\n");
        printf("choice: ");
        if (scanf("%d", &choice) != 1) {
            int c;
            if (feof(stdin)) { return; }   /* EOF: leave instead of looping forever */
            while ((c = getchar()) != '\n' && c != EOF) { }
            choice = -1;
        }

        if (choice >= 2 && choice <= 5) {
            printf("name: ");
            if (scanf("%11s", name) != 1) {
                if (feof(stdin)) { return; }
                choice = -1;
            }
        }

        if (choice == 1) {
            fs_format();
            printf("fs_format -> done\n");
        } else if (choice == 2) {
            printf("fs_create(\"%s\") -> %d\n", name, fs_create(name));
        } else if (choice == 3) {
            printf("data: ");
            getchar();
            if (fgets(data, sizeof data, stdin) == NULL) {
                data[0] = '\0';
            }
            data[strcspn(data, "\n")] = '\0';
            printf("fs_write(\"%s\") -> %d\n",
                   name, fs_write(name, data, (int)strlen(data)));
        } else if (choice == 4) {
            int got = fs_read(name, data, FS_MAX_FILE_SIZE);
            if (got < 0) {
                printf("fs_read(\"%s\") -> -1 (fichier absent)\n", name);
            } else {
                data[got] = '\0';
                printf("fs_read(\"%s\") -> %d : \"%s\"\n", name, got, data);
            }
        } else if (choice == 5) {
            printf("fs_delete(\"%s\") -> %d\n", name, fs_delete(name));
        } else if (choice == 6) {
            fs_ls();
        }
    } while (choice != 0);
}

int main(void)
{
    fs_menu();
    return 0;
}
