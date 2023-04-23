#pragma once

#include <stdint.h>
#include <stdbool.h>

enum ir_type_definition_type {
  ir_type_definition_type_unknown = 0,
  ir_type_definition_type_int_unsigned = 1,
  ir_type_definition_type_int_signed = 2,
  ir_type_definition_type_float = 3,
}

struct ir_type {
  bool is_symbol : 1;
  union {
    uint32_t symbol_data;
    struct {
      uint16_t size;
      enum ir_type_definition_type type : 16;
    } pre_defined;
  };
};

static struct ir_type ir_type_unknown = { .is_symbol = false, .pre_defined = { .size = 0, .type = ir_type_definition_type_unknown } };
static struct ir_type ir_type_u8 = { .is_symbol = false, .pre_defined = { .size = 1, .type = ir_type_definition_type_int_unsigned } };
static struct ir_type ir_type_i8 = { .is_symbol = false, .pre_defined = { .size = 1, .type = ir_type_definition_type_int_signed } };
static struct ir_type ir_type_f8 = { .is_symbol = false, .pre_defined = { .size = 1, .type = ir_type_definition_type_float } };
static struct ir_type ir_type_u16 = { .is_symbol = false, .pre_defined = { .size = 2, .type = ir_type_definition_type_int_unsigned } };
static struct ir_type ir_type_i16 = { .is_symbol = false, .pre_defined = { .size = 2, .type = ir_type_definition_type_int_signed } };
static struct ir_type ir_type_f16 = { .is_symbol = false, .pre_defined = { .size = 2, .type = ir_type_definition_type_float } };
static struct ir_type ir_type_u32 = { .is_symbol = false, .pre_defined = { .size = 4, .type = ir_type_definition_type_int_unsigned } };
static struct ir_type ir_type_i32 = { .is_symbol = false, .pre_defined = { .size = 4, .type = ir_type_definition_type_int_signed } };
static struct ir_type ir_type_f32 = { .is_symbol = false, .pre_defined = { .size = 4, .type = ir_type_definition_type_float } };
static struct ir_type ir_type_u64 = { .is_symbol = false, .pre_defined = { .size = 8, .type = ir_type_definition_type_int_unsigned } };
static struct ir_type ir_type_i64 = { .is_symbol = false, .pre_defined = { .size = 8, .type = ir_type_definition_type_int_signed } };
static struct ir_type ir_type_f64 = { .is_symbol = false, .pre_defined = { .size = 8, .type = ir_type_definition_type_float } };

static bool ir_equal_types(struct ir_type a, struct ir_type b) {
  return a.is_symbol == b.is_symbol && a.symbol_data == b.symbol_data;
}