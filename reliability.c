#include "header.h"

float capture_temperature()
{

  FILE *thermal_sensor = fopen("/sys/devices/virtual/thermal/thermal_zone0/temp", "r");
  if (thermal_sensor == NULL)
  {
    perror("cannot open thermal sensor file\n");
    return -1;
  }
  
  char *read_buf = (char *)malloc(sizeof(char) * 8);
  fgets(read_buf, sizeof(char) * 8, thermal_sensor);
  float temperature = atof(read_buf) / 1000; 
  return temperature;
}


int gen_rel_file()
{
  FILE *rel_file = fopen("./rel_input.txt", "w");
  if (rel_file == NULL)
  {
    perror("cannot create reliability input file\n");
    return -1;
  }
  fprintf(rel_file, "l 5.4 0 0 0\n");
  fprintf(rel_file, "w 5 0.25 0.25 0.25\n");
  fprintf(rel_file, "w 5 0.25 0.25 0.25\n");
  fprintf(rel_file, "w 5 0.25 0.25 0.25\n");
  fprintf(rel_file, "t 5\n");
  fprintf(rel_file, "40.7587 120 1e12 120 1.0 110 1e09 5000 0.25 0.25 0.25 0.25\n");
  fprintf(rel_file, "s 1111\n");

  int comp_index = 0;
  int temp_point_index = 0;
  for (comp_index = 0; comp_index < CORES; comp_index++)
  {
    fprintf(rel_file, "component %d\n", comp_index);
    for (temp_point_index = 0; temp_point_index < NUM_OF_SAMPLING_WINDOW; temp_point_index++)
      fprintf(rel_file, "%d %f 1.0 1e09\n", length_samp_window, temperature_profile[temp_point_index]);
  }
  fclose(rel_file);
  return 0;
}

float get_reliability()
{
  FILE *rel_file = fopen("./rel_output.txt", "r");
  if (rel_file == NULL)
  {
    perror("cannot open reliability output file\n");
    return -1;
  }
  char *read_buf = (char *)malloc(sizeof(char) * 128);
  fgets(read_buf, sizeof(char) * 128, rel_file);
  char *mttf_str = strtok(read_buf, "\t");
  float mttf = atof(mttf_str);
  printf("mttf: %f\n", mttf);
  return mttf;

}


