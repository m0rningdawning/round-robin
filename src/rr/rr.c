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
    (*processes)[i].e_status = READY;
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
    *processes = read_proc_file("./proc.txt", count * sizeof(s_process));

    if (*processes == NULL) {
      fprintf(stderr, "Failed to read processes from file\n");
      exit(EXIT_FAILURE);
    }
  } else {
    uint8_t is_quant_static;
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
      is_quant_static = 1;
      puts("Enter the quantum time (1 - 20): ");
    } else {
      is_quant_static = 0;
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

// Per-process "wait for arrival" task
void *process_task(s_process *process) {
  while (1) {
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

// Task for initial queue population on arrival
void *populate_arrival_task(void *arg) {
  rr_thread_args *args = (rr_thread_args *)arg;

  uint32_t proc_left = *args->num_processes;
  pthread_t *threads = malloc((*args->num_processes) * sizeof(pthread_t));

  for (int i = 0; i < *args->num_processes; ++i) {
    pthread_create(&threads[i], NULL, (void *(*)(void *))process_task,
                   &args->processes[i]);
  }

  while (proc_left > 0) {
    for (int i = 0; i < *args->num_processes; ++i) {
      pthread_mutex_lock(&args->processes[i].m_lock);

      if (args->processes[i].e_status == READY) {
        push_back(args->r_queue, &args->processes[i]);
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

void *inc_tcompl_task(void *arg) {
  rr_thread_args *args = (rr_thread_args *)arg;
  while (!args->inc_thread_done) {
    pthread_mutex_lock(args->m_inc_ct);
    pthread_cond_wait(args->con_inc_ct, args->m_inc_ct);
    if (args->inc_thread_done) {
      pthread_mutex_unlock(args->m_inc_ct);
      break;
    }
    for (int i = 0; i < *args->num_processes; ++i) {
      pthread_mutex_lock(&args->processes[i].m_lock);
      if (args->processes[i].e_status != FINISHED) {
        args->processes[i].t_completion++;
      }
      pthread_mutex_unlock(&args->processes[i].m_lock);
    }
    pthread_mutex_unlock(args->m_inc_ct);
  }
  return NULL;
}

// Main schedule/reschedule task
void *schedule_reschedule(void *arg) {
  rr_thread_args *args = (rr_thread_args *)arg;

  static int p_finished = 0;
  static pthread_mutex_t finish_mutex = PTHREAD_MUTEX_INITIALIZER;

  while (1) {
    pthread_mutex_lock(&args->r_queue->m_lock);
    if (args->r_queue->size != 0) {
      s_process *proc = pop(args->r_queue);
      proc->e_status = RUNNING;
      pthread_mutex_unlock(&args->r_queue->m_lock);

      for (int i = 0; i < proc->quantum; ++i) {
        usleep(10000);

        // Completion time ++ on each tick
        pthread_mutex_lock(args->m_inc_ct);
        pthread_cond_signal(args->con_inc_ct);
        pthread_mutex_unlock(args->m_inc_ct);

        if (proc->t_burst > 0) {
          proc->t_burst--;
        } else {
          proc->e_status = FINISHED;
          break;
        }
      }

      if (proc->e_status == FINISHED) {
        pthread_mutex_lock(&finish_mutex);
        p_finished++;
        if (p_finished == *args->num_processes) {
          pthread_mutex_unlock(&finish_mutex);
          break;
        }

        pthread_mutex_unlock(&finish_mutex);
      } else {
        proc->e_status = WAITING;
        push_back(args->r_queue, proc);
      }

    } else {
      pthread_mutex_unlock(&args->r_queue->m_lock);
      // Sleep for a tick 'till a process arrives
      usleep(10000);
    }
  }

  return NULL;
}

// Round robin algo with constant or variable quantum, variable arrival time and
// multithreading (ready queue population and scheduling)
void round_robin(s_process *processes, uint32_t *num_processes,
                 ready_queue *r_queue) {
  // Arival and scheduling threads
  pthread_t arr_thread;
  pthread_t sched_thread;
  // Incrementation thread + its lock and incrementation condition
  pthread_t inc_thread;
  pthread_mutex_t *m_inc_ct = malloc(sizeof(pthread_mutex_t));
  pthread_cond_t *con_inc_ct = malloc(sizeof(pthread_cond_t));
  pthread_mutex_init(m_inc_ct, NULL);
  pthread_cond_init(con_inc_ct, NULL);

  // Arguments to pass inside the thread
  rr_thread_args *args = malloc(sizeof(rr_thread_args));
  args->processes = processes;
  args->num_processes = num_processes;
  args->r_queue = r_queue;
  args->m_inc_ct = m_inc_ct;
  args->con_inc_ct = con_inc_ct;
  args->inc_thread_done = 0;

  // Thread init and exec
  pthread_create(&arr_thread, NULL, (void *(*)(void *))populate_arrival_task, args);
  pthread_create(&sched_thread, NULL, (void *(*)(void *))schedule_reschedule, args);
  pthread_create(&inc_thread, NULL, inc_tcompl_task, args);

  // Total cleanup
  pthread_join(arr_thread, NULL);
  pthread_join(sched_thread, NULL);
  args->inc_thread_done = 1;
  pthread_cond_signal(con_inc_ct);
  pthread_join(inc_thread, NULL);

  pthread_mutex_destroy(m_inc_ct);
  pthread_cond_destroy(con_inc_ct);
  free(args);
  free(m_inc_ct);
  free(con_inc_ct);

  pthread_mutex_destroy(&(*r_queue).m_lock);
  for (int i = 0; i < *num_processes; ++i) {
    pthread_mutex_destroy(&processes[i].m_lock);
  }
}
