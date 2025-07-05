#include "proc.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void print_process(s_process *processes, int num_processess) {
  for (int i = 0; i < num_processess; ++i) {
    printf("Process %d\n", i + 1);
    printf("%u\n%d\n%u\n%u\n%u\n%u\n%u\n%u\n", processes[i].pid,
           processes[i].e_status, processes[i].quantum, processes[i].t_arrival,
           processes[i].t_burst, processes[i].t_completion,
           processes[i].t_turnaround, processes[i].t_waiting);
  }
}

s_process *generate_proc(const uint32_t amount, const uint8_t is_quant_static,
                         const uint32_t quantum, const uint32_t t_arrival_r,
                         const uint32_t t_burst_r) {
  s_process *processes = malloc(sizeof(s_process) * amount);

  if (!processes) {
    perror("Memory allocation failed");
    return NULL;
  }

  for (int i = 0; i < amount; ++i) {
    processes[i].pid = i;
    processes[i].e_status = READY;
    processes[i].quantum =
        is_quant_static == 1 ? quantum : (rand() % quantum) + 1;
    processes[i].t_arrival = (rand() % t_arrival_r) + 1;
    processes[i].t_burst = (rand() % t_burst_r) + 1;
    processes[i].t_completion = 0;
    processes[i].t_turnaround = 0;
    processes[i].t_waiting = 0;
  }

  return processes;
}

int comp_proc_arrv(const void *a, const void *b) {
  const s_process *p1 = (const s_process *)a;
  const s_process *p2 = (const s_process *)b;
  if (p1->t_arrival < p2->t_arrival) return -1;
  if (p1->t_arrival > p2->t_arrival) return 1;
  return 0;
}
