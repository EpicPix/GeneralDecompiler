#include "utils.h"
#include <time.h>

struct timespec app_start_time;

void init_app_time() {
  clock_gettime(CLOCK_MONOTONIC, &app_start_time);
}

double get_app_time() {
  struct timespec current_time;
  clock_gettime(CLOCK_MONOTONIC, &current_time);
  double start = (double) app_start_time.tv_sec + (double) app_start_time.tv_nsec / 1000000000.;
  double end = (double) current_time.tv_sec + (double) current_time.tv_nsec / 1000000000.;
  return end - start;
}