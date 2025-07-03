// This will handle the file related IO:
// - Read the proc/queue description form the file
// - Save the log file (pipe from log.h)

#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include "../rr/proc.h"

s_process* read_proc_file(const char* path, const uint32_t size);
void write_proc_file(const char* path, s_process* process, const uint32_t size);
// finish this
void write_log_file();

#endif /* FILE_H  */
