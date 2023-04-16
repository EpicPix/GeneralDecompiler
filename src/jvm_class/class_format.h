#pragma once

#include <stdint.h>
#include "../arch/arch.h"
#include "../utils.h"
#include "class_format_loaded.h"

#define ARCH_LOG(format, ...) DEBUG_LOG("[jvmclass] " format, __VA_ARGS__)