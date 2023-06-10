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
  if(loc.data.reg == (uint64_t) ir_instruction_low_special_registers_base_stack) return true;
  if(loc.data.reg == (uint64_t) ir_instruction_low_special_registers_stack) return true;
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

static void ir_decompiled_print_register_name(uint64_t reg) {
  if(reg == (uint64_t) ir_instruction_low_special_registers_base_stack) {
    printf("rbp");
    return;
  }
  if(reg == (uint64_t) ir_instruction_low_special_registers_stack) {
    printf("rsp");
    return;
  }
  if(reg >> 63) {
    printf("rx%ld", -reg);
  }else {
    printf("r%ld", reg);
  }
}

static void ir_decompiled_print_location(struct ir_instruction_collapsed_location loc, struct ir_decompiled_register_definitions** defs) {
  if(loc.location_type == ir_instruction_collapsed_location_type_immediate) {
    printf("%ld", loc.data.imm);
  }else if(loc.location_type == ir_instruction_collapsed_location_type_register) {
    ir_decompiled_print_register_name(loc.data.reg);
  }else if(loc.location_type == ir_instruction_collapsed_location_type_register_address) {
    printf("*");
    ir_decompiled_print_register_name(loc.data.reg);
  }else if(loc.location_type == ir_instruction_collapsed_location_type_inlined) {
    printf("(");
    ir_decompiled_print_instruction(loc.data.instr, defs);
    printf(")");
  }
}

static void ir_decompiled_print_instruction_assign(struct ir_instruction_collapsed_location* instr, struct ir_decompiled_register_definitions** defs) {
  if(instr->location_type == ir_instruction_collapsed_location_type_register_address) {
    printf("*((%s*) ", ir_decompiled_get_type_name(instr->type));
    struct ir_instruction_collapsed_location inst = *instr;
    inst.location_type = ir_instruction_collapsed_location_type_register;
    ir_decompiled_print_location(inst, defs);
    printf(")");
  }else {
    if (!ir_decompiled_define_register(*instr, defs)) printf("%s ", ir_decompiled_get_type_name(instr->type));
    ir_decompiled_print_location(*instr, defs);
  }
}

static void ir_decompiled_print_instruction(struct ir_instruction_collapsed* instr, struct ir_decompiled_register_definitions** defs) {
  if(instr->type == ir_instruction_collapsed_type_mov) {
    ir_decompiled_print_instruction_assign(&instr->data.mov.output, defs);
    printf(" = ");
    ir_decompiled_print_location(instr->data.mov.input, defs);
  }else if(instr->type == ir_instruction_collapsed_type_add) {
    ir_decompiled_print_instruction_assign(&instr->data.add.output, defs);
    printf(" = ");
    ir_decompiled_print_location(instr->data.add.inputa, defs);
    printf(" + ");
    ir_decompiled_print_location(instr->data.add.inputb, defs);
  }else if(instr->type == ir_instruction_collapsed_type_sub) {
    ir_decompiled_print_instruction_assign(&instr->data.sub.output, defs);
    printf(" = ");
    ir_decompiled_print_location(instr->data.sub.inputa, defs);
    printf(" - ");
    ir_decompiled_print_location(instr->data.sub.inputb, defs);
  }else if(instr->type == ir_instruction_collapsed_type_mul) {
    ir_decompiled_print_instruction_assign(&instr->data.mul.output, defs);
    printf(" = ");
    ir_decompiled_print_location(instr->data.mul.inputa, defs);
    printf(" * ");
    ir_decompiled_print_location(instr->data.mul.inputb, defs);
  }
}

void ir_decompiled_print(struct ir_data data) {
  if(data.instruction_level != ir_instruction_level_collapsed) return;

  struct ir_decompiled_register_definitions* defs = NULL;

  struct ir_instruction_list* instructions = data.instructions;
  while(instructions) {
    for(uint64_t i = 0; i<instructions->instruction_count; i++) {
      ir_decompiled_print_instruction(&instructions->instructions.collapsed_level[i], &defs);
      printf(";\n");
    }
    instructions = instructions->next;
  }
}