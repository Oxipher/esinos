# C compiler and flags: enable strict warnings and module include paths
CC = gcc
CFLAGS = -Wall -Wextra -I. -Imem -Ifs -Iui

# Memory simulation sources (address translation, FIFO, LRU, Optimal, TLB)
MEM_SRC = mem/translate.c mem/fifo.c mem/lru.c mem/optimal.c mem/tlb.c

# Custom disk filesystem source
FS_SRC = fs/fs.c

# Shared subsystem sources used across main, menu, and demo binaries
CORE_SRC = workload.c demo_core.c ui/ui.c \
           util/gantt.c util/metrics.c \
           sched/fcfs.c sched/sjf.c sched/rr.c sched/priority.c \
           $(MEM_SRC) $(FS_SRC)

# Complete source list for the primary esinos executable
SRC = main.c $(CORE_SRC)

# Default target: build all executables and test binaries
all: esinos test_memory demo fs_check test_fs menu

# Build main OS simulation executable
esinos: $(SRC) esinos.h mem/mem.h fs/fs.h ui/ui.h
	$(CC) $(CFLAGS) $(SRC) -o esinos

# Build standalone memory algorithm test binary
test_memory: tests/test_memory.c $(MEM_SRC)
	$(CC) $(CFLAGS) -o $@ tests/test_memory.c $(MEM_SRC)

# Build interactive/non-interactive 404 OS demo binary
demo: demo.c $(CORE_SRC) esinos.h mem/mem.h fs/fs.h ui/ui.h
	$(CC) $(CFLAGS) -o $@ demo.c $(CORE_SRC)

# Build filesystem write/read verification helper
fs_check: tests/fs_check.c $(FS_SRC) fs/fs.h
	$(CC) $(CFLAGS) -o $@ tests/fs_check.c $(FS_SRC)

# Build filesystem unit test binary
test_fs: tests/test_fs.c $(FS_SRC) fs/fs.h
	$(CC) $(CFLAGS) -o $@ tests/test_fs.c $(FS_SRC)

# Build interactive text menu binary
menu: menu.c $(CORE_SRC) esinos.h mem/mem.h fs/fs.h ui/ui.h
	$(CC) $(CFLAGS) -o $@ menu.c $(CORE_SRC)

# Build targets and run automated test binaries plus shell test suite
test: esinos test_memory demo fs_check test_fs
	./test_memory
	./fs_check write
	./fs_check read
	./test_fs
	./demo
	sh tests/run.sh

# Clean build artifacts, generated binaries, and virtual disk image
clean:
	rm -f esinos test_memory demo fs_check test_fs menu *.o disk.img

# Declare non-file phony targets
.PHONY: all test clean
