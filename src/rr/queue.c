#include "queue.h"
#include "proc.h"
#include <stdlib.h>
#include <string.h>

void init_queue(ready_queue *r_queue, s_process *processes,
                uint32_t num_processes) {
  r_queue->processes = malloc(sizeof(s_process) * num_processes);
  r_queue->capacity = num_processes;
  r_queue->size = num_processes;
  memcpy(r_queue->processes, processes, sizeof(s_process) * num_processes);
  qsort(r_queue->processes, num_processes, sizeof(s_process), comp_proc_arrv);
}

void free_queue(ready_queue *r_queue) {
  free(r_queue->processes);
  free(r_queue);
}

void print_queue(ready_queue *r_queue) {
  for (uint32_t i = 0; i < r_queue->size; ++i) {
    s_process *proc = &r_queue->processes[i];
    printf("Process %u\n", i + 1);
    printf("PID: %u\n", proc->pid);
    printf("Status: %d\n", proc->e_status);
    printf("Quantum: %u\n", proc->quantum);
    printf("Arrival Time: %u\n", proc->t_arrival);
    printf("Burst Time: %u\n", proc->t_burst);
    printf("Completion Time: %u\n", proc->t_completion);
    printf("Turnaround Time: %u\n", proc->t_turnaround);
    printf("Waiting Time: %u\n", proc->t_waiting);
    printf("\n");
  }
}

void push(ready_queue *r_queue, s_process proc) {
  if (r_queue->size < r_queue->capacity) {
    r_queue->processes[r_queue->size++] = proc;
  } else {
    perror("queue is full!");
  }
}

s_process pop(ready_queue *r_queue) {
  if (r_queue->size == 0) {
    perror("queue is empty!");
    exit(EXIT_FAILURE);
  }
  s_process proc = r_queue->processes[0];
  memmove(&r_queue->processes[0], &r_queue->processes[1],
          (r_queue->size - 1) * sizeof(s_process));
  r_queue->size--;
  return proc;
}

s_process *peek(ready_queue *r_queue) {
  if (r_queue->size == 0) {
    return NULL;
  }
  return &r_queue->processes[0];
}
