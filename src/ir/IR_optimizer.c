#include "IR.h"
#include "../utils.h"
#include <stdlib.h>
#include <stdio.h>

static struct ir_instruction_low* ir_get_instruction(struct ir_instruction_list* instructions, uint64_t instruction) {
  while(instructions) {
    if(instructions->start_address <= instruction && instructions->start_address + instructions->allocated_count > instruction) {
      if(instructions->start_address + instructions->instruction_count <= instruction) {
        return NULL;
      }
      return &instructions->instructions.low_level[instruction - instructions->start_address];
    }
    instructions = instructions->next;
  }
  return NULL;
}

struct ir_data ir_optimize(struct ir_data data) {
  if(data.is_high_level) return data;

  struct ir_symbol_table* symbol_table = ir_symbol_create_table(NULL);
  struct ir_instruction_list* instructions_start = ir_instruction_create_list(NULL, 0x10000, 1024, false);
  struct ir_instruction_list* instructions_current = instructions_start;
  struct ir_instruction_list* instructions_in = data.instructions;

  while(instructions_in) {
    for(uint64_t i = 0; i<instructions_in->instruction_count; i++) {
      uint64_t ptr = instructions_in->start_address + i;
      struct ir_instruction_low* instr = ir_get_instruction(instructions_in, ptr);
      instructions_current = ir_instruction_add_instruction_low(instructions_current, 1024, *instr);
    }
    instructions_in = instructions_in->next;
  }

  return (struct ir_data) {
          .is_high_level = false,
          .instructions = instructions_start,
          .memory_page_start = data.memory_page_start,
          .symbol_table = symbol_table,
          .type_table = data.type_table
  };
}
