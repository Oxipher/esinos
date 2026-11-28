#include <stdio.h>   
#include "esinos.h"  /* Project header declaring Gantt chart functions */

/* Appends a single task time block to the ASCII Gantt chart */
void gantt_add(const char *name, int start, int end) {
    printf("| %s %d-%d ", name, start, end);
}

/* Closes the Gantt chart row with a trailing pipe and newline */
void gantt_finish(void) {
    printf("|\n");
}
