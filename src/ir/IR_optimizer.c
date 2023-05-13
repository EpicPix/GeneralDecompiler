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

struct ir_optimize_location_mapping {
    struct ir_instruction_low_location_with_offset loc;
    uint64_t reg;
};

struct ir_optimize_location_mappings {
    struct ir_optimize_location_mapping mapping;
    struct ir_optimize_location_mappings* next;
};

static struct ir_instruction_list* ir_optimize_body(struct ir_instruction_list* output, uint64_t current_location, struct ir_instruction_list* input_instructions, struct ir_optimize_location_mappings) {
  struct ir_instruction_low* current_instruction = ir_get_instruction(input_instructions, current_location);
  return ir_instruction_add_instruction_low(output, 1024, *current_instruction);
}

struct ir_data ir_optimize(struct ir_data data) {
  if(data.is_high_level) return data;

  struct ir_symbol_table* symbol_table = ir_symbol_create_table(NULL);
  struct ir_instruction_list* instructions_start = ir_instruction_create_list(NULL, 0x10000, 1024, false);
  struct ir_instruction_list* instructions_current = instructions_start;
  struct ir_instruction_list* instructions_in = data.instructions;
  struct ir_optimize_location_mappings* mappings = NULL;

  while(instructions_in) {
    for(uint64_t i = 0; i<instructions_in->instruction_count; i++) {
      uint64_t ptr = instructions_in->start_address + i;
      instructions_current = ir_optimize_body(instructions_current, ptr, instructions_in, mappings);
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
