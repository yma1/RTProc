#include "header.h"

int scaling_fre(int set_fre_lvl)
{
  if (set_fre_lvl < 0)
    set_fre_lvl = 0;
  if (set_fre_lvl > 11)
    set_fre_lvl = 11;

  int core_index = 0;
  FILE *fre_file;
  char *fre_path = (char *)malloc(sizeof(char) * 128);
  char *read_buf = (char *)malloc(sizeof(char) * 16);
  for (core_index = 0; core_index < CORES; core_index++)
  {
    sprintf(fre_path, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_setspeed", core_index);
    fre_file = fopen(fre_path, "w");
    if (fre_file == NULL)
    {
      printf("cannot open core's frequency file\n");
      return -1;
    }
    fprintf(fre_file, "%s", fre_list[set_fre_lvl]);
    fclose(fre_file);

    sprintf(fre_path, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", core_index);
    fre_file = fopen(fre_path, "r");
    fgets(read_buf, sizeof(char) * 16, fre_file);
    read_buf[strlen(read_buf) - 1] = '\0';
    fclose(fre_file);
    
    if (strcmp(read_buf, fre_list[set_fre_lvl]) != 0)
    {
      printf("failed to set core %d's frequency: %s, %s\n", core_index, read_buf, fre_list[set_fre_lvl]);
      return -1;
    }
  }  
  free(fre_path);
  free(read_buf);
  return set_fre_lvl;
}

int opt_solver(int cur_fre_lvl)
{
  int min_util_diff = 400;
  int util_diff = 0;
  int optimal_fre = 0;

  int fre_lvl = 0;
  int core_index = 0;
  int exp_util;
  for (fre_lvl = 0; fre_lvl < 12; fre_lvl++)
  {
    util_diff = 0;
    for (core_index = 1; core_index < CORES; core_index++)
    {
      exp_util = (int)(util_core[core_index] * (float)(atoi(fre_list[cur_fre_lvl]))/atoi(fre_list[fre_lvl]));
      util_diff += abs(TEMP_SET_POINT - exp_util);
    }
    if (util_diff < min_util_diff)
    {
      min_util_diff = util_diff;
      optimal_fre = fre_lvl;
    }
  }
  
//  printf("core 1: %d, core 2: %d, core 3: %d\n", util_core[1], util_core[2], util_core[3]);
//  printf("%s\n", fre_list[optimal_fre]);
  return optimal_fre;
}

int DVFS()
{
#ifdef TA
  float temperature = capture_temperature();
  if (temperature > TEMP_SET_POINT)
    fre_lvl = scaling_fre(fre_lvl - 1);
  else
    fre_lvl = scaling_fre(fre_lvl + 1);
#else
  fre_lvl = opt_solver(fre_lvl);
//  fre_lvl = scaling_fre(fre_lvl);
#endif
  int core_index = 0;
  for (core_index = 0; core_index < CORES; core_index++)
  {
    util_core[core_index] = 0;
  }

  if (fre_lvl == -1)
  {
    printf("error in DVFS\n");
    return -1;
  }
  return 0;
}

int sampling_window_scaling(float cur_r)
{
  if (reliability_index == 0)
  {
    previous_mttf = cur_r;
    length_samp_window = (MAX_SAMPLING_WINDOW_LENGTH + MIN_SAMPLING_WINDOW_LENGTH) / 2;
  }
  else if (upper_bound_sw - lower_bound_sw > 1)
  {
    if (cur_r < previous_mttf)
      upper_bound_sw = length_samp_window;
    else if (cur_r >= previous_mttf)
      lower_bound_sw = length_samp_window;
    length_samp_window = (upper_bound_sw + lower_bound_sw) / 2;
    previous_mttf = cur_r;
  }
  printf("length_sampling_window: %d\n", length_samp_window);
  return 0;
}
