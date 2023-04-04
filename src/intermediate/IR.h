#pragma once
#include <stdint.h>
#include <stdbool.h>

#define IR_REGISTER_IP 0xffff
#define IR_REGISTER_SP 0xfffe

#define IR_DATATYPE_IMMEDIATE 0x00
#define IR_DATATYPE_REGISTER 0x01
#define IR_DATATYPE_STACK 0x02
#define IR_DATATYPE_POINTER 0x03
#define IR_DATATYPE_ADDRESS 0x04

enum ir_instruction_type {
  IR_INSTR_ADD,
  IR_INSTR_SUB,
  IR_INSTR_MUL,
  IR_INSTR_DIV,
  IR_INSTR_MOD,
  IR_INSTR_AND,
  IR_INSTR_OR,
  IR_INSTR_XOR,

  IR_INSTR_MOV,

  IR_INSTR_INTRINSIC,
};

struct ir_type {
  bool is_8bit : 1;
  bool is_16bit : 1;
  bool is_32bit : 1;
  bool is_64bit : 1;
  bool is_unsigned : 1;
  bool is_atomic : 1;
  bool is_float : 1;
};

struct ir_register {
  struct ir_type type;
  uint16_t register_index;
};

struct ir_stack {
  struct ir_type type;
};

struct ir_immediate {
  struct ir_type type;
  uint64_t value;
};

struct ir_pointer {
  struct ir_type type;
  uint16_t register_index;
  uint32_t displacement;
  uint8_t scale;
};

struct ir_address {
  struct ir_type type;
  uint64_t address;
};

struct ir_data_access_in {
  uint8_t data_type;
  union {
    struct ir_immediate imm;
    struct ir_register reg;
    struct ir_stack stack;
    struct ir_pointer pointer;
    struct ir_address address;
  };
};

struct ir_data_access_out {
  uint8_t data_type;
  union {
    struct ir_register reg;
    struct ir_stack stack;
    struct ir_pointer pointer;
    struct ir_address address;
  };
};

struct ir_binary_operation {
  uint16_t modifiable_flags;
  struct ir_type type;
  struct ir_data_access_in a;
  struct ir_data_access_in b;
  struct ir_data_access_out result;
};

struct ir_unary_operation {
  struct ir_type type;
  struct ir_data_access_in input;
  struct ir_data_access_out output;
};

struct ir_intrinsic {
  uint32_t intrinsic_id;
};

struct ir_instruction {
  enum ir_instruction_type instr_type;
  union {
    struct ir_binary_operation binary_operation;
    struct ir_unary_operation unary_operation;
    struct ir_intrinsic intrinsic;
  };
};

void ir_print_instruction(struct ir_instruction* instr);

#define IR_IMMEDIATE(TYPE, VALUE) (struct ir_data_access_in) { .data_type = IR_DATATYPE_IMMEDIATE, .imm = { .type = TYPE, .value = VALUE } }

#define IR_REGISTER(TYPE, REGISTER_INDEX) { .data_type = IR_DATATYPE_REGISTER, .reg = { .type = TYPE, .register_index = REGISTER_INDEX } }
#define IR_STACK(TYPE) { .data_type = IR_DATATYPE_STACK, .stack = { .type = TYPE } }
#define IR_POINTER(TYPE, REGISTER_INDEX, DISPLACEMENT, SCALE) { .data_type = IR_DATATYPE_POINTER, .pointer = { .type = TYPE, .register_index = REGISTER_INDEX, .displacement = DISPLACEMENT, .scale = SCALE } }
#define IR_ADDRESS(TYPE, ADDR) { .data_type = IR_DATATYPE_ADDRESS, .address = { .type = TYPE, .address = ADDR } }

#define IR_INSTR_BINARY(INSTR_TYPE, MODIFIABLE_FLAGS, TYPE, A, B, RESULT) (struct ir_instruction) { .instr_type = INSTR_TYPE, .binary_operation = { .modifiable_flags = MODIFIABLE_FLAGS, .type = TYPE, .a = A, .b = B, .result = RESULT } }
#define IR_INSTR_UNARY(INSTR_TYPE, TYPE, IN, OUT) (struct ir_instruction) { .instr_type = INSTR_TYPE, .unary_operation = { .type = TYPE, .input = IN, .output = OUT } }
#define IR_INSTR_INTRINSIC(INTRINSIC_ID) (struct ir_instruction) { .instr_type = IR_INSTR_INTRINSIC, .intrinsic = { .intrinsic_id = INTRINSIC_ID } }