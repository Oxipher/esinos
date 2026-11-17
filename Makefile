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
