#include "file.h"

#include <stdio.h>
#include <stdlib.h>

s_process *read_proc_file(const char *path, const uint32_t count) {
  if (!path) {
    puts("Please provide the path to the file!");
    return NULL;
  }
  FILE *f = fopen(path, "r");
  if (!f) {
    puts("Can't open the file, check the path!");
    return NULL;
  }

  s_process *proc = malloc(count * sizeof(s_process));
  if (!proc) {
    fclose(f);
    puts("Memory allocation failed!");
    return NULL;
  }

  char buf[256];
  size_t idx = 0;
  while (fgets(buf, sizeof(buf), f) && idx < count) {
    if (sscanf(buf, "%u,%u,%u,%u", &proc[idx].pid, &proc[idx].quantum,
               &proc[idx].t_arrival, &proc[idx].t_burst) == 4) {
      proc[idx].t_completion = 0;
      proc[idx].t_turnaround = 0;
      proc[idx].t_waiting = 0;
      proc[idx].e_status = OTW;
      idx++;
    }
  }

  fclose(f);
  return proc;
}

void write_proc_file(const char *path, s_process *process,
                     const uint32_t size) {
  if (!path || !process) {
    puts("Invalid path or process pointer!");
    return;
  }
  FILE *f = fopen(path, "w");
  if (!f) {
    puts("Can't open the file for writing!");
    return;
  }
  for (size_t i = 0; i < size; ++i) {
    fprintf(f, "%u,%u,%u,%u\n", process[i].pid, process[i].quantum,
            process[i].t_arrival, process[i].t_burst);
  }
  fclose(f);
}

void write_log_file();
