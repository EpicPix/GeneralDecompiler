#pragma once

#include <stdint.h>
#include "../arch/arch.h"
#include "../utils.h"

struct jvm_class_loaded_file {
  uint16_t minor_version;
  uint16_t major_version;
};

#define ARCH_LOG(format, ...) DEBUG_LOG("[jvmclass] " format, __VA_ARGS__)