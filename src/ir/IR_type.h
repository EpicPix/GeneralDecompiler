#pragma once

#include <stdint.h>
#include <stdbool.h>

enum ir_type_definition_type {
  ir_type_definition_type_void = 0,
  ir_type_definition_type_int_unsigned = 1,
  ir_type_definition_type_int_signed = 2,
  ir_type_definition_type_float = 3,
};

typedef struct ir_type {
  bool is_builtin : 1;
  union {
    uint32_t composed_type_index;
    struct {
      uint16_t size;
      enum ir_type_definition_type type : 16;
    } built_in;
  };
} ir_type_t;

enum ir_type_composed_type {
  ir_type_composed_type_struct = 0,
  ir_type_composed_type_union = 1,
};

struct ir_type_composed {
  enum ir_type_composed_type composed_type;
  int type_count;
  ir_type_t contained_types[];
};

struct ir_type_table {
  uint32_t start_index;
  uint32_t entry_count;
  int allocated_count;
  struct ir_type_table* next;
  struct ir_type_composed* types[];
};

static ir_type_t ir_type_void = { .is_builtin = true, .built_in = { .size = 0, .type = ir_type_definition_type_void } };
static ir_type_t ir_type_u8 = { .is_builtin = true, .built_in = { .size = 8, .type = ir_type_definition_type_int_unsigned } };
static ir_type_t ir_type_s8 = { .is_builtin = true, .built_in = { .size = 8, .type = ir_type_definition_type_int_signed } };
static ir_type_t ir_type_f8 = { .is_builtin = true, .built_in = { .size = 8, .type = ir_type_definition_type_float } };
static ir_type_t ir_type_u16 = { .is_builtin = true, .built_in = { .size = 16, .type = ir_type_definition_type_int_unsigned } };
static ir_type_t ir_type_s16 = { .is_builtin = true, .built_in = { .size = 16, .type = ir_type_definition_type_int_signed } };
static ir_type_t ir_type_f16 = { .is_builtin = true, .built_in = { .size = 16, .type = ir_type_definition_type_float } };
static ir_type_t ir_type_u32 = { .is_builtin = true, .built_in = { .size = 32, .type = ir_type_definition_type_int_unsigned } };
static ir_type_t ir_type_s32 = { .is_builtin = true, .built_in = { .size = 32, .type = ir_type_definition_type_int_signed } };
static ir_type_t ir_type_f32 = { .is_builtin = true, .built_in = { .size = 32, .type = ir_type_definition_type_float } };
static ir_type_t ir_type_u64 = { .is_builtin = true, .built_in = { .size = 64, .type = ir_type_definition_type_int_unsigned } };
static ir_type_t ir_type_s64 = { .is_builtin = true, .built_in = { .size = 64, .type = ir_type_definition_type_int_signed } };
static ir_type_t ir_type_f64 = { .is_builtin = true, .built_in = { .size = 64, .type = ir_type_definition_type_float } };

static bool ir_equal_types(ir_type_t a, ir_type_t b) {
  return a.is_builtin == b.is_builtin && a.composed_type_index == b.composed_type_index;
}

struct ir_type_composed* ir_type_get_composed(uint32_t composed_type_id, struct ir_type_table* type_table);
uint64_t ir_type_bit_size(ir_type_t type, struct ir_type_table* table);

struct ir_type_table* ir_type_create_table(struct ir_type_table* prev);