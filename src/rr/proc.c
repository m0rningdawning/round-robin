#include "proc.h"

#include <stdio.h>
#include <stdlib.h>

#include "../utils/randomize.h"

void print_process(s_process *processes, int num_processess) {
  for (int i = 0; i < num_processess; ++i) {
    printf("Process %d\n", i + 1);
    printf("  pid: %u\n", processes[i].pid);
    printf("  e_status: %d\n", processes[i].e_status);
    printf("  quantum: %u\n", processes[i].quantum);
    printf("  t_arrival: %u\n", processes[i].t_arrival);
    printf("  t_burst: %u\n", processes[i].t_burst);
    printf("  t_completion: %u\n", processes[i].t_completion);
    printf("  t_turnaround: %u\n", processes[i].t_turnaround);
    printf("  t_waiting: %u\n", processes[i].t_waiting);
  }
}

s_process *generate_proc(const uint32_t amount, const bool is_quant_static,
                         const uint32_t quantum, const uint32_t t_arrival_r,
                         const uint32_t t_burst_r) {
  s_process *processes = malloc(sizeof(s_process) * amount);

  if (!processes) {
    perror("Memory allocation failed");
    return NULL;
  }

  for (int i = 0; i < amount; ++i) {
    processes[i].pid = i;
    processes[i].e_status = OTW;
    processes[i].quantum = is_quant_static ? quantum : s_random_int(1, quantum);
    processes[i].t_arrival = s_random_int(0, t_arrival_r);
    processes[i].t_burst = s_random_int(1, t_burst_r);
    processes[i].t_completion = 0;
    processes[i].t_turnaround = 0;
    processes[i].t_waiting = 0;
  }

  return processes;
}

// Comparison function for quicksort
int comp_proc_arrv(const void *a, const void *b) {
  const s_process *p1 = (const s_process *)a;
  const s_process *p2 = (const s_process *)b;
  if (p1->t_arrival < p2->t_arrival) return -1;
  if (p1->t_arrival > p2->t_arrival) return 1;
  return 0;
}

void calculate_stats(s_process *processes, s_process *processes_init,
                     int num_processes) {
  for (int i = 0; i < num_processes; ++i) {
    processes[i].t_arrival = processes_init[i].t_arrival;
    processes[i].t_burst = processes_init[i].t_burst;
    int32_t turnaround = (int32_t)processes[i].t_completion -
                         (int32_t)processes_init[i].t_arrival;
    int32_t waiting = turnaround - (int32_t)processes_init[i].t_burst;
    processes[i].t_turnaround = turnaround;
    processes[i].t_waiting = waiting > 0 ? waiting : 0;
  }
}
