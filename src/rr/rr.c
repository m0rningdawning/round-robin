#include "rr.h"

#include "../utils/file.h"
#include "../utils/utils.h"
#include "proc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// After the RR is implemented, add the possibility to optionally save logs to
// the file

void init_rr_manual(s_process **processes, uint32_t *num_processes) {
  printf("Enter number of processes: ");
  *num_processes = get_int(0);

  *processes = malloc((*num_processes) * sizeof(s_process));
  if (*processes == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < *num_processes; ++i) {
    printf("Proecss %d: \n", i);
    printf("Process %d PID: \n", i);
    (*processes)[i].pid = get_int(0);
    printf("Process %d Quantum: \n", i);
    (*processes)[i].quantum = get_int(0);
    printf("Process %d Arrival Time: \n", i);
    (*processes)[i].t_arrival = get_int(0);
    printf("Process %d Burst Time: \n", i);
    (*processes)[i].t_burst = get_int(0);
    (*processes)[i].t_completion = 0;
    (*processes)[i].t_turnaround = 0;
    (*processes)[i].t_waiting = 0;
    (*processes)[i].e_status = READY;
  }

  puts("Do you want to save the processes into the \"proc.txt\" file for "
       "future use? [y/n]");
  char ans;
  do {
    ans = get_string()[0];
  } while (ans != 'y' && ans != 'n');

  if (ans == 'y') {
    write_proc_file("./proc.txt", *processes, *num_processes);
  }

  // Ask if the user wants logs to be stored in a file

  // Proceed with the rr algo here
}

// Finish this properly
void init_rr_automatic(s_process **processes, uint32_t *num_processes) {
  puts("Do you want to load the processes from the file? [y/n]");
  char ans;

  do {
    ans = get_string()[0];
  } while (ans != 'y' && ans != 'n');

  if (ans == 'y') {
    FILE *f = fopen("./proc.txt", "r");
    if (!f) {
      puts("Can't open proc.txt!");
      *num_processes = 0;
      *processes = NULL;
      return;
    }

    uint32_t count = 0;
    char buf[256];
    while (fgets(buf, sizeof(buf), f)) {
      if (strspn(buf, " \t\r\n") != strlen(buf))
        count++;
    }
    fclose(f);

    *num_processes = count;
    *processes = read_proc_file("./proc.txt", count * sizeof(s_process));
  } else {
    uint8_t is_quant_static;
    uint32_t quantum, t_arrival_r, t_burst_r;
    ans = '\0';

    // Ask for the amount of processes
    puts("Enter the amount of processes to generate (1 - 50):");
    do {
      *num_processes = get_int(0);
    } while (*num_processes <= 0 || *num_processes >= 51);

    // Ask for quantum type
    puts("Do you want the quantum type be constant or randomly generated in a "
         "certain range? [y/n]");
    do {
      ans = get_string()[0];
    } while (ans != 'y' && ans != 'n');

    if (ans == 'y') {
      is_quant_static = 1;
      puts("Enter the quantum time (1 - 20): ");
    } else {
      is_quant_static = 0;
      puts("Enter the quantum time max value in range (1 - 20):");
    }

    // Ask for the quantum itself (if static - quant, if not - range)
    do {
      quantum = get_int(0);
    } while (quantum <= 0 || quantum >= 21);

    // Ask for arrival range
    puts("Enter the arrival time range (1 - 20): ");
    do {
      t_arrival_r = get_int(0);
    } while (t_arrival_r <= 0 || t_arrival_r >= 21);

    // Ask for burst range
    puts("Enter the burst time range (1 - 20): ");
    do {
      t_burst_r = get_int(0);
    } while (t_burst_r <= 0 || t_burst_r >= 21);

    *processes = generate_proc(*num_processes, is_quant_static, quantum,
                               t_arrival_r, t_burst_r);
  }

  // Ask if the user wants logs to be stored in a file

  // Proceed with the rr algo here
}
