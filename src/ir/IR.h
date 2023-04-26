#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "IR_high_level.h"
#include "IR_low_level.h"
#include "IR_type.h"
#include "IR_symbol.h"

struct ir_memory_page {
  uint64_t start_address;
  uint64_t length;
  uint8_t* data;
  struct ir_memory_page* next_page;
};

struct ir_instruction_list {
  uint64_t start_address;
  uint64_t instruction_count;
  uint64_t allocated_count;
  union {
    struct ir_instruction_low* low_level;
    struct ir_instruction_high* high_level;
  } instructions;
  struct ir_instruction_list* next;
};

struct ir_instruction_list* ir_instruction_create_list(struct ir_instruction_list* prev, uint64_t start_address, uint64_t instruction_count, bool high_level);

struct ir_data {
  struct ir_symbol_table* symbol_table;
  struct ir_type_table* type_table;
  struct ir_memory_page* memory_page_start;
  struct ir_instruction_list* instructions;
  bool is_high_level;
};

struct ir_data ir_optimize(struct ir_data data);
void ir_print_instructions(struct ir_data data);
void ir_print_decompiled(struct ir_data data);
