#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

  // had to save the initial state for statistical purposes
  s_process *processes_init;
  processes_init = malloc(num_processes * sizeof(s_process));
  memcpy(processes_init, processes, num_processes * sizeof(s_process));

  puts("Initial processes statistics: ");
  print_process(processes, num_processes);

  round_robin(processes, &num_processes, r_queue);

  puts("Scheduled processes statistics: ");
  calculate_stats(processes, processes_init, num_processes);
  print_process(processes, num_processes);
  // print_queue(r_queue);

  free(processes);
  free(processes_init);
  free_queue(r_queue);
  free(r);
}
