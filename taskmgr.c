#include "header.h"

int release_job(int cur_time)
{
  int task_id = 0;
  for (task_id = 0; task_id < num_of_tasks; task_id++)
  {
    if (taskset[task_id].release < cur_time)
    {
      printf("sync error in release at %d: %d\n", cur_time, taskset[task_id].deadline);
      return -1;
    }
  
    if (taskset[task_id].release == cur_time)
    {
      taskset[task_id].release += taskset[task_id].period;
      pid_t bench_pid = fork();
      if (bench_pid > 0)
      {
        taskset[task_id].pid = bench_pid;
        runtime_stat(0, task_id, cur_time);
      }
      else if (bench_pid == 0)
      {
        int output_file = open("/dev/null", O_WRONLY | O_TRUNC | O_CREAT);
        dup2(output_file, 1);
        
        cpu_set_t cpu_mask;
        CPU_ZERO(&cpu_mask);
        CPU_SET(taskset[task_id].cpu, &cpu_mask);
        sched_setaffinity(0, sizeof(cpu_set_t), &cpu_mask);
    
        struct sched_param prio;
        prio.sched_priority= taskset[task_id].priority;
        sched_setscheduler(0, SCHED_FIFO, &prio);

        execv(taskset[task_id].executable, taskset[task_id].args);
      }
      else if (bench_pid < 0)
      {
        printf("Cannot create process %d at time %d\n", task_id, cur_time);
        return -1;
      }
    }
  }
  return 0;
}

int complete_job(int cur_time)
{
//  printf("cur_time: %d\n", cur_time);
  int task_id = 0;
  for (task_id = 0; task_id < num_of_tasks; task_id++)
  {
    int status;
    if (taskset[task_id].deadline <= cur_time)
    {
      taskset[task_id].deadline += taskset[task_id].period;
      taskset[task_id].completed += 1;
      kill(taskset[task_id].pid, SIGKILL);
      waitpid(taskset[task_id].pid, &status, 0);
      if (WIFSIGNALED(status))
        taskset[task_id].miss += 1;
      runtime_stat(1, task_id, cur_time);
    }
    else if (taskset[task_id].deadline < cur_time)
    {
      printf("sync error in completion at %d: %d\n", cur_time, taskset[task_id].deadline);
      return -1;
    }
  }
  return 0;
}


