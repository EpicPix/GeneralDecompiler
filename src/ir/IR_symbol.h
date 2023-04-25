#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "IR_type.h"

enum ir_symbol_table_type {
  ir_symbol_table_type_function = 0,
  ir_symbol_table_type_field = 1,
  ir_symbol_table_type_variable = 2,
};

struct ir_symbol_table_data_function {
  ir_type_t return_value;
  int parameter_count;
  ir_type_t parameters[];
};

struct ir_symbol_table_element {
  uint64_t location;
  char* name;
  union {
    struct ir_symbol_table_data_function* func;
    ir_type_t field;
  } extra;
  enum ir_symbol_table_type type;
};

struct ir_symbol_table {
  int start_index;
  int entry_count;
  struct ir_symbol_table* next;
  struct ir_symbol_table_element elements[512];
};

struct ir_symbol_table* ir_symbol_create_table(struct ir_symbol_table* prev);