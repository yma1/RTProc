#include "header.h"

int *before_exe;
int *after_exe;

int capure_exe_cores(int *exe_core)
{
  char *read_buffer = (char *)malloc(sizeof(char) * 128);
  FILE *read_cpu = fopen("/proc/stat", "r");
  if (read_cpu == NULL)
  {
    printf("Cannot open /proc/stat");
    return -1;
  }
  fgets(read_buffer, sizeof(char) * 128, read_cpu);

  char *str_user_time;
  int int_user_time;
  char *str_nice_time;
  int int_nice_time;
  char *str_sys_time;
  int int_sys_time;
  char *str_idle_time;
  int int_idle_time;
  char *str_iowait_time;
  int int_iowait_time;
  char *str_irq_time;
  int int_irq_time;
  char *str_softirq_time;
  int int_softirq_time;

  int total;
  int i = 0;
  for (i = 0; i < CORES; i++)
  {
    fgets(read_buffer, sizeof(char) * 128, read_cpu);

    strtok(read_buffer, " ");
    str_user_time = strtok(0, " ");
    str_nice_time = strtok(0, " ");
    str_sys_time = strtok(0, " ");
    str_idle_time = strtok(0, " ");
    str_iowait_time = strtok(0, " ");
    str_irq_time = strtok(0, " ");
    str_softirq_time = strtok(0, " ");

    int_user_time = atoi(str_user_time);
    int_nice_time = atoi(str_nice_time);
    int_sys_time = atoi(str_sys_time);
    int_idle_time = atoi(str_idle_time);
    int_iowait_time = atoi(str_iowait_time); 
    int_irq_time = atoi(str_irq_time); 
    int_softirq_time = atoi(str_softirq_time); 

    total = int_user_time + int_nice_time + int_sys_time + int_iowait_time + int_irq_time + int_softirq_time;
    exe_core[i] = total;
  }
  return 0;
}

int cap_util()
{
  char core_index = 0;
  int *temp;

  if (capure_exe_cores(after_exe) == -1)
    return -1;

  for (core_index = 0; core_index < CORES; core_index++)
    util_core[core_index] += (after_exe[core_index] - before_exe[core_index]);

  temp = before_exe;
  before_exe = after_exe;
  after_exe = temp;

  return 0;
}

void init_util()
{
  before_exe = (int *)malloc(sizeof(int) * CORES);
  after_exe = (int *)malloc(sizeof(int) * CORES);
  capure_exe_cores(before_exe);
}





