#define _GNU_SOURCE

#include <stdio.h>
#include <sched.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

#define CORES 4
#define RUNTIME 300
#define NUM_OF_SAMPLING_WINDOW 30 //one profiling window has 100 sampling windows
#define	TRACE_OF_RELIABILITY 4048 //in total can save reliabilities in 4048 profiling windows
#define MAX_SAMPLING_WINDOW_LENGTH 4 //max length of sampling window
#define MIN_SAMPLING_WINDOW_LENGTH 1 //min length of sampling window
#define TIMEUNIT 2 //time unit is 2ms

//#define TA

#define TEMP_SET_POINT 40
#define UTIL_SET_POINT 55

struct task
{
  char *name;
  int cpu;
  int deadline;
  int release;
  int period;
  int priority;
  char *executable;
  char **args;
  pid_t pid;
  int completed;
  int miss;
};

int cur_time;
int num_of_tasks;
int window_index; //index of sampling window
int length_samp_window; //the length of sampling window
float *temperature_profile; //the temperature points inside a profiling window
struct task *taskset;
int util_core[CORES];
char *stat_file_path;
char *output_file_path;
char *conf_file_path;
FILE *stat_file; //a file saves the run-time statistics
FILE *temperature_file; //temperature profile generated at the end of each profiling window
int reliability_index; //index of reliability point
float *reliability; //trace the reliability in different profiling windows
int upper_bound_sw;
int lower_bound_sw;
float previous_mttf;
pid_t rel_pid;
int fre_lvl; //core's frequency level

static char *susan_args[] = {"susan", "./taskset/susan/input_large.pgm", "/dev/null",
"-s", NULL};
static char *qsort_args[] = {"qsort_large", "./taskset/qsort/input_large.dat", NULL};
static char *blowfish_args[] = {"bf", "e", "./taskset/blowfish/input_large.asc", "/dev/null",
"1234567890abcdeffedcba0987654321", NULL};
static char *cjpeg_args[] = {"cjpeg", "-dct", "int", "-progressive", "-opt", "-outfile", "/dev/null", "./taskset/cjpeg/input_small.ppm", NULL};
static char *crc_args[] = {"crc", "./taskset/CRC32/large.adpcm", NULL};
static char *dijkstra_args[] = {"dijkstra_large", "./taskset/dijkstra/input.dat", NULL};
static char *patricia_args[] = {"patricia", "./taskset/patricia/large.udp", NULL};
static char *stringsearch_args[] = {"search_large", "abc", NULL};
static char *sha_args[] = {"sha", "./taskset/sha/input_large.asc", NULL};
static char *bitcount_args[] = {"bitc	ount", "3000000", NULL};
static char *fre_list[] = {"1224000", "1326000", "1428000", "1530000", "1632000", "1734000", "1836000", "1938000", "2014500", "2116500", "2218500", "2320500"};

int init(char *);
void init_util();
int cap_util();
int DVFS();
float capture_temperature();
int gen_rel_file();
float get_reliability();
int sampling_window_scaling(float);
void runtime_stat(int, int, int);






















