#include "rr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../utils/file.h"
#include "../utils/utils.h"
#include "proc.h"
#include "queue.h"

// - After the RR is implemented, add the possibility to optionally save logs to
// the file

void init_rr_manual(s_process **processes, uint32_t *num_processes,
                    ready_queue **r_queue) {
  printf("Enter number of processes: ");
  *num_processes = get_int(false);

  *processes = malloc((*num_processes) * sizeof(s_process));
  if (*processes == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < *num_processes; ++i) {
    printf("Proecss %d: \n", i);
    printf("Process %d PID: \n", i);
    (*processes)[i].pid = get_int(false);
    printf("Process %d Quantum: \n", i);
    (*processes)[i].quantum = get_int(false);
    printf("Process %d Arrival Time: \n", i);
    (*processes)[i].t_arrival = get_int(false);
    printf("Process %d Burst Time: \n", i);
    (*processes)[i].t_burst = get_int(false);
    (*processes)[i].t_completion = 0;
    (*processes)[i].t_turnaround = 0;
    (*processes)[i].t_waiting = 0;
    (*processes)[i].e_status = OTW;
  }

  puts(
      "Do you want to save the processes into the \"proc.txt\" file for "
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

  // Sort the processes (asc) by arrival time
  qsort(*processes, *num_processes, sizeof(s_process), comp_proc_arrv);

  // Init the ready queue
  *r_queue = malloc(sizeof(ready_queue));
  init_queue(*r_queue, *processes, *num_processes);

  // Init the locks
  pthread_mutex_init(&(*r_queue)->m_lock, NULL);
  for (int i = 0; i < *num_processes; ++i) {
    pthread_mutex_init(&(*processes)[i].m_lock, NULL);
  }
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
      if (strspn(buf, " \t\r\n") != strlen(buf)) count++;
    }
    fclose(f);

    *num_processes = count;
    *processes = read_proc_file("./proc.txt", count);

    if (*processes == NULL) {
      fprintf(stderr, "Failed to read processes from file\n");
      exit(EXIT_FAILURE);
    }
  } else {
    bool is_quant_static;
    uint32_t quantum, t_arrival_r, t_burst_r;

    puts("Enter the amount of processes to generate (1 - 50):");
    do {
      *num_processes = get_int(false);
    } while (*num_processes <= 0 || *num_processes >= 51);

    puts(
        "Do you want the quantum type be constant or randomly generated in a "
        "certain range? [y/n]");
    do {
      ans = get_string();
    } while (ans[0] != 'y' && ans[0] != 'n');

    if (ans[0] == 'y') {
      is_quant_static = true;
      puts("Enter the quantum time (1 - 20): ");
    } else {
      is_quant_static = false;
      puts("Enter the quantum time max value in range (1 - 20):");
    }

    // Ask for the quantum itself (if static - quant, if not - range)
    do {
      quantum = get_int(false);
    } while (quantum <= 0 || quantum >= 21);

    puts("Enter the arrival time range (1 - 20): ");
    do {
      t_arrival_r = get_int(false);
    } while (t_arrival_r <= 0 || t_arrival_r >= 21);

    puts("Enter the burst time range (1 - 20): ");
    do {
      t_burst_r = get_int(false);
    } while (t_burst_r <= 0 || t_burst_r >= 21);

    *processes = generate_proc(*num_processes, is_quant_static, quantum,
                               t_arrival_r, t_burst_r);
  }
  free(ans);

  // Ask if the user wants logs to be stored in a file

  // Sort the processes (asc) by arrival time
  qsort(*processes, *num_processes, sizeof(s_process), comp_proc_arrv);

  // Init the ready queue
  *r_queue = malloc(sizeof(ready_queue));
  init_queue(*r_queue, *processes, *num_processes);

  // Init the locks
  pthread_mutex_init(&(*r_queue)->m_lock, NULL);
  for (int i = 0; i < *num_processes; ++i) {
    pthread_mutex_init(&(*processes)[i].m_lock, NULL);
  }
}

