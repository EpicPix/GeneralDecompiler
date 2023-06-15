#include "IR_optimizer.h"
#include "../../utils.h"
#include <stdlib.h>
#include <stdio.h>


static struct ir_instruction_list* ir_optimize_run_step(ir_optimize_step_func_t step_func, struct ir_instruction_list* input_instructions, struct ir_optimize_data* optimizer_data) {
  struct ir_instruction_list* instructions_start = ir_instruction_create_list(NULL, 0x10000, 1024, ir_instruction_level_low);
  struct ir_instruction_list* instructions_current = instructions_start;
  while(input_instructions) {
    for(uint64_t i = 0; i<input_instructions->instruction_count; i++) {
      uint64_t ptr = input_instructions->start_address + i;
      instructions_current = step_func(instructions_current, ptr, input_instructions, optimizer_data);
    }
    input_instructions = input_instructions->next;
  }
  return instructions_start;
}

struct ir_data ir_optimize(struct ir_data data) {
  if(data.instruction_level != ir_instruction_level_low) return data;

  struct ir_symbol_table* symbol_table = ir_symbol_create_table(NULL);
  struct ir_optimize_data optimizer_data = { };

  double start = get_app_time();
  DEBUG_LOG("ir_optimizer", "%s \"%s\"", "Running optimization step", "stack");
  struct ir_instruction_list* instructions_step_empty = ir_optimize_run_step(ir_optimize_step_stack, data.instructions,
                                                                             &optimizer_data);
  DEBUG_LOG("ir_optimizer", "Optimizations finished, took %.6lf seconds for all optimizations", get_app_time() - start);

  return (struct ir_data) {
          .instruction_level = ir_instruction_level_low,
          .instructions = instructions_step_empty,
          .memory_page_start = data.memory_page_start,
          .symbol_table = symbol_table,
          .type_table = data.type_table
  };
}
