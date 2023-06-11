#include "../IR_optimizer.h"
#include "../../../utils.h"
#include <stdlib.h>
#include <stdio.h>

struct ir_instruction_list* ir_optimize_step_empty(struct ir_instruction_list* output, uint64_t current_location, struct ir_instruction_list* input_instructions, struct ir_optimize_data* data) {
  struct ir_instruction_low* instr = ir_get_instruction_low(input_instructions, current_location);
  return ir_instruction_add_instruction_low(output, 1024, *instr);
}