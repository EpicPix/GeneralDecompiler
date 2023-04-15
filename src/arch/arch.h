#pragma once

#include <stdint.h>
#include "../ir/IR.h"

struct arch_prepared_data {
  void* prepared_data;
  void* details_data;
};

typedef void* (*arch_load_data_func)(uint8_t* code, int code_length);
typedef struct arch_prepared_data (*arch_prepare_data_func)(void* loaded_data);
typedef struct ir_data* (*arch_generate_ir_func)(void* prepared_data);

typedef struct arch_info {
  arch_load_data_func load_data;
  arch_prepare_data_func prepare_data;
  arch_generate_ir_func generate_ir;
} arch_info;

const arch_info* arch_get(const char* name);

extern const arch_info arch_jvm;