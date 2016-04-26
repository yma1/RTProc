//add
#include "header.h"

int main_alg()
{
  int core_index = 0;
  window_index += 1;

  //capture utilization at each second
  cap_util();

  //at the end of each sampling window
  if (window_index % length_samp_window == 0)
  {
    //capture the utilization at each sampling window
    for (core_index = 0; core_index < CORES; core_index++)
      util_core[core_index] /= length_samp_window;

    //capture temperature at sampling window
    temperature_profile[(window_index / length_samp_window - 1) % NUM_OF_SAMPLING_WINDOW] = capture_temperature();
    if (temperature_profile[(window_index / length_samp_window - 1) % NUM_OF_SAMPLING_WINDOW] == -1)
      return -1;

    //util_core is reset to 0 in DVFS function
    if (DVFS() == -1)
      return -1; 

    printf("temperature[%d]:%f  win_index:%d  lsw:%d  fre:%s  cur_time:%d\n", 
      (window_index / length_samp_window - 1) % NUM_OF_SAMPLING_WINDOW, 
      temperature_profile[(window_index / length_samp_window - 1) % NUM_OF_SAMPLING_WINDOW],
      window_index,
      length_samp_window,
      fre_list[fre_lvl],
      cur_time);
  }

  //at the end of each profiling window, calculate reliability then change the length of SW
  if ((window_index % (NUM_OF_SAMPLING_WINDOW * length_samp_window) == 0) && (cur_time > 30000))
  {
    printf("cur_time: %d, window_index: %d, fre_lvl: %d\n", cur_time, window_index, fre_lvl);
    window_index = 0;

    //generate reliability file
    if (gen_rel_file() == -1)
      return -1;

#ifndef TA
    if (rel_pid != 0)
    {
      kill(rel_pid, SIGKILL);
      int status;
      waitpid(rel_pid, &status, 0);
      if (WIFSIGNALED(status))
      {
        printf("error in rTool\n");
        return -1;
      }
    }  

    rel_pid = vfork();
    if (rel_pid == 0)
    {
      system("./rTool -r ./rel_input.txt > ./rel_output.txt");
      kill(getppid(), SIGUSR1);
      _exit(0);
    }
#endif

  }

  return 0;
}

void sig_cap_r(int signo)
{  
#ifndef TA
  if (signo == SIGUSR1)
  {
    //capture reliability
    reliability[reliability_index] = get_reliability();

    if (reliability[reliability_index] == -1)
      exit(-1);

    //change the length of sampling window
//    if (sampling_window_scaling(reliability[reliability_index]) == -1)
//      exit(-1);

    reliability_index += 1;
  }
#endif
}


int output()
{
  FILE *output_file = fopen(output_file_path, "w");
  if (output_file == NULL)
  {
    printf("cannot create output file\n");
    return -1;
  }

  int task_index;
  for (task_index = 0; task_index < num_of_tasks; task_index++)
    fprintf(output_file, "task id: %d, completed: %d, misses: %d\n", task_index, taskset[task_index].completed, taskset[task_index].miss);

  char *fre_path = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";
  char *read_buf = (char *)malloc(sizeof(char) * 16);
  FILE *fre_file = fopen(fre_path, "r");
  fgets(read_buf, sizeof(char) * 16, fre_file);
  fprintf(output_file, "frequency: %s", read_buf);

  fclose(output_file);
  fclose(fre_file);
  free(read_buf);
  return 0;
}

void timer_handler(int signo)
{
  cur_time += 1;
  if (cur_time == RUNTIME * 500)
  {
    fclose(stat_file);
 
    int i = 0;
    for(i = 0; i < num_of_tasks; i++)
    {
      waitpid(taskset[i].pid, NULL, 0);
    }
 
    if (output() != 0)
      exit(-1);
    else
      exit(0);
  }
/*
  if (cur_time % (10000 / TIMEUNIT) == 0)
    printf("cur_time: %d\n", cur_time);
*/
  if ((cur_time % (1000 / TIMEUNIT) == 0) && (main_alg() != 0))
    exit(-1);

  if (complete_job(cur_time) != 0)
    exit(-1);

  if (release_job(cur_time) != 0)
    exit(-1);
}

int main(int argc, char **argv)
{
  int argv_scan = 1;

  while (argv_scan < argc)
  {
    if (!strcmp(argv[argv_scan], "-o"))
      output_file_path = argv[argv_scan + 1];
    if (!strcmp(argv[argv_scan], "-c"))
      conf_file_path = argv[argv_scan + 1];
    if (!strcmp(argv[argv_scan], "-s"))
      stat_file_path = argv[argv_scan + 1];
    argv_scan += 2;
  }

  if (output_file_path == NULL)
    output_file_path = "./output.txt";
  if (conf_file_path == NULL)
    conf_file_path = "./config.txt";
  if (stat_file_path == NULL)
    stat_file_path = "./stat.txt";

  if (init(conf_file_path) != 0)
    return -1;

  stat_file = fopen(stat_file_path, "w");

  cur_time = 0;

  signal(SIGUSR2, timer_handler);
  timer_t timerid;
  struct sigevent sevp;
  sevp.sigev_notify = SIGEV_SIGNAL;
  sevp.sigev_signo = SIGUSR2; //raise signal SIGUSR1
  timer_create(CLOCK_REALTIME, &sevp, &timerid);

  struct timespec initial;
  struct timespec interval;
  initial.tv_sec = 1;
  initial.tv_nsec = 0;
  interval.tv_sec = 0;
  interval.tv_nsec = 1000000 * TIMEUNIT; //timer is 1*TIMEUNIT millisecond
  struct itimerspec new_value;
  new_value.it_interval = interval;
  new_value.it_value = initial;
  timer_settime(timerid, 0, &new_value, NULL);

  while(1);

  return 0;
}

