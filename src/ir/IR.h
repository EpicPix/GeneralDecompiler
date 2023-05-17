#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "IR_high_level.h"
#include "IR_low_level.h"
#include "IR_collapsed_level.h"
#include "IR_type.h"
#include "IR_symbol.h"

enum ir_instruction_level {
    ir_instruction_level_high,
    ir_instruction_level_low,
    ir_instruction_level_collapsed,
};

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

struct ir_instruction_list* ir_instruction_create_temp_list(enum ir_instruction_level level);
struct ir_instruction_list* ir_instruction_create_list(struct ir_instruction_list* prev, uint64_t start_address, uint64_t instruction_count, enum ir_instruction_level level);
void ir_instruction_destroy_list(struct ir_instruction_list* instructions, enum ir_instruction_level level);
struct ir_instruction_list* ir_instruction_move_list(struct ir_instruction_list* new_list, struct ir_instruction_list* old_list, enum ir_instruction_level level);
struct ir_instruction_list* ir_instruction_move_and_destroy_list(struct ir_instruction_list* new_list, struct ir_instruction_list* old_list, enum ir_instruction_level level);

struct ir_data {
  struct ir_symbol_table* symbol_table;
  struct ir_type_table* type_table;
  struct ir_memory_page* memory_page_start;
  struct ir_instruction_list* instructions;
  enum ir_instruction_level instruction_level;
};

struct ir_data ir_lower_level(struct ir_data data);
struct ir_data ir_optimize(struct ir_data data);
struct ir_data ir_collapse(struct ir_data data);
void ir_print_instructions(struct ir_data data);
void ir_decompiled_print(struct ir_data data);
