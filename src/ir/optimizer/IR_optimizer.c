#include "IR_optimizer.h"
#include "../../utils.h"
#include <stdlib.h>
#include <stdio.h>

struct ir_optimize_location_mapping* ir_optimize_get_or_create_mapping(struct ir_instruction_low_location_with_offset loc, struct ir_optimize_data* data) {
  if(data->mappings == NULL) {
    struct ir_optimize_location_mappings* lmap = malloc(sizeof(struct ir_optimize_location_mappings));
    lmap->mapping = (struct ir_optimize_location_mapping) { .loc = loc, .reg = ir_instruction_low_special_registers_mappings_start - data->mapping_count };
    lmap->next = NULL;
    data->mappings = lmap;
    data->mapping_count++;
    return &lmap->mapping;
  }
  struct ir_optimize_location_mappings* maps = data->mappings;
  struct ir_optimize_location_mappings* pre_last = maps;
  while(maps) {
    if(ir_instruction_compare_locations_with_offset_low(loc, maps->mapping.loc)) {
      return &maps->mapping;
    }
    pre_last = maps;
    maps = maps->next;
  }

  struct ir_optimize_location_mappings* lmap = malloc(sizeof(struct ir_optimize_location_mappings));
  lmap->mapping = (struct ir_optimize_location_mapping) { .loc = loc, .reg = ir_instruction_low_special_registers_mappings_start - data->mapping_count++ };
  lmap->next = NULL;
  pre_last->next = lmap;
  return &lmap->mapping;
}

bool ir_optimize_remove_mapping(struct ir_instruction_low_location_with_offset loc, struct ir_optimize_data* data) {
  if(!data->mappings) return false;
  struct ir_optimize_location_mappings* pre_last = NULL;
  struct ir_optimize_location_mappings* maps = data->mappings;
  while(maps) {
    if(ir_instruction_compare_locations_with_offset_low(loc, maps->mapping.loc)) {
      if(pre_last) {
        pre_last->next = maps->next;
      }else {
        data->mappings = maps->next;
      }
      free(maps);
      return true;
    }
    pre_last = maps;
    maps = maps->next;
  }
  return false;
}

struct ir_optimize_register_usage_data* ir_optimize_increment_register_usage_mapping(struct ir_instruction_low_location* reg, struct ir_instruction_low_location* loc, struct ir_optimize_data* data) {
  if(reg->location_type != ir_instruction_low_location_type_register && reg->location_type != ir_instruction_low_location_type_register_address) return NULL;

  if(data->register_usage == NULL) {
    struct ir_optimize_register_usage_data* usage = malloc(sizeof(struct ir_optimize_register_usage_data));
    usage->reg = *reg;
    usage->count = 1;
    usage->has_assigned_data = loc != NULL;
    if(loc) {
      usage->assigned_data = *loc;
    }
    usage->next = NULL;
    data->register_usage = usage;
    return usage;
  }
  struct ir_optimize_register_usage_data* usage = data->register_usage;
  struct ir_optimize_register_usage_data* pre_last = usage;
  while(usage) {
    if(ir_instruction_compare_locations_low(*reg, usage->reg)) {
      if(!usage->has_assigned_data && loc) {
        usage->assigned_data = *loc;
      }
      usage->count++;
      return usage;
    }
    pre_last = usage;
    usage = usage->next;
  }

  usage = malloc(sizeof(struct ir_optimize_register_usage_data));
  usage->reg = *reg;
  usage->has_assigned_data = loc != NULL;
  if(loc) {
    usage->assigned_data = *loc;
  }
  usage->count = 1;
  usage->next = NULL;
  pre_last->next = usage;
  return usage;
}



static struct ir_instruction_list* ir_optimize_run_step(ir_optimize_step_func_t step_func, struct ir_instruction_list* input_instructions, struct ir_optimize_data* optimizer_data) {
  struct ir_instruction_list* instructions_start = ir_instruction_create_list(NULL, 0x10000, 1024, false);
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
  if(data.is_high_level) return data;

  struct ir_symbol_table* symbol_table = ir_symbol_create_table(NULL);
  struct ir_optimize_data optimizer_data = { .mappings = NULL, .mapping_count = 0, .register_usage = NULL, .register_tree = NULL };

  struct ir_instruction_list* instructions_step1 = ir_optimize_run_step(ir_optimize_step1, data.instructions, &optimizer_data);
  ir_optimize_prepare_step2(&optimizer_data);
  struct ir_instruction_list* instructions_step2 = ir_optimize_run_step(ir_optimize_step2, instructions_step1, &optimizer_data);
  ir_optimize_prepare_step3(&optimizer_data);
  struct ir_instruction_list* instructions_step3 = ir_optimize_run_step(ir_optimize_step3, instructions_step2, &optimizer_data);

  return (struct ir_data) {
          .is_high_level = false,
          .instructions = instructions_step3,
          .memory_page_start = data.memory_page_start,
          .symbol_table = symbol_table,
          .type_table = data.type_table
  };
}
