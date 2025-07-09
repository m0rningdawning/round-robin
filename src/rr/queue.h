#ifndef QUEUE_H
#define QUEUE_H

#include "proc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
  s_process **processes;
  uint32_t head;
  uint32_t tail;
  uint32_t size;
  uint32_t capacity;
  pthread_mutex_t m_lock;
} ready_queue;

void init_queue(ready_queue *r_queue, s_process *processes,
                uint32_t num_processes);
void free_queue(ready_queue *r_queue);
void print_queue(ready_queue *r_queue);

void push_back(ready_queue *r_queue, s_process *proc);
s_process *pop(ready_queue *r_queue);
s_process *peek(ready_queue *r_queue);

#endif /* QUEUE_H */
