#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "IR_high_level.h"
#include "IR_low_level.h"

struct ir_symbol_table {

};

struct ir_data {
  struct ir_symbol_table* symbol_table;
};

struct ir_data ir_optimize(struct ir_data data);
void ir_print_instructions(struct ir_data data);
void ir_print_decompiled(struct ir_data data);
