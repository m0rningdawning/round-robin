// This will handle the file related IO:
// - Read the proc/queue description form the file
// - Save the log file (pipe from log.h)

#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include "../rr/proc.h"

s_process* read_proc_file(FILE* file);
void write_proc_file(s_process* process);
void write_log_file();

#endif /* FILE_H  */
