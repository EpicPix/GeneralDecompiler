#pragma once

#include <stdint.h>
#include <stdbool.h>

struct ir_symbol_table {

};

struct ir_data {
  struct ir_symbol_table* symbol_table;
};

struct ir_data ir_optimize(struct ir_data data);
void ir_print_instructions(struct ir_data data);
void ir_print_decompiled(struct ir_data data);
