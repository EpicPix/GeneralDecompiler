#include "IR.h"
#include "../utils.h"
#include <stdlib.h>
#include <stdio.h>

static void ir_print_instruction_print_type(ir_type_t type) {
  printf("{:");
  if(type.is_builtin) {
    if(type.built_in.type == ir_type_definition_type_void) {
      printf("v");
    }else if(type.built_in.type == ir_type_definition_type_int_signed) {
      printf("s%d", type.built_in.size);
    }else if(type.built_in.type == ir_type_definition_type_int_unsigned) {
      printf("u%d", type.built_in.size);
    }else if(type.built_in.type == ir_type_definition_type_float) {
      printf("f%d", type.built_in.size);
    }else {
      printf("%d-%d", type.built_in.type, type.built_in.size);
    }
  }else {
    printf("%d", type.composed_type_index);
  }
  printf("}");
}

static void ir_print_instruction_high_inl(struct ir_instruction_high* instr);
static void ir_print_instruction_high_location(struct ir_instruction_high_location* location) {
  ir_print_instruction_print_type(location->type);
  printf(" ");
  if(location->location_type == ir_instruction_high_location_type_immediate) {
    printf("%ld", location->data.imm);
  }else if(location->location_type == ir_instruction_high_location_type_address) {
    printf("*0x%016lx", location->data.addr);
  }else if(location->location_type == ir_instruction_high_location_type_register) {
    printf("@%ld", location->data.reg);
  }else if(location->location_type == ir_instruction_high_location_type_register_address) {
    printf("*@%ld", location->data.reg);
  }else if(location->location_type == ir_instruction_high_location_type_instruction) {
    printf("<");
    ir_print_instruction_high_inl(location->data.instr);
    printf(">");
  }else if(location->location_type == ir_instruction_high_location_type_inherited) {
    printf("@^");
  }else {
    printf("?");
  }
}

static void ir_print_instruction_high_inl(struct ir_instruction_high* instr) {
  if(instr->type == ir_instruction_high_type_add) {
    printf("add ");
    ir_print_instruction_high_location(&instr->data.oii.output);
    printf(", ");
    ir_print_instruction_high_location(&instr->data.oii.inputa);
    printf(", ");
    ir_print_instruction_high_location(&instr->data.oii.inputb);
  }else if(instr->type == ir_instruction_high_type_sub) {
    printf("sub ");
    ir_print_instruction_high_location(&instr->data.oii.output);
    printf(", ");
    ir_print_instruction_high_location(&instr->data.oii.inputa);
    printf(", ");
    ir_print_instruction_high_location(&instr->data.oii.inputb);
  }else if(instr->type == ir_instruction_high_type_mul) {
    printf("mul ");
    ir_print_instruction_high_location(&instr->data.oii.output);
    printf(", ");
    ir_print_instruction_high_location(&instr->data.oii.inputa);
    printf(", ");
    ir_print_instruction_high_location(&instr->data.oii.inputb);
  }else if(instr->type == ir_instruction_high_type_mov) {
    printf("mov ");
    ir_print_instruction_high_location(&instr->data.oi.output);
    printf(", ");
    ir_print_instruction_high_location(&instr->data.oi.input);
  }else if(instr->type == ir_instruction_high_type_intrinsic) {
    printf("intrinsic %d", instr->data.intrinsic);
  }else if(instr->type == ir_instruction_high_type_intrinsic_typed) {
    printf("intrinsic ");
    ir_print_instruction_print_type(instr->data.intrinsic_typed.type);
    printf(" %d", instr->data.intrinsic);
  }
}

static void ir_print_instruction_high(uint64_t addr, struct ir_instruction_high* instr) {
  printf("0x%016lx: ", addr);
  ir_print_instruction_high_inl(instr);
  printf("\n");
}

static void ir_print_instruction_low_location(struct ir_instruction_low_location* location) {
  ir_print_instruction_print_type(location->type);
  printf(" ");
  if(location->location_type == ir_instruction_low_location_type_immediate) {
    printf("%ld", location->data.imm);
  }else if(location->location_type == ir_instruction_low_location_type_address) {
    printf("*0x%016lx", location->data.addr);
  }else if(location->location_type == ir_instruction_low_location_type_register) {
    printf("@%ld", location->data.reg);
  }else if(location->location_type == ir_instruction_low_location_type_register_address) {
    printf("*@%ld", location->data.reg);
  }else {
    printf("?");
  }
}

static void ir_print_instruction_low_location_offset(struct ir_instruction_low_location_with_offset* loc) {
  printf("(");
  ir_print_instruction_low_location(&loc->loc);
  printf(" + ");
  ir_print_instruction_low_location(&loc->offset);
  printf(")");
}

#include "generated/instructions_ir_level_low_print_include.h"

static void ir_print_instruction_low(uint64_t addr, struct ir_instruction_low* instr) {
  printf("0x%016lx: ", addr);
  ir_print_instruction_low_inl(instr);
  printf("\n");
}



static void ir_print_instruction_collapsed_inl(struct ir_instruction_collapsed* instr);
static void ir_print_instruction_collapsed_location(struct ir_instruction_collapsed_location* location) {
  ir_print_instruction_print_type(location->type);
  printf(" ");
  if(location->location_type == ir_instruction_collapsed_location_type_immediate) {
    printf("%ld", location->data.imm);
  }else if(location->location_type == ir_instruction_collapsed_location_type_address) {
    printf("*0x%016lx", location->data.addr);
  }else if(location->location_type == ir_instruction_collapsed_location_type_register) {
    printf("@%ld", location->data.reg);
  }else if(location->location_type == ir_instruction_collapsed_location_type_register_address) {
    printf("*@%ld", location->data.reg);
  }else if(location->location_type == ir_instruction_collapsed_location_type_inlined) {
    printf("<");
    ir_print_instruction_collapsed_inl(location->data.instr);
    printf(">");
  }else if(location->location_type == ir_instruction_collapsed_location_type_inherit) {
    printf("@^");
  }else {
    printf("?");
  }
}

#include "generated/instructions_ir_level_collapsed_print_include.h"

static void ir_print_instruction_collapsed(uint64_t addr, struct ir_instruction_collapsed* instr) {
  printf("0x%016lx: ", addr);
  ir_print_instruction_collapsed_inl(instr);
  printf("\n");
}

void ir_print_instructions(struct ir_data data) {
  struct ir_instruction_list* list = data.instructions;
  if(data.instruction_level == ir_instruction_level_high) {
    while(list != NULL) {
      for(uint64_t i = 0; i<list->instruction_count; i++) {
        ir_print_instruction_high(list->start_address + i, &list->instructions.high_level[i]);
      }
      list = list->next;
    }
  }else if(data.instruction_level == ir_instruction_level_low) {
    while(list != NULL) {
      for(uint64_t i = 0; i<list->instruction_count; i++) {
        ir_print_instruction_low(list->start_address + i, &list->instructions.low_level[i]);
      }
      list = list->next;
    }
  }else if(data.instruction_level == ir_instruction_level_collapsed) {
    while(list != NULL) {
      for(uint64_t i = 0; i<list->instruction_count; i++) {
        ir_print_instruction_collapsed(list->start_address + i, &list->instructions.collapsed_level[i]);
      }
      list = list->next;
    }
  }
}