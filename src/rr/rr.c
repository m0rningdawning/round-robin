#include "rr.h"

#include <stdio.h>
#include <stdlib.h>

void init_rr_manual(s_process **processes, int *num_processes) {
  printf("Enter number of processes: ");
  scanf("%d", num_processes);

  *processes = malloc((*num_processes) * sizeof(s_process));
  if (*processes == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < *num_processes; ++i) {
    printf("Proecss %d: \n", i);
    printf("Process %d PID: \n", i + 1);
    scanf("%u", &((*processes)[i].pid));
    printf("Process %d Quantum: \n", i + 1);
    scanf("%u", &((*processes)[i].quantum));
    printf("Process %d Arrival Time: \n", i + 1);
    scanf("%u", &((*processes)[i].t_arrival));
    printf("Process %d Burst Time: \n", i + 1);
    scanf("%u", &((*processes)[i].t_burst));
    (*processes)[i].t_completion = 0;
    (*processes)[i].t_turnaround = 0;
    (*processes)[i].t_waiting = 0;
    (*processes)[i].e_status = READY;
  }
}
