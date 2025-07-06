#ifndef PROC_H
#define PROC_H

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

enum PROC_STATUS { OTW, WAITING, READY, RUNNING, BLOCKED, FINISHED };

typedef struct {
  uint32_t pid;
  uint32_t quantum;
  uint32_t t_arrival;
  uint32_t t_burst;
  uint32_t t_completion;
  uint32_t t_turnaround;
  uint32_t t_waiting;
  enum PROC_STATUS e_status;
  pthread_mutex_t m_lock;
} s_process;

void print_process(s_process *processes, int num_processes);
s_process *generate_proc(const uint32_t amount, const uint8_t is_quant_static,
                         const uint32_t quantum, const uint32_t t_arrival_r,
                         const uint32_t t_burst_r);
int comp_proc_arrv(const void *a, const void *b);

#endif /* ifndef PROC_H */
