#include "header.h"

void sig_cap_r(int);

void print_conf(void)
{
  int task_id = 0;
  for (task_id = 0; task_id < num_of_tasks; task_id++)
  {
    printf("name: %s\n", taskset[task_id].name);
    printf("cpu: %d\n", taskset[task_id].cpu);
    printf("period: %d\n", taskset[task_id].period);
    printf("deadline: %d\n", taskset[task_id].deadline);
    printf("release: %d\n", taskset[task_id].release);
  }
}

int check_conf(void)
{
  int task_id = 0;
  for (task_id = 0; task_id < num_of_tasks; task_id++)
  {
    if ((taskset[task_id].release < 0) || (taskset[task_id].deadline < 0) || (taskset[task_id].period < 0))
    {
      printf("error in configurations is checked\n");
      return -1;
    }
    if ((taskset[task_id].cpu < 0) || (taskset[task_id].cpu > 3))
    {
      printf("error in cpu is checked\n");
      return -1;
    }
  }
  return 0;
}

int set_args(struct task *cur_task)
{
  if (strcmp("susan", cur_task->name) == 0)
  {
    cur_task->args = susan_args;
    cur_task->executable = "./taskset/susan/susan";
  }
  else if (strcmp("qsort", cur_task->name) == 0)
  {
    cur_task->args = qsort_args;
    cur_task->executable = "./taskset/qsort/qsort_large";
  }
  else if (strcmp("blowfish", cur_task->name) == 0)
  {
    cur_task->args = blowfish_args;
    cur_task->executable = "./taskset/blowfish/bf";
  }
  else if (strcmp("cjpeg", cur_task->name) == 0)
  {
    cur_task->args = cjpeg_args;
    cur_task->executable = "./taskset/cjpeg/cjpeg";
  }
  else if (strcmp("CRC32", cur_task->name) == 0)
  {
    cur_task->args = crc_args;
    cur_task->executable = "./taskset/CRC32/crc";
  }
  else if (strcmp("dijkstra", cur_task->name) == 0)
  {
    cur_task->args = dijkstra_args;
    cur_task->executable = "./taskset/dijkstra/dijkstra_large";
  }
  else if (strcmp("patricia", cur_task->name) == 0)
  {
    cur_task->args = patricia_args;
    cur_task->executable = "./taskset/patricia/patricia";
  }
  else if (strcmp("sha", cur_task->name) == 0)
  {
    cur_task->args = sha_args;
    cur_task->executable = "./taskset/sha/sha";
  }
  else if (strcmp("stringsearch", cur_task->name) == 0)
  {
    cur_task->args = stringsearch_args;
    cur_task->executable = "./taskset/stringsearch/search_large";
  }
  else if (strcmp("bitcount", cur_task->name) == 0)
  {
    cur_task->args = bitcount_args;
    cur_task->executable = "./taskset/bitcount/bitcnts";
  }
  else
  {
    printf("unknown app name: %s\n", cur_task->name);
    return -1;
  }

  return 0;
}

int init_name(char *value)
{
  int task_id = 0;
  for (task_id = 0; task_id < num_of_tasks; task_id++)
    if (taskset[task_id].name == NULL)
      break;
  if (task_id == num_of_tasks)
  {
    printf("Error in multiple names\n");
    return -1;
  }
  taskset[task_id].name = (char *)malloc(sizeof(char) * 32);
  strncpy(taskset[task_id].name, value, strlen(value) - 1);
  taskset[task_id].completed = 0;
  taskset[task_id].miss = 0;
  taskset[task_id].period = -1;
  taskset[task_id].deadline = -1;
  taskset[task_id].release = -1;
  if (set_args(&taskset[task_id]) == -1)
    return -1;
  return task_id;
}

void init_cpu(int task_id, char *value)
{
  taskset[task_id].cpu = atoi(value);
}

void init_release(int task_id, char *value)
{
  taskset[task_id].release = cur_time + atoi(value);
}

void init_deadline(int task_id, char *value)
{
  taskset[task_id].deadline = cur_time + atoi(value);
}

void init_period(int task_id, char *value)
{
  taskset[task_id].period = atoi(value);
}

void init_priority(int task_id, char *value)
{
  taskset[task_id].priority = atoi(value);
}

int conf(char *conf_file)
{
  FILE *fd_conf_file = fopen(conf_file, "r");
  if (fd_conf_file == NULL)
  {
    perror("Error opening file");
  }

  char *key;
  char *value;
  char *read_buf = (char *)malloc(sizeof(char) * 128);  
  fgets(read_buf, 128, fd_conf_file);
  while ((read_buf[0] == '#') || (read_buf[0] == '\n'))
    fgets(read_buf, 128, fd_conf_file);

  key = strtok(read_buf, ":");
  value = strtok(NULL, ":");
  if (strcmp(key, "number_of_tasks"))
  {
    printf("the first configuration must be the number of tasks\n");
    return -1;
  }
  num_of_tasks = atoi(value);
  taskset = (struct task *)malloc(sizeof(struct task) * num_of_tasks);  

  int task_id = -1;
  while (fgets(read_buf, 128, fd_conf_file) != NULL)
  {
    while ((read_buf[0] == '#') || (read_buf[0] == '\n'))
      fgets(read_buf, 128, fd_conf_file);
    
    key = strtok(read_buf, ":");
    value = strtok(NULL, ":");

    if (strcmp(key, "name") == 0)
    {
      task_id = init_name(value);
      if (task_id == -1)
        return -1;
    }
    else if (strcmp(key, "cpu") == 0)
      init_cpu(task_id, value);
    else if (strcmp(key, "release") == 0)
      init_release(task_id, value);
    else if (strcmp(key, "deadline") == 0)
      init_deadline(task_id, value);
    else if (strcmp(key, "period") == 0)
      init_period(task_id, value);
    else if (strcmp(key, "priority") == 0)
      init_priority(task_id, value);
    else
    {
      printf("Unknown paramter: %s\n", key);
      return -1;
    }
  }

  fclose(fd_conf_file);

  print_conf();

  if (check_conf() != 0)
    return -1;
  return 0;
}

int init_sched_affinity()
{
/*
  cpu_set_t cpu_mask;
  CPU_ZERO(&cpu_mask);
  CPU_SET(0, &cpu_mask);
  if (sched_setaffinity(0, sizeof(cpu_set_t), &cpu_mask) == -1)
  {
    printf("error to set master's affinity\n");
    return -1;
  } 
  struct sched_param prio;
  prio.sched_priority= 90;
  if (sched_setscheduler(0, SCHED_FIFO, &prio) == -1)
  {
    printf("error to set master's priority\n");
    return -1;
  }
*/
  return 0;
}


int init(char *conf_path)
{
  if (conf(conf_path) != 0)
    return -1;

  if (init_sched_affinity() != 0)  
    return -1;

  rel_pid = 0;
  fre_lvl = 0;
  length_samp_window = MIN_SAMPLING_WINDOW_LENGTH;
  temperature_profile = (float *)malloc(sizeof(float) * NUM_OF_SAMPLING_WINDOW);
  window_index = 0;
  reliability_index = 0;  
  reliability = (float *)malloc(sizeof(float) * TRACE_OF_RELIABILITY);
  upper_bound_sw = MAX_SAMPLING_WINDOW_LENGTH;
  lower_bound_sw = MIN_SAMPLING_WINDOW_LENGTH;
  previous_mttf = 0;

  signal(SIGUSR1, sig_cap_r);

  init_util();

  return 0;
}





















