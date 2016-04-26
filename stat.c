#include "header.h"

void runtime_stat(int mode, int task_id, int cur_time)
{
  if (mode == 0)
  {
    fprintf(stat_file, "%d releases at time: %d\n", task_id, cur_time);
    fprintf(stat_file, "\t\tpid: %d, priority: %d, cpu: %d\n", taskset[task_id].pid, taskset[task_id].priority, taskset[task_id].cpu);
  }
  else
  {
    fprintf(stat_file, "%d completes at time: %d\n", task_id, cur_time);
    fprintf(stat_file, "\t\tcompleted: %d, miss: %d\n", taskset[task_id].completed, taskset[task_id].miss);
  }
}
