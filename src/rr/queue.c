#include "queue.h"

#include <stdlib.h>
#include <string.h>

#include "proc.h"

void init_queue(ready_queue *r_queue, s_process *processes,
                uint32_t num_processes) {
  r_queue->processes = malloc(sizeof(s_process) * num_processes);
  r_queue->capacity = num_processes;
  r_queue->size = 0;
  r_queue->head = 0;
  r_queue->tail = 0;
  // memcpy(r_queue->processes, processes, sizeof(s_process) * num_processes);
  // qsort(r_queue->processes, num_processes, sizeof(s_process),
  // comp_proc_arrv);
}

void free_queue(ready_queue *r_queue) {
  free(r_queue->processes);
  free(r_queue);
}

void print_queue(ready_queue *r_queue) {
  for (uint32_t i = 0; i < r_queue->size; ++i) {
    s_process *proc = r_queue->processes[i];
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

void push_back(ready_queue *r_queue, s_process *proc) {
  pthread_mutex_lock(&r_queue->m_lock);
  if (r_queue->size < r_queue->capacity) {
    r_queue->processes[r_queue->tail] = proc;
    r_queue->tail = (r_queue->tail + 1) % r_queue->capacity;
    r_queue->size++;
  } else {
    perror("queue is full!");
  }
  pthread_mutex_unlock(&r_queue->m_lock);
}

s_process *pop(ready_queue *r_queue) {
  pthread_mutex_lock(&r_queue->m_lock);
  if (r_queue->size == 0) {
    pthread_mutex_unlock(&r_queue->m_lock);
    return NULL;
  }
  s_process *proc = r_queue->processes[r_queue->head];
  r_queue->head = (r_queue->head + 1) % r_queue->capacity;
  r_queue->size--;
  pthread_mutex_unlock(&r_queue->m_lock);
  return proc;
}

s_process *peek(ready_queue *r_queue) {
  pthread_mutex_lock(&r_queue->m_lock);
  if (r_queue->size == 0) {
    pthread_mutex_unlock(&r_queue->m_lock);
    return NULL;
  }
  s_process *proc = r_queue->processes[r_queue->head];
  pthread_mutex_unlock(&r_queue->m_lock);
  return proc;
}
