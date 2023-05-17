#include "../IR.h"
#include "../../utils.h"
#include <stdlib.h>
#include <stdio.h>

struct ir_collapse_register_usage_data {
    struct ir_instruction_collapsed_location reg;
    uint64_t count;
    struct ir_collapse_register_usage_data* next;
};

struct ir_collapse_data {
    struct ir_collapse_register_usage_data* register_usage;
};


static struct ir_instruction_collapsed_location ir_collapse_convert_location(struct ir_instruction_low_location location) {
  if(location.location_type == ir_instruction_low_location_type_immediate) return (struct ir_instruction_collapsed_location) { .type = location.type, .location_type = ir_instruction_collapsed_location_type_immediate, .data = { .imm = location.data.imm } };
  if(location.location_type == ir_instruction_low_location_type_register) return (struct ir_instruction_collapsed_location) { .type = location.type, .location_type = ir_instruction_collapsed_location_type_register, .data = { .reg = location.data.reg } };
  if(location.location_type == ir_instruction_low_location_type_register_address) return (struct ir_instruction_collapsed_location) { .type = location.type, .location_type = ir_instruction_collapsed_location_type_register_address, .data = { .reg = location.data.reg } };

  return (struct ir_instruction_collapsed_location) { .type = location.type, .location_type = ir_instruction_collapsed_location_type_address, .data = { .addr = location.data.addr } };
}

static struct ir_instruction_list* ir_collapse_convert_low(struct ir_instruction_low* instr, struct ir_instruction_list* list, struct ir_collapse_data* data) {
  if(instr->type == ir_instruction_low_type_mov) {
    return ir_instruction_add_instruction_collapsed(list, 1024, (struct ir_instruction_collapsed) {
      .type = ir_instruction_collapsed_type_mov,
      .data = {
        .mov = {
          .input = ir_collapse_convert_location(instr->data.mov.input),
          .output = ir_collapse_convert_location(instr->data.mov.output)
        }
      }
    });
  }else if(instr->type == ir_instruction_low_type_add) {
    return ir_instruction_add_instruction_collapsed(list, 1024, (struct ir_instruction_collapsed) {
      .type = ir_instruction_collapsed_type_add,
      .data = {
        .add = {
          .inputa = ir_collapse_convert_location(instr->data.add.inputa),
          .inputb = ir_collapse_convert_location(instr->data.add.inputb),
          .output = ir_collapse_convert_location(instr->data.add.output)
        }
      }
    });
  }
  return list;
}

struct ir_collapse_register_usage_data* ir_collapse_increment_register_usage_mapping(struct ir_instruction_collapsed_location* reg, struct ir_collapse_data* data) {
  if(reg->location_type != ir_instruction_collapsed_location_type_register && reg->location_type != ir_instruction_collapsed_location_type_register_address) return NULL;

  if(data->register_usage == NULL) {
    struct ir_collapse_register_usage_data* usage = malloc(sizeof(struct ir_collapse_register_usage_data));
    usage->reg = *reg;
    usage->count = 1;
    usage->next = NULL;
    data->register_usage = usage;
    return usage;
  }
  struct ir_collapse_register_usage_data* usage = data->register_usage;
  struct ir_collapse_register_usage_data* pre_last = usage;
  while(usage) {
    if(ir_instruction_compare_locations_collapsed(*reg, usage->reg)) {
      usage->count++;
      return usage;
    }
    pre_last = usage;
    usage = usage->next;
  }

  usage = malloc(sizeof(struct ir_collapse_register_usage_data));
  usage->reg = *reg;
  usage->count = 1;
  usage->next = NULL;
  pre_last->next = usage;
  return usage;
}

static struct ir_collapse_data ir_collapse_prepare_data(struct ir_data data) {
  struct ir_collapse_data cdata = { .register_usage = NULL };


