CC     = gcc
CFLAGS = -I. -Wall -Wextra
SRC    = main.c workload.c esinos.h \
         util/gantt.c util/metrics.c \
         sched/fcfs.c sched/sjf.c sched/rr.c sched/priority.c

esinos:
	$(CC) $(CFLAGS) $(SRC) -o esinos

test: esinos
	./tests/run.sh

clean:
	rm -f esinos *.o
