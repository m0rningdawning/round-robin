#include "proc.h"

#include <stdio.h>

void print_process(s_process *processes, int num_processess) {
  for (int i = 0; i < num_processess; ++i) {
    printf("Process %d\n", i);
    printf("%u\n%d\n%u\n%u\n%u\n%u\n%u\n%u\n",
           processes[i].pid,
           processes[i].e_status,
           processes[i].quantum,
           processes[i].t_arrival,
           processes[i].t_burst,
           processes[i].t_completion,
           processes[i].t_turnaround,
           processes[i].t_waiting);
  }
}
