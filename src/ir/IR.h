#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "IR_high_level.h"
#include "IR_low_level.h"

struct ir_memory_page {
  uint64_t start_address;
  uint64_t length;
  uint8_t* data;
  struct ir_memory_page* next_page;
};

struct ir_symbol_table {

};

struct ir_data {
  struct ir_symbol_table* symbol_table;
  struct ir_memory_page* memory_page_start;
};

struct ir_data ir_optimize(struct ir_data data);
void ir_print_instructions(struct ir_data data);
void ir_print_decompiled(struct ir_data data);
