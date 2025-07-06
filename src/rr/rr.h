#ifndef RR_H
#define RR_H

#include "proc.h"
#include "queue.h"
#include <pthread.h>
#include <stdint.h>

typedef struct {
  s_process *processes;
  uint32_t *num_processes;
  ready_queue *r_queue;
} rr_thread_args;

void init_rr_manual(s_process **processes, uint32_t *num_processes,
                    ready_queue **r_queue); // Manually init the rr
// make proper automatic gen
void init_rr_automatic(
    s_process **processes, uint32_t *num_processes,
    ready_queue **r_queue); // Fully random, fully random w/write to the
                            // file, read from the file

void *process_task(s_process *process);
void *populate_arrival(void *arg);
void round_robin(s_process *processes, uint32_t *num_processes,
                 ready_queue *r_queue);

#endif /* RR_H */
