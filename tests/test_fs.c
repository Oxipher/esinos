#include <stdio.h>
#include <string.h>
#include "fs/fs.h"

static int passed = 0;
static int failed = 0;

static void check(int cond, const char *label)
{
    if (cond) {
        printf("PASS  %s\n", label);
        passed++;
    } else {
        printf("FAIL  %s\n", label);
        failed++;
    }
}

int main(void)
{
    char buf[FS_MAX_FILE_SIZE + 1];
    const char *msg = "hello week 3";
    int got;

    printf("== test_fs ==\n");

    fs_format();
    check(1, "format does not crash");

    check(fs_create("a.txt") == 0, "create a.txt");
    check(fs_create("a.txt") != 0, "create duplicate a.txt fails");
    check(fs_write("a.txt", msg, (int)strlen(msg)) == (int)strlen(msg), "write a.txt");

    fs_ls();
    check(1, "ls lists without crash");

    buf[0] = '\0';
    got = fs_read("a.txt", buf, (int)sizeof buf);
    check(got == (int)strlen(msg), "read a.txt");
    if (got >= 0) { buf[got] = '\0'; }
    check(strcmp(buf, msg) == 0, "read returns what was written");

    check(fs_write("ghost.txt", "x", 1) < 0, "write nonexistent file fails");
    check(fs_read("ghost.txt", buf, (int)sizeof buf) < 0, "read nonexistent file fails");

    check(fs_delete("a.txt") == 0, "delete a.txt");
    check(fs_delete("a.txt") != 0, "delete deleted a.txt fails");
    check(fs_read("a.txt", buf, (int)sizeof buf) < 0, "read deleted a.txt fails");
    check(fs_create("a.txt") == 0, "create reuses freed slot");

    fs_format();
    buf[0] = '\0';
    check(fs_read("a.txt", buf, (int)sizeof buf) < 0, "format wipes previous files");

    printf("== %d passed, %d failed ==\n", passed, failed);
    return failed > 0;
}
