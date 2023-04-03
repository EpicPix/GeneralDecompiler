#pragma once
#include <stdint.h>

#define IR_REGISTER_IP 0xffff
#define IR_REGISTER_SP 0xfffe

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
}

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
  uint32_t displacement;
  uint8_t scale;
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
  uint64_t value;
};

struct ir_data_access_in {
  uint8_t data_type;
  union {
    struct ir_immediate imm;
    struct ir_register reg;
    struct ir_stack stack;
    struct ir_pointer pointer;
  };
};

struct ir_data_access_out {
  uint8_t data_type;
  union {
    struct ir_register reg;
    struct ir_stack stack;
    struct ir_pointer pointer;
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