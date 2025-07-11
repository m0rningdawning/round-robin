#ifndef RR_H_
#define RR_H_

#include <pthread.h>
#include <stdint.h>

#include "proc.h"
#include "queue.h"

typedef struct {
  s_process *processes;
  uint32_t *num_processes;
  ready_queue *r_queue;
  pthread_mutex_t *m_tick;
  pthread_mutex_t *m_master;
  pthread_cond_t *con_tick;
  volatile uint32_t *time;
  volatile bool sim_finished;
} rr_thread_args;

void init_rr_manual(s_process **processes, uint32_t *num_processes,
                    ready_queue **r_queue);  // Manually init the rr
void init_rr_automatic(
    s_process **processes, uint32_t *num_processes,
    ready_queue **r_queue);  // Fully random, fully random w/write to the
                             // file, read from the file

void *process_task(s_process *process);
void *populate_arrival_task(void *arg);
void *schedule_reschedule(void *arg);
void round_robin(s_process *processes, uint32_t *num_processes,
                 ready_queue *r_queue);

#endif /* RR_H_ */
