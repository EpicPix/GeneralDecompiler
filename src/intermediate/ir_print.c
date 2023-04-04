#include "IR.h"
#include "stdio.h"

const char* ir_instr_name(enum ir_instruction_type instr) {
  switch(instr) {
    case IR_INSTR_ADD: return "add";
    case IR_INSTR_SUB: return "sub";
    case IR_INSTR_MUL: return "mul";
    case IR_INSTR_DIV: return "div";
    case IR_INSTR_MOD: return "mod";
    case IR_INSTR_AND: return "and";
    case IR_INSTR_OR: return "or";
    case IR_INSTR_XOR: return "xor";
    case IR_INSTR_MOV: return "mov";
    case IR_INSTR_INTRINSIC: return "intrinsic";
    default: return "unknown";
  }
}

void ir_print_type(struct ir_type type) {
    if(type.is_8bit) printf("8bit");
    else if(type.is_16bit) printf("16bit");
    else if(type.is_32bit) printf("32bit");
    else if(type.is_64bit) printf("64bit");
    else printf("??bit");

    if(type.is_unsigned) printf(",unsigned");
    if(type.is_atomic) printf(",atomic");
    if(type.is_float) printf(",float");
};

void ir_print_immediate(struct ir_immediate* imm) {
  printf("immediate ");
  ir_print_type(imm->type);
  printf(" %lu", imm->value);
}

void ir_print_register(struct ir_register* reg) {
  printf("register ");
  ir_print_type(reg->type);
  printf(" %d", reg->register_index);
}

void ir_print_stack(struct ir_stack* stack) {
  printf("stack ");
  ir_print_type(stack->type);
}

void ir_print_pointer(struct ir_pointer* pointer) {
  printf("pointer ");
  ir_print_type(pointer->type);
  printf(" %d %u %u", pointer->register_index, pointer->displacement, pointer->scale);
}

void ir_print_address(struct ir_address* addr) {
  printf("address ");
  ir_print_type(addr->type);
  printf(" %016lx", addr->address);
}

void ir_print_data_access_in(struct ir_data_access_in* access) {
  printf("[");
  switch(access->data_type) {
    case IR_DATATYPE_IMMEDIATE: ir_print_immediate(&access->imm); break;
    case IR_DATATYPE_REGISTER: ir_print_register(&access->reg); break;
    case IR_DATATYPE_STACK: ir_print_stack(&access->stack); break;
    case IR_DATATYPE_POINTER: ir_print_pointer(&access->pointer); break;
    case IR_DATATYPE_ADDRESS: ir_print_address(&access->address); break;
  }
  printf("]");
}

void ir_print_data_access_out(struct ir_data_access_out* access) {
  printf("[");
  switch(access->data_type) {
    case IR_DATATYPE_REGISTER: ir_print_register(&access->reg); break;
    case IR_DATATYPE_STACK: ir_print_stack(&access->stack); break;
    case IR_DATATYPE_POINTER: ir_print_pointer(&access->pointer); break;
    case IR_DATATYPE_ADDRESS: ir_print_address(&access->address); break;
  }
  printf("]");
}

void ir_print_instruction(struct ir_instruction* instr) {
  printf("%s ", ir_instr_name(instr->instr_type));
  switch(instr->instr_type) {
    case IR_INSTR_ADD:
    case IR_INSTR_SUB:
    case IR_INSTR_MUL:
    case IR_INSTR_DIV:
    case IR_INSTR_MOD:
    case IR_INSTR_AND:
    case IR_INSTR_OR:
    case IR_INSTR_XOR:
    {
      ir_print_type(instr->binary_operation.type);
      printf(" ");
      ir_print_data_access_in(&instr->binary_operation.a);
      printf(" ");
      ir_print_data_access_in(&instr->binary_operation.b);
      printf(" ");
      ir_print_data_access_out(&instr->binary_operation.result);
      printf(" ");
      printf("%04x", instr->binary_operation.modifiable_flags);
      break;
    }
    case IR_INSTR_MOV:
    {
      ir_print_type(instr->unary_operation.type);
      printf(" ");
      ir_print_data_access_in(&instr->unary_operation.input);
      printf(" ");
      ir_print_data_access_out(&instr->unary_operation.output);
      break;
    }
    case IR_INSTR_INTRINSIC:
    {
      printf("%08x", instr->intrinsic.intrinsic_id);
      break;
    }
  }
  printf("\n");
}