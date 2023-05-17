#pragma once

#ifdef DEBUG

#include <stdio.h>
#define DEBUG_LOG(TAG, FORMAT, ...) printf("%s:%d: [" TAG "] [%.6lf] " FORMAT "\n", __FILE__, __LINE__, get_app_time(), __VA_ARGS__)

#else

#define DEBUG_LOG(TAG, format, args...)

#endif

#include <stdlib.h>
#include <stdio.h>

#define TODO(MESSAGE, ...) { printf(MESSAGE __VA_OPT__(,) __VA_ARGS__); exit(1); }

void init_app_time();
double get_app_time();