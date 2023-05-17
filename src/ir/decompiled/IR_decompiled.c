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

static void ir_decompiled_print_location(struct ir_instruction_low_location loc) {
  if(loc.location_type == ir_instruction_low_location_type_immediate) {
    printf("%ld", loc.data.imm);
  }else if(loc.location_type == ir_instruction_low_location_type_register) {
    if(loc.data.reg >> 63) {
      printf("rx%ld", -loc.data.reg);
    }else {
      printf("r%ld", loc.data.reg);
    }
  }
}

static void ir_decompiled_print_instruction(struct ir_instruction_low* instr) {
  if(instr->type == ir_instruction_low_type_mov) {
    printf("%s ", ir_decompiled_get_type_name(instr->data.mov.output.type));
    ir_decompiled_print_location(instr->data.mov.output);
    printf(" = ");
    ir_decompiled_print_location(instr->data.mov.input);
    printf(";\n");
  }else if(instr->type == ir_instruction_low_type_add) {
    printf("%s ", ir_decompiled_get_type_name(instr->data.add.output.type));
    ir_decompiled_print_location(instr->data.add.output);
    printf(" = ");
    ir_decompiled_print_location(instr->data.add.inputa);
    printf(" + ");
    ir_decompiled_print_location(instr->data.add.inputb);
    printf(";\n");
  }
}

void ir_decompiled_print(struct ir_data data) {
  if(data.instruction_level != ir_instruction_level_low) return;

  struct ir_instruction_list* instructions = data.instructions;
  while(instructions) {
    for(uint64_t i = 0; i<instructions->instruction_count; i++) {
      ir_decompiled_print_instruction(&instructions->instructions.low_level[i]);
    }
    instructions = instructions->next;
  }
}