#pragma once
#include <stdint.h>

typedef void* (*arch_disassemble_func)(uint8_t* code, int code_length);

typedef struct arch_info {
  arch_disassemble_func disassemble;
} arch_info;

const arch_info* arch_get(const char* name);
