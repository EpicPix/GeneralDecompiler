#pragma once

#include <stdint.h>
#include <stdbool.h>

struct ir_data {
  
};

struct ir_data* ir_optimize(struct ir_data* data);
void ir_print_instructions(struct ir_data* data, void* details);
void ir_print_decompiled(struct ir_data* data, void* details);
