#pragma once

#ifdef DEBUG

#include <stdio.h>
#define DEBUG_LOG(TAG, FORMAT, ...) printf("%s:%d: [" TAG "] " FORMAT "\n", __FILE__, __LINE__, __VA_ARGS__)

#else

#define DEBUG_LOG(TAG, format, args...)

#endif