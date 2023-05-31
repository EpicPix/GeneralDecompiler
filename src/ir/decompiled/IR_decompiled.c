#include "../IR.h"
#include "../../utils.h"
#include <stdlib.h>
#include <stdio.h>

static const char* ir_decompiled_get_type_name(ir_type_t type) {
  if(ir_equal_types(type, ir_type_s8)) return "int8_t";
  if(ir_equal_types(type, ir_type_s16)) return "int16_t";
  if(ir_equal_types(type, ir_type_s32)) return "int32_t";
  if(ir_equal_types(type, ir_type_s64)) return "int64_t";
  if(ir_equal_types(type, ir_type_u8)) return "uint8_t";
  if(ir_equal_types(type, ir_type_u16)) return "uint16_t";
  if(ir_equal_types(type, ir_type_u32)) return "uint32_t";
  if(ir_equal_types(type, ir_type_u64)) return "uint64_t";
  if(ir_equal_types(type, ir_type_f32)) return "float";
  if(ir_equal_types(type, ir_type_f64)) return "double";
  return "unktype";
}

struct ir_decompiled_register_definitions {
    uint64_t reg;
    struct ir_decompiled_register_definitions* next;
};

static bool ir_decompiled_define_register(struct ir_instruction_collapsed_location loc, struct ir_decompiled_register_definitions** defs) {
  if(loc.location_type != ir_instruction_collapsed_location_type_register) return false;
  if(!*defs) {
    *defs = malloc(sizeof(struct ir_decompiled_register_definitions));
    (*defs)->reg = loc.data.reg;
    return false;
  }
  struct ir_decompiled_register_definitions* cur = *defs;
  struct ir_decompiled_register_definitions* prev = *defs;
  while(cur) {
    if(loc.data.reg == cur->reg) {
      return true;
    }
    prev = cur;
    cur = cur->next;
  }
  prev->next = malloc(sizeof(struct ir_decompiled_register_definitions));
  prev->next->reg = loc.data.reg;
  return false;
}

static void ir_decompiled_print_instruction(struct ir_instruction_collapsed* instr, struct ir_decompiled_register_definitions** defs);

static void ir_decompiled_print_location(struct ir_instruction_collapsed_location loc, struct ir_decompiled_register_definitions** defs) {
  if(loc.location_type == ir_instruction_collapsed_location_type_immediate) {
    printf("%ld", loc.data.imm);
  }else if(loc.location_type == ir_instruction_collapsed_location_type_register) {
    if(loc.data.reg >> 63) {
      printf("rx%ld", -loc.data.reg);
    }else {
      printf("r%ld", loc.data.reg);
    }
  }else if(loc.location_type == ir_instruction_collapsed_location_type_inlined) {
    printf("(");
    ir_decompiled_print_instruction(loc.data.instr, defs);
    printf(")");
  }
}

static void ir_decompiled_print_instruction(struct ir_instruction_collapsed* instr, struct ir_decompiled_register_definitions** defs) {
  if(instr->type == ir_instruction_collapsed_type_mov) {
    if(!ir_decompiled_define_register(instr->data.mov.output, defs)) printf("%s ", ir_decompiled_get_type_name(instr->data.mov.output.type));
    ir_decompiled_print_location(instr->data.mov.output, defs);
    printf(" = ");
    ir_decompiled_print_location(instr->data.mov.input, defs);
  }else if(instr->type == ir_instruction_collapsed_type_add) {
    if(!ir_decompiled_define_register(instr->data.add.output, defs)) printf("%s ", ir_decompiled_get_type_name(instr->data.add.output.type));
    ir_decompiled_print_location(instr->data.add.output, defs);
    printf(" = ");
    ir_decompiled_print_location(instr->data.add.inputa, defs);
    printf(" + ");
    ir_decompiled_print_location(instr->data.add.inputb, defs);
  }else if(instr->type == ir_instruction_collapsed_type_sub) {
    if(!ir_decompiled_define_register(instr->data.sub.output, defs)) printf("%s ", ir_decompiled_get_type_name(instr->data.sub.output.type));
    ir_decompiled_print_location(instr->data.sub.output, defs);
    printf(" = ");
    ir_decompiled_print_location(instr->data.sub.inputa, defs);
    printf(" - ");
    ir_decompiled_print_location(instr->data.sub.inputb, defs);
  }else if(instr->type == ir_instruction_collapsed_type_mul) {
    if(!ir_decompiled_define_register(instr->data.mul.output, defs)) printf("%s ", ir_decompiled_get_type_name(instr->data.mul.output.type));
    ir_decompiled_print_location(instr->data.mul.output, defs);
    printf(" = ");
    ir_decompiled_print_location(instr->data.mul.inputa, defs);
    printf(" * ");
    ir_decompiled_print_location(instr->data.mul.inputb, defs);
  }
}

void ir_decompiled_print(struct ir_data data) {
  if(data.instruction_level != ir_instruction_level_collapsed) return;

  struct ir_decompiled_register_definitions* defs;

  struct ir_instruction_list* instructions = data.instructions;
  while(instructions) {
    for(uint64_t i = 0; i<instructions->instruction_count; i++) {
      ir_decompiled_print_instruction(&instructions->instructions.collapsed_level[i], &defs);
      printf(";\n");
    }
    instructions = instructions->next;
  }
}