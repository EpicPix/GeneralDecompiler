#pragma once
#include <stdint.h>

#define IR_REGISTER_IP 0xffff
#define IR_REGISTER_SP 0xfffe

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

struct ir_data_access_in {
  uint8_t data_type;
  union {
    struct ir_immediate imm;
    struct ir_register reg;
    struct ir_stack stack;
  };
};

struct ir_data_access_out {
  uint8_t data_type;
  union {
    struct ir_register reg;
    struct ir_stack stack;
  };
};

struct ir_add {
  uint16_t modifiable_flags;
  struct ir_type type;
  struct ir_data_access_in a;
  struct ir_data_access_in b;
  struct ir_data_access_out result;
};

struct ir_instruction {
  uint16_t instr_type;
  union {
    ir_add add;
  };
};