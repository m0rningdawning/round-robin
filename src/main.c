#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rr/queue.h"
#include "rr/rr.h"
#include "utils/utils.h"

// #include "utils/file.h"
// Fix leaks on wrong Input!

int main(void) {
  // Rand seed
  srand(time(NULL));

  s_process *processes;
  uint32_t num_processes;
  ready_queue *r_queue;
  char *r;

  puts("Morningdawning's scheduler simulator (RR only for now)");
  puts("Select \"m\" for manual RR setup or \"a\" for automatic RR setup");

  do {
    r = get_string();
    if (!r) {
      puts("Input error!");
      return 1;
    }
  } while (r[0] != 'm' && r[0] != 'a');

  if (r[0] == 'a') {
    init_rr_automatic(&processes, &num_processes, &r_queue);
  } else {
    init_rr_manual(&processes, &num_processes, &r_queue);
  }

  // The algo here
  round_robin(processes, &num_processes, r_queue);

  print_process(processes, num_processes);
  // print_queue(r_queue);

  free(processes);
  free_queue(r_queue);
  free(r);
}