// Task for initial queue population on arrival
void *populate_arrival_task(void *arg) {
  rr_thread_args *args = (rr_thread_args *)arg;

  // A dumb-ass workaround - essentially, cuz the time starts
  // to advance immediately, we need to check for the processes
  // once before the loop starts to catch all the t_arrival: 0 proc
  for (uint32_t i = 0; i < *args->num_processes; i++) {
    if (args->processes[i].t_arrival == 0 &&
        args->processes[i].e_status == OTW) {
      push_back(args->r_queue, &args->processes[i]);
      args->processes[i].e_status = WAITING;
    }
  }

  while (!args->sim_finished) {
    pthread_mutex_lock(args->m_tick);
    pthread_cond_wait(args->con_tick, args->m_tick);
    pthread_mutex_unlock(args->m_tick);

    if (args->sim_finished) break;

    pthread_mutex_lock(args->m_master);
    for (uint32_t i = 0; i < *args->num_processes; i++) {
      if (args->processes[i].t_arrival == *args->time &&
          args->processes[i].e_status == OTW) {
        args->processes[i].e_status = WAITING;
        push_back(args->r_queue, &args->processes[i]);
      }
    }
    pthread_mutex_unlock(args->m_master);
  }
  return NULL;
}

// Main schedule/reschedule task
void *schedule_reschedule(void *arg) {
  rr_thread_args *args = (rr_thread_args *)arg;
  s_process *current_proc = NULL;
  uint32_t p_finished = 0;
  uint32_t p_quant_passed = 0;

  while (p_finished < *args->num_processes) {
    pthread_mutex_lock(args->m_master);

    // A process finished its quantum or finished
    if (current_proc != NULL) {
      if (current_proc->t_burst == 0) {
        current_proc->e_status = FINISHED;
        current_proc->t_completion = *args->time;
        p_finished++;
        current_proc = NULL;
      } else if (p_quant_passed >= current_proc->quantum) {
        current_proc->e_status = WAITING;
        push_back(args->r_queue, current_proc);
        current_proc = NULL;
      }
    }

    // Check for arriving threads just in case
    pthread_mutex_lock(args->m_tick);
    pthread_cond_signal(args->con_tick);
    pthread_mutex_unlock(args->m_tick);

    if (current_proc == NULL) {
      current_proc = pop(args->r_queue);
      if (current_proc != NULL) {
        current_proc->e_status = RUNNING;
        p_quant_passed = 0;
      }
    }

    if (current_proc != NULL) {
      (*args->time)++;
      current_proc->t_burst--;
      p_quant_passed++;
    } else {
      (*args->time)++;
    }

    pthread_mutex_unlock(args->m_master);
    // To not lock up the populate arrival thread
    usleep(10000);
  }

  // Signal all threads to exit
  args->sim_finished = true;
  pthread_mutex_lock(args->m_tick);
  pthread_cond_signal(args->con_tick);
  pthread_mutex_unlock(args->m_tick);
  return NULL;
}

// Round robin algo with constant or variable quantum, variable arrival time and
// multithreading (ready queue population and scheduling)
void round_robin(s_process *processes, uint32_t *num_processes,
                 ready_queue *r_queue) {
  // Arival and scheduling threads
  pthread_t arr_thread;
  pthread_t sched_thread;

  // Shared resources for threads
  pthread_mutex_t m_tick = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t con_tick = PTHREAD_COND_INITIALIZER;
  pthread_mutex_t m_master = PTHREAD_MUTEX_INITIALIZER;
  volatile uint32_t time = 0;

  // Thread argument structure init
  rr_thread_args *args = malloc(sizeof(rr_thread_args));
  args->processes = processes;
  args->num_processes = num_processes;
  args->r_queue = r_queue;
  args->m_tick = &m_tick;
  args->con_tick = &con_tick;
  args->time = &time;
  args->sim_finished = false;
  args->m_master = &m_master;

  // Threads init and exec
  pthread_create(&arr_thread, NULL, (void *(*)(void *))populate_arrival_task, args);
  pthread_create(&sched_thread, NULL, (void *(*)(void *))schedule_reschedule, args);

  // Total cleanup
  pthread_join(arr_thread, NULL);
  pthread_join(sched_thread, NULL);

  pthread_mutex_destroy(&m_tick);
  pthread_cond_destroy(&con_tick);
  pthread_mutex_destroy(&m_master);
  free(args);

  pthread_mutex_destroy(&(*r_queue).m_lock);
  for (uint32_t i = 0; i < *num_processes; ++i) {
    pthread_mutex_destroy(&processes[i].m_lock);
  }
}

