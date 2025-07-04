#ifndef RR_H
#define RR_H

#include "proc.h"
#include <stdint.h>

void init_rr_manual(s_process **processes,
                    uint32_t *num_processes); // Manually init the rr
// make proper automatic gen
void init_rr_automatic(
    s_process **processes,
    uint32_t *num_processes); // Fully random, fully random w/write to the file,
                              // read from the file

#endif /* RR_H */
