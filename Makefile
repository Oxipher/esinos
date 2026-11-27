# Compiler and warning/include flags
CC = gcc
CFLAGS = -Wall -Wextra -I. -Imem -Ifs -Iui

# Memory management source files
MEM_SRC = mem/translate.c mem/fifo.c mem/lru.c mem/optimal.c mem/tlb.c

# Filesystem source files
FS_SRC = fs/fs.c

# Shared simulation, UI, utility, scheduling, memory, and filesystem sources
CORE_SRC = workload.c demo_core.c ui/ui.c \
           util/gantt.c util/metrics.c \
           sched/fcfs.c sched/sjf.c sched/rr.c sched/priority.c \
           $(MEM_SRC) $(FS_SRC)

# All source files needed for the main binary
SRC = main.c $(CORE_SRC)

# Default target: build all binaries and test executables
all: esinos test_memory demo fs_check test_fs menu

# Main OS simulator executable
esinos: $(SRC) esinos.h mem/mem.h fs/fs.h ui/ui.h
	$(CC) $(CFLAGS) $(SRC) -o esinos

# Memory module test binary
test_memory: tests/test_memory.c $(MEM_SRC)
	$(CC) $(CFLAGS) -o $@ tests/test_memory.c $(MEM_SRC)

# Interactive/demo executable
demo: demo.c $(CORE_SRC) esinos.h mem/mem.h fs/fs.h ui/ui.h
	$(CC) $(CFLAGS) -o $@ demo.c $(CORE_SRC)

# Filesystem read/write check utility
fs_check: tests/fs_check.c $(FS_SRC) fs/fs.h
	$(CC) $(CFLAGS) -o $@ tests/fs_check.c $(FS_SRC)

# Filesystem test binary
test_fs: tests/test_fs.c $(FS_SRC) fs/fs.h
	$(CC) $(CFLAGS) -o $@ tests/test_fs.c $(FS_SRC)

# Menu executable
menu: menu.c $(CORE_SRC) esinos.h mem/mem.h fs/fs.h ui/ui.h
	$(CC) $(CFLAGS) -o $@ menu.c $(CORE_SRC)

# Build and run automated tests and scripts
test: esinos test_memory demo fs_check test_fs
	./test_memory
	./fs_check write
	./fs_check read
	./test_fs
	./demo
	sh tests/run.sh

# Remove generated binaries, object files, and virtual disk image
clean:
	rm -f esinos test_memory demo fs_check test_fs menu *.o disk.img

# Targets that do not correspond to output files
.PHONY: all test clean
