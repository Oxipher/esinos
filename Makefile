CC = gcc
CFLAGS = -I. -Wall -Wextra
SRC = main.c workload.c \
util/gantt.c util/metrics.c \
sched/fcfs.c sched/sjf.c sched/rr.c sched/priority.c
esinos: $(SRC) esinos.h
$(CC) $(CFLAGS) $(SRC) -o esinos
test: esinos
sh tests/run.sh
clean:
rm -f esinos *.o

MEM_SRC = mem/translate.c mem/fifo.c mem/lru.c mem/optimal.c mem/tlb.c
CFLAGS  = -Wall -Wextra -I. -Imem

test_memory: tests/test_memory.c $(MEM_SRC)
	$(CC) $(CFLAGS) -o $@ tests/test_memory.c $(MEM_SRC)

test: test_memory
	./test_memory && ./tests/run.sh
