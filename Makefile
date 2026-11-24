CC = gcc
CFLAGS = -Wall -Wextra -I. -Imem -Ifs

SRC = main.c workload.c \
      util/gantt.c util/metrics.c \
      sched/fcfs.c sched/sjf.c sched/rr.c sched/priority.c

MEM_SRC = mem/translate.c mem/fifo.c mem/lru.c mem/optimal.c mem/tlb.c

FS_SRC = fs/fs.c

DEMO_SRC = workload.c util/gantt.c util/metrics.c \
           sched/fcfs.c sched/sjf.c sched/rr.c sched/priority.c \
           $(MEM_SRC) $(FS_SRC)

all: esinos test_memory demo fs_check test_fs menu

esinos: $(SRC) esinos.h
    $(CC) $(CFLAGS) $(SRC) -o esinos

test_memory: tests/test_memory.c $(MEM_SRC)
    $(CC) $(CFLAGS) -o $@ tests/test_memory.c $(MEM_SRC)

demo: demo.c $(DEMO_SRC) esinos.h mem/mem.h fs/fs.h
    $(CC) $(CFLAGS) -o $@ demo.c $(DEMO_SRC)

fs_check: fs/fs_check.c $(FS_SRC) fs/fs.h
    $(CC) $(CFLAGS) -o $@ fs/fs_check.c $(FS_SRC)

test_fs: tests/test_fs.c $(FS_SRC) fs/fs.h
    $(CC) $(CFLAGS) -o $@ tests/test_fs.c $(FS_SRC)

menu: menu.c $(DEMO_SRC) esinos.h mem/mem.h fs/fs.h
    $(CC) $(CFLAGS) -o $@ menu.c $(DEMO_SRC)

test: esinos test_memory demo fs_check test_fs
    ./test_memory
    ./fs_check
    ./test_fs
    ./demo
    sh tests/run.sh

clean:
    rm -f esinos test_memory demo fs_check test_fs menu *.o disk.img

.PHONY: all test clean
