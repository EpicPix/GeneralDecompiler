#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "IR_type.h"

struct ir_instruction_high;


enum ir_instruction_high_location_type {
  ir_instruction_high_location_type_immediate,
  ir_instruction_high_location_type_register,
  ir_instruction_high_location_type_address,
  ir_instruction_high_location_type_register_address,
  ir_instruction_high_location_type_instruction,
  ir_instruction_high_location_type_inherited,
};

struct ir_instruction_high_location {
  ir_type_t type;
  enum ir_instruction_high_location_type location_type;
  union {
    uint64_t imm;
    uint64_t reg;
    uint64_t addr;
    struct ir_instruction_high* instr;
  } data;
};

enum ir_instruction_high_type {
  ir_instruction_high_type_add,
  ir_instruction_high_type_sub,
  ir_instruction_high_type_mul,
  ir_instruction_high_type_mov,
  ir_instruction_high_type_intrinsic,
  ir_instruction_high_type_intrinsic_typed,
};

enum ir_instruction_high_type_intrinsics {
    ir_instruction_high_type_intrinsic_unused,
};

struct ir_instruction_high {
  enum ir_instruction_high_type type;
  union {
    struct ir_instruction_high_data_oii {
      struct ir_instruction_high_location output;
      struct ir_instruction_high_location inputa;
      struct ir_instruction_high_location inputb;
    } oii;
    struct ir_instruction_high_data_oi {
      struct ir_instruction_high_location output;
      struct ir_instruction_high_location input;
    } oi;
    struct ir_instruction_high_data_i {
      struct ir_instruction_high_location input;
    } i;
    struct ir_instruction_high_data_o {
      struct ir_instruction_high_location output;
    } o;
    enum ir_instruction_high_type_intrinsics intrinsic;
    struct ir_instruction_high_data_intrinsic_typed {
        enum ir_instruction_high_type_intrinsics intrinsic;
        ir_type_t type;
    } intrinsic_typed;
  } data;
};

#define IR_HIGH_INSTR_OI(TYPE, OUTPUT, INPUT) (struct ir_instruction_high) { .type = TYPE, .data = { .oi = { OUTPUT, INPUT } } }
#define IR_HIGH_INSTR_OII(TYPE, OUTPUT, INPUTA, INPUTB) (struct ir_instruction_high) { .type = TYPE, .data = { .oii = { OUTPUT, INPUTA, INPUTB } } }
#define IR_HIGH_DATA_REG_ADDR(TYPE, REG) (struct ir_instruction_high_location) { .type = TYPE, .location_type = ir_instruction_high_location_type_register_address, .data = { .reg = REG } }
#define IR_HIGH_DATA_REG(TYPE, REG) (struct ir_instruction_high_location) { .type = TYPE, .location_type = ir_instruction_high_location_type_register, .data = { .reg = REG } }
#define IR_HIGH_DATA_IMM(TYPE, IMM) (struct ir_instruction_high_location) { .type = TYPE, .location_type = ir_instruction_high_location_type_immediate, .data = { .imm = IMM } }

struct ir_instruction_list* ir_instruction_add_instruction_high(struct ir_instruction_list* list, uint64_t instruction_count, struct ir_instruction_high instr);

struct ir_instruction_high* ir_instruction_alloc_high(struct ir_instruction_high data);