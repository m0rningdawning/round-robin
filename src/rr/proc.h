#ifndef PROC_H
#define PROC_H

#include <stdint.h>

enum PROC_STATUS {
  READY, RUNNING, WAITING, BLOCKED, FINISHED
};

typedef struct {
  uint32_t pid;
  uint32_t quantum;
  uint32_t t_arrival;
  uint32_t t_burst;
  uint32_t t_completion;
  uint32_t t_turnaround;
  uint32_t t_waiting;
  enum PROC_STATUS e_status;
} s_process;

void print_process(s_process* processes, int num_processes);

#endif /* ifndef PROC_H */
