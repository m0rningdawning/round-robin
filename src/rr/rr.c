#include "rr.h"

#include "../utils/file.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_rr_manual(s_process **processes, uint32_t *num_processes) {
  printf("Enter number of processes: ");
  *num_processes = get_int(0);

  *processes = malloc((*num_processes) * sizeof(s_process));
  if (*processes == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < *num_processes; ++i) {
    printf("Proecss %d: \n", i);
    printf("Process %d PID: \n", i);
    (*processes)[i].pid = get_int(0);
    printf("Process %d Quantum: \n", i);
    (*processes)[i].quantum = get_int(0);
    printf("Process %d Arrival Time: \n", i);
    (*processes)[i].t_arrival = get_int(0);
    printf("Process %d Burst Time: \n", i);
    (*processes)[i].t_burst = get_int(0);
    (*processes)[i].t_completion = 0;
    (*processes)[i].t_turnaround = 0;
    (*processes)[i].t_waiting = 0;
    (*processes)[i].e_status = READY;
  }
}

// Finish this properly
void init_rr_automatic(s_process **processes, uint32_t *num_processes) {
  FILE *f = fopen("./proc.txt", "r");
  if (!f) {
    puts("Can't open proc.txt!");
    *num_processes = 0;
    *processes = NULL;
    return;
  }

  uint32_t count = 0;
  char buf[256];
  while (fgets(buf, sizeof(buf), f)) {
    if (strspn(buf, " \t\r\n") != strlen(buf))
      count++;
  }
  fclose(f);

  *num_processes = count;
  *processes = read_proc_file("./proc.txt", count * sizeof(s_process));
}
