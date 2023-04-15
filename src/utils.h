#pragma once

#ifdef DEBUG

#include <stdlib.h>
#define DEBUG_LOG(format, ...) printf("%s:%d: " format "\n", __FILE__, __LINE__, __VA_ARGS__)

#else

#define DEBUG_LOG(format, args...)

#endif