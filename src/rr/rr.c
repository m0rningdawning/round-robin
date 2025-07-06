#include "rr.h"

#include "../utils/file.h"
#include "../utils/utils.h"
#include "proc.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// - After the RR is implemented, add the possibility to optionally save logs to
// the file
// - Remake the sleep on every thread -> sleep on the main + threads on aux

void init_rr_manual(s_process **processes, uint32_t *num_processes,
                    ready_queue **r_queue) {
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
  char *ans;
  do {
    ans = get_string();
  } while (ans[0] != 'y' && ans[0] != 'n');

  if (ans[0] == 'y') {
    write_proc_file("./proc.txt", *processes, *num_processes);
  }
  free(ans);

  // Ask if the user wants logs to be stored in a file

  // Sorting the proc after saving to the file to avoid the sorting worst case
  // scenario on the file load
  qsort(*processes, *num_processes, sizeof(s_process), comp_proc_arrv);

  // Init the locks (Destroy afterwards!!)
  for (int i = 0; i < *num_processes; ++i) {
    pthread_mutex_init(&(*processes)[i].m_lock, NULL);
  }

  // Init the ready queue
  *r_queue = malloc(sizeof(ready_queue));
  init_queue(*r_queue, *processes, *num_processes);
}

void init_rr_automatic(s_process **processes, uint32_t *num_processes,
                       ready_queue **r_queue) {
  puts("Do you want to load the processes from the file? [y/n]");
  char *ans;

  do {
    ans = get_string();
  } while (ans[0] != 'y' && ans[0] != 'n');

  if (ans[0] == 'y') {
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

    if (*processes == NULL) {
      fprintf(stderr, "Failed to read processes from file\n");
      exit(EXIT_FAILURE);
    }
  } else {
    uint8_t is_quant_static;
    uint32_t quantum, t_arrival_r, t_burst_r;

    // Ask for the amount of processes
    puts("Enter the amount of processes to generate (1 - 50):");
    do {
      *num_processes = get_int(0);
    } while (*num_processes <= 0 || *num_processes >= 51);

    // Ask for quantum type
    puts("Do you want the quantum type be constant or randomly generated in a "
         "certain range? [y/n]");
    do {
      ans = get_string();
    } while (ans[0] != 'y' && ans[0] != 'n');

    if (ans[0] == 'y') {
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
  free(ans);

  // Ask if the user wants logs to be stored in a file

  // Sorting the proc after saving to the file to avoid the sorting worst case
  // scenario on the file load
  qsort(*processes, *num_processes, sizeof(s_process), comp_proc_arrv);

  // Init the locks (Destroy afterwards!!)
  for (int i = 0; i < *num_processes; ++i) {
    pthread_mutex_init(&(*processes)[i].m_lock, NULL);
  }

  // Init the ready queue
  *r_queue = malloc(sizeof(ready_queue));
  init_queue(*r_queue, *processes, *num_processes);
}

// A per-process "wait for arrival" task
void *process_task(s_process *process) {
  while (1) {
    sleep(1);

    pthread_mutex_lock(&process->m_lock);
    if (process->t_arrival > 0) {
      process->t_arrival--;
    }

    if (process->t_arrival == 0) {
      process->e_status = READY;
      pthread_mutex_unlock(&process->m_lock);
      break;
    }

    pthread_mutex_unlock(&process->m_lock);
  }

  return NULL;
}

// A task for initial queue population on arrival
void *populate_arrival(void *arg) {
  rr_thread_args *args = (rr_thread_args *)arg;

  uint32_t proc_left = *args->num_processes;
  pthread_t *threads = malloc((*args->num_processes) * sizeof(pthread_t));

  for (int i = 0; i < *args->num_processes; ++i) {
    pthread_create(&threads[i], NULL, (void *(*)(void *))process_task,
                   &args->processes[i]);
  }

  while (proc_left > 0) {
    sleep(1);

    for (int i = 0; i < *args->num_processes; ++i) {
      pthread_mutex_lock(&args->processes[i].m_lock);

      if (args->processes[i].e_status == READY) {
        push_back(args->r_queue, args->processes[i]);
        args->processes[i].e_status = WAITING;
        proc_left--;
      }

      pthread_mutex_unlock(&args->processes[i].m_lock);
    }
  }

  for (int i = 0; i < *args->num_processes; ++i) {
    pthread_join(threads[i], NULL);
  }
  free(threads);

  return NULL;
}

// Main schedule/reschedule task
void *schedule_reschedule(void *arg) { return NULL; }

// Round robin algo with constant quantum (for now), variable arrival time and
// multithreading for tasks
void round_robin(s_process *processes, uint32_t *num_processes,
                 ready_queue *r_queue) {
  pthread_t arr_thread;
  // pthread_t sched_thread;

  rr_thread_args *args = malloc(sizeof(rr_thread_args));
  args->processes = processes;
  args->num_processes = num_processes;
  args->r_queue = r_queue;

  pthread_create(&arr_thread, NULL, (void *(*)(void *))populate_arrival, args);
  // pthread_create(&scheed_thread, NULL, (void *(*)(void
  // *))schedule_reschedule,...);

  pthread_join(arr_thread, NULL);
  free(args);
  // pthread_join(sched_thread, NULL);
}