  struct ir_instruction_list* input_instructions = data.instructions;
  while(input_instructions) {
    for(uint64_t i = 0; i<input_instructions->instruction_count; i++) {
      struct ir_instruction_low* instr = &input_instructions->instructions.low_level[i];
      if(instr->type == ir_instruction_low_type_mov) {
        struct ir_instruction_collapsed_location mi = ir_collapse_convert_location(instr->data.mov.input);
        ir_collapse_increment_register_usage_mapping(&mi, &cdata);
        struct ir_instruction_collapsed_location mo = ir_collapse_convert_location(instr->data.mov.output);
        ir_collapse_increment_register_usage_mapping(&mo, &cdata);
      }else if(instr->type == ir_instruction_low_type_add) {
        struct ir_instruction_collapsed_location mia = ir_collapse_convert_location(instr->data.add.inputa);
        ir_collapse_increment_register_usage_mapping(&mia, &cdata);
        struct ir_instruction_collapsed_location mib = ir_collapse_convert_location(instr->data.add.inputb);
        ir_collapse_increment_register_usage_mapping(&mib, &cdata);
        struct ir_instruction_collapsed_location mo = ir_collapse_convert_location(instr->data.add.output);
        ir_collapse_increment_register_usage_mapping(&mo, &cdata);
      }
    }
    input_instructions = input_instructions->next;
  }

  return cdata;
}

struct ir_data ir_collapse(struct ir_data data) {
  if(data.instruction_level != ir_instruction_level_low) return data;

  struct ir_symbol_table* symbol_table = ir_symbol_create_table(NULL);

  struct ir_collapse_data collapse = ir_collapse_prepare_data(data);

  struct ir_instruction_list* input_instructions = data.instructions;
  struct ir_instruction_list* instructions_out = ir_instruction_create_list(NULL, 0x10000, 1024, ir_instruction_level_collapsed);
  struct ir_instruction_list* instructions_current = instructions_out;
  while(input_instructions) {
    for(uint64_t i = 0; i<input_instructions->instruction_count; i++) {
      struct ir_instruction_low* instr = &input_instructions->instructions.low_level[i];
      instructions_current = ir_collapse_convert_low(instr, instructions_current, &collapse);
    }
    input_instructions = input_instructions->next;
  }

  return (struct ir_data) {
    .instruction_level = ir_instruction_level_collapsed,
    .instructions = instructions_out,
    .memory_page_start = data.memory_page_start,
    .symbol_table = symbol_table,
    .type_table = data.type_table
  };
}


bool ir_instruction_compare_locations_collapsed(struct ir_instruction_collapsed_location location_a, struct ir_instruction_collapsed_location location_b) {
  if(location_a.location_type != location_b.location_type) return false;
  if(!ir_equal_types(location_a.type, location_b.type)) return false;

  if(location_a.location_type == ir_instruction_collapsed_location_type_register) return location_a.data.reg == location_b.data.reg;
  if(location_a.location_type == ir_instruction_collapsed_location_type_register_address) return location_a.data.reg == location_b.data.reg;
  if(location_a.location_type == ir_instruction_collapsed_location_type_immediate) return location_a.data.imm == location_b.data.imm;
  if(location_a.location_type == ir_instruction_collapsed_location_type_address) return location_a.data.addr == location_b.data.addr;
  return false;
}

struct ir_instruction_list* ir_instruction_add_instruction_collapsed(struct ir_instruction_list* list, uint64_t instruction_count, struct ir_instruction_collapsed instr) {
  if(list->instruction_count >= list->allocated_count) {
    list = ir_instruction_create_list(list, list->start_address + list->allocated_count, instruction_count, ir_instruction_level_collapsed);
  }
  list->instructions.collapsed_level[list->instruction_count] = instr;
  list->instruction_count++;
  return list;
}

struct ir_instruction_collapsed* ir_instruction_alloc_collapsed(struct ir_instruction_collapsed data) {
  struct ir_instruction_collapsed* allocated = malloc(sizeof(struct ir_instruction_high));
  allocated->type = data.type;
  allocated->data = data.data;
  return allocated;
}
