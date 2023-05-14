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

static struct ir_optimize_location_mapping* ir_optimize_get_or_create_mapping(struct ir_optimize_location_mappings** mappings, struct ir_instruction_low_location_with_offset loc, uint64_t* mapping_count) {
  if(*mappings == NULL) {
    struct ir_optimize_location_mappings* lmap = malloc(sizeof(struct ir_optimize_location_mappings));
    lmap->mapping = (struct ir_optimize_location_mapping) { .loc = loc, .reg = ir_instruction_low_special_registers_mappings_start - *mapping_count };
    lmap->next = NULL;
    *mappings = lmap;
    (*mapping_count)++;
    return &lmap->mapping;
  }
  struct ir_optimize_location_mappings* pre_last = *mappings;
  struct ir_optimize_location_mappings* maps = *mappings;
  while(maps) {
    if(ir_instruction_compare_locations_with_offset_low(loc, maps->mapping.loc)) {
      return &maps->mapping;
    }
    pre_last = maps;
    maps = maps->next;
  }

  struct ir_optimize_location_mappings* lmap = malloc(sizeof(struct ir_optimize_location_mappings));
  lmap->mapping = (struct ir_optimize_location_mapping) { .loc = loc, .reg = ir_instruction_low_special_registers_mappings_start - *mapping_count };
  lmap->next = NULL;
  pre_last->next = lmap;
  (*mapping_count)++;
  return &lmap->mapping;
}

static bool ir_optimize_remove_mapping(struct ir_optimize_location_mappings** mappings, struct ir_instruction_low_location_with_offset loc) {
  if(!*mappings) return false;
  struct ir_optimize_location_mappings* pre_last = NULL;
  struct ir_optimize_location_mappings* maps = *mappings;
  while(maps) {
    if(ir_instruction_compare_locations_with_offset_low(loc, maps->mapping.loc)) {
      if(pre_last) {
        pre_last->next = maps->next;
      }else {
        *mappings = maps->next;
      }
      free(maps);
      return true;
    }
    pre_last = maps;
    maps = maps->next;
  }
  return false;
}

static struct ir_instruction_list* ir_optimize_body(struct ir_instruction_list* output, uint64_t current_location, struct ir_instruction_list* input_instructions, struct ir_optimize_location_mappings** mappings, uint64_t* mapping_count) {
  struct ir_instruction_low* instr = ir_get_instruction(input_instructions, current_location);
  if(instr->type == ir_instruction_low_type_mov_offsetout) {
    struct ir_optimize_location_mapping* mapping = ir_optimize_get_or_create_mapping(mappings, instr->data.movoout.output, mapping_count);
    return ir_instruction_add_instruction_low(output, 1024, (struct ir_instruction_low) {
      .type = ir_instruction_low_type_mov,
      .data = {
        .mov = {
          .input = instr->data.movoout.input,
          .output = (struct ir_instruction_low_location) { .type = mapping->loc.loc.type, .location_type = ir_instruction_low_location_type_register, .data = { .reg = mapping->reg } }
        }
      }
    });
  }else if(instr->type == ir_instruction_low_type_mov_offsetin) {
    struct ir_optimize_location_mapping* mapping = ir_optimize_get_or_create_mapping(mappings, instr->data.movoin.input, mapping_count);
    return ir_instruction_add_instruction_low(output, 1024, (struct ir_instruction_low) {
      .type = ir_instruction_low_type_mov,
      .data = {
        .movoin = {
          .input = (struct ir_instruction_low_location) { .type = mapping->loc.loc.type, .location_type = ir_instruction_low_location_type_register, .data = { .reg = mapping->reg } },
          .output = instr->data.movoin.output
        }
      }
    });
  }else if(instr->type == ir_instruction_low_type_norelso) {
    bool mapping_removed = ir_optimize_remove_mapping(mappings, instr->data.norel.loc);
    if(mapping_removed) {
      return output;
    }
  }
  return ir_instruction_add_instruction_low(output, 1024, *instr);
}

struct ir_data ir_optimize(struct ir_data data) {
  if(data.is_high_level) return data;

  struct ir_symbol_table* symbol_table = ir_symbol_create_table(NULL);
  struct ir_instruction_list* instructions_start = ir_instruction_create_list(NULL, 0x10000, 1024, false);
  struct ir_instruction_list* instructions_current = instructions_start;
  struct ir_instruction_list* instructions_in = data.instructions;
  struct ir_optimize_location_mappings* mappings = NULL;
  uint64_t mapping_count = 0;

  while(instructions_in) {
    for(uint64_t i = 0; i<instructions_in->instruction_count; i++) {
      uint64_t ptr = instructions_in->start_address + i;
      instructions_current = ir_optimize_body(instructions_current, ptr, instructions_in, &mappings, &mapping_count);
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
