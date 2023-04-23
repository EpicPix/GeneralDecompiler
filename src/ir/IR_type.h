#pragma once

#include <stdint.h>
#include <stdbool.h>

enum ir_type_definition_type {
  ir_type_definition_type_unknown = 0,
  ir_type_definition_type_int_unsigned = 1,
  ir_type_definition_type_int_signed = 2,
  ir_type_definition_type_float = 3,
};

struct ir_type {
  bool is_builtin : 1;
  union {
    uint32_t composed_type_index;
    struct {
      uint16_t size;
      enum ir_type_definition_type type : 16;
    } built_in;
  };
};

enum ir_type_composed_type {
  ir_type_composed_type_struct = 0,
  ir_type_composed_type_union = 1,
};

struct ir_type_composed {
  enum ir_type_composed_type composed_type;
  int type_count;
  struct ir_type contained_types[];
};

struct ir_type_table {
  int start_index;
  int entry_count;
  int allocated_count;
  struct ir_type_composed** types;
  struct ir_type_table* next;
};

static struct ir_type ir_type_unknown = { .is_builtin = true, .built_in = { .size = 0, .type = ir_type_definition_type_unknown } };
static struct ir_type ir_type_u8 = { .is_builtin = true, .built_in = { .size = 8, .type = ir_type_definition_type_int_unsigned } };
static struct ir_type ir_type_i8 = { .is_builtin = true, .built_in = { .size = 8, .type = ir_type_definition_type_int_signed } };
static struct ir_type ir_type_f8 = { .is_builtin = true, .built_in = { .size = 8, .type = ir_type_definition_type_float } };
static struct ir_type ir_type_u16 = { .is_builtin = true, .built_in = { .size = 16, .type = ir_type_definition_type_int_unsigned } };
static struct ir_type ir_type_i16 = { .is_builtin = true, .built_in = { .size = 16, .type = ir_type_definition_type_int_signed } };
static struct ir_type ir_type_f16 = { .is_builtin = true, .built_in = { .size = 16, .type = ir_type_definition_type_float } };
static struct ir_type ir_type_u32 = { .is_builtin = true, .built_in = { .size = 32, .type = ir_type_definition_type_int_unsigned } };
static struct ir_type ir_type_i32 = { .is_builtin = true, .built_in = { .size = 32, .type = ir_type_definition_type_int_signed } };
static struct ir_type ir_type_f32 = { .is_builtin = true, .built_in = { .size = 32, .type = ir_type_definition_type_float } };
static struct ir_type ir_type_u64 = { .is_builtin = true, .built_in = { .size = 64, .type = ir_type_definition_type_int_unsigned } };
static struct ir_type ir_type_i64 = { .is_builtin = true, .built_in = { .size = 64, .type = ir_type_definition_type_int_signed } };
static struct ir_type ir_type_f64 = { .is_builtin = true, .built_in = { .size = 64, .type = ir_type_definition_type_float } };

static bool ir_equal_types(struct ir_type a, struct ir_type b) {
  return a.is_builtin == b.is_builtin && a.composed_type_index == b.composed_type_index;
}