#ifndef QUEUE_H
#define QUEUE_H

#include "proc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  s_process *processes;
  uint32_t size;
  uint32_t capacity;
} ready_queue;

void init_queue(ready_queue *r_queue, s_process *processes,
                uint32_t num_processes);
void free_queue(ready_queue *r_queue);
void print_queue(ready_queue *r_queue);

void push(ready_queue *r_queue, s_process proc);

s_process pop(ready_queue *r_queue);
s_process *peek(ready_queue *r_queue);

#endif /* QUEUE_H */
