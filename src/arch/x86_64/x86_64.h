#pragma once

#include <stdint.h>

#include "../arch.h"
#include "../../utils.h"

#define ARCH_LOG(format, ...) DEBUG_LOG("x86_64", format, __VA_ARGS__)