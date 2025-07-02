#include <stdio.h>
#include <stdlib.h>

#include "rr/rr.h"
// #include "utils/file.h"

int main(void) {
  s_process *processes;
  int num_processes;
  char r;

  puts("Morningdawning's scheduler simulator (RR only for now)");
  puts("Select \"m\" for manual RR setup or \"a\" for automatic RR setup");

  do {
    scanf("%c", &r);
  } while (r != 'm');

  init_rr_manual(&processes, &num_processes);
  print_process(processes, num_processes);
}
