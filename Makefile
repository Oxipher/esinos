CC = gcc
CFLAGS = -Wall -Wextra -I. -Imem -Ifs

MEM_SRC = mem/translate.c mem/fifo.c mem/lru.c mem/optimal.c mem/tlb.c

FS_SRC = fs/fs.c

CORE_SRC = workload.c demo_core.c ui.c \
           util/gantt.c util/metrics.c \
           sched/fcfs.c sched/sjf.c sched/rr.c sched/priority.c \
           $(MEM_SRC) $(FS_SRC)

SRC = main.c $(CORE_SRC)

all: esinos test_memory demo fs_check test_fs menu

esinos: $(SRC) esinos.h mem/mem.h fs/fs.h ui.h
    $(CC) $(CFLAGS) $(SRC) -o esinos

test_memory: tests/test_memory.c $(MEM_SRC)
    $(CC) $(CFLAGS) -o $@ tests/test_memory.c $(MEM_SRC)

demo: demo.c $(CORE_SRC) esinos.h mem/mem.h fs/fs.h ui.h
    $(CC) $(CFLAGS) -o $@ demo.c $(CORE_SRC)

fs_check: tests/fs_check.c $(FS_SRC) fs/fs.h
    $(CC) $(CFLAGS) -o $@ tests/fs_check.c $(FS_SRC)

test_fs: tests/test_fs.c $(FS_SRC) fs/fs.h
    $(CC) $(CFLAGS) -o $@ tests/test_fs.c $(FS_SRC)

menu: menu.c $(CORE_SRC) esinos.h mem/mem.h fs/fs.h ui.h
    $(CC) $(CFLAGS) -o $@ menu.c $(CORE_SRC)

test: esinos test_memory demo fs_check test_fs
    ./test_memory
    ./fs_check write
    ./fs_check read
    ./test_fs
    ./demo
    sh tests/run.sh

clean:
    rm -f esinos test_memory demo fs_check test_fs menu *.o disk.img

.PHONY: all test clean
