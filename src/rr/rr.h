#ifndef RR_H
#define RR_H

#include "proc.h"

void init_rr_random(); // Fully random, fully random w/write to the file, read from the file
void init_rr_manual(s_process** processes, int *num_processes); // Manually init the rr

#endif /* RR_H */
