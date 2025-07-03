#include <stdio.h>
#include <stdlib.h>

#include "rr/rr.h"
#include "utils/utils.h"

// #include "utils/file.h"

int main(void) {
  s_process *processes;
  uint32_t num_processes;
  char *r;

  puts("Morningdawning's scheduler simulator (RR only for now)");
  puts("Select \"m\" for manual RR setup or \"a\" for automatic RR setup");

  do {
    r = get_string();
    if (!r) {
      puts("Input error!");
      return 1;
    }
  } while (r[0] != 'm');

  free(r);

  init_rr_manual(&processes, &num_processes);
  print_process(processes, num_processes);
}
