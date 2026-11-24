#include <stdio.h>
#include <string.h>
#include "fs/fs.h"

static void fs_menu(void)
{
    char name[64];
    char data[256];
    char buf[256];
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
            while ((c = getchar()) != '\n' && c != EOF) { }
            choice = -1;
        }

        if (choice == 2 || choice == 3 || choice == 4 || choice == 5) {
            printf("name: ");
            scanf("%63s", name);
        }

        if (choice == 1) {
            printf("fs_format -> %d\n", fs_format());
        } else if (choice == 2) {
            printf("fs_create(\"%s\") -> %d\n", name, fs_create(name));
        } else if (choice == 3) {
            printf("data: ");
            getchar();
            fgets(data, sizeof data, stdin);
            data[strcspn(data, "\n")] = '\0';
            printf("fs_write(\"%s\") -> %d\n", name, fs_write(name, data));
        } else if (choice == 4) {
            buf[0] = '\0';
            printf("fs_read(\"%s\") -> %d\n", name, fs_read(name, buf, sizeof buf));
            printf("content: \"%s\"\n", buf);
        } else if (choice == 5) {
            printf("fs_delete(\"%s\") -> %d\n", name, fs_delete(name));
        } else if (choice == 6) {
            fs_ls();
        }
    } while (choice != 0);
}
