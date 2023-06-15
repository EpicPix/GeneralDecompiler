#pragma once

#include "../IR.h"

struct ir_optimize_data {
};

typedef struct ir_instruction_list* (*ir_optimize_step_func_t)(struct ir_instruction_list* output, uint64_t current_location, struct ir_instruction_list* input_instructions, struct ir_optimize_data* data);


struct ir_instruction_list* ir_optimize_step_stack(struct ir_instruction_list* output, uint64_t current_location, struct ir_instruction_list* input_instructions, struct ir_optimize_data* data);
