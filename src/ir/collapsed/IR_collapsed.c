#include "../IR.h"
#include "../../utils.h"
#include <stdlib.h>
#include <stdio.h>

struct ir_data ir_collapse(struct ir_data data) {
  return data;
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
