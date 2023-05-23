#pragma once

#include <stdint.h>
#include "../arch.h"
#include "../../utils.h"

#include "class_format_loaded.h"
#include "class_format_prepared.h"

#define ARCH_LOG(format, ...) DEBUG_LOG("jvmclass", format, __VA_ARGS__)