#pragma once

#include <stdint.h>
#include <stdbool.h>

struct ir_type {
  bool is_symbol : 1;
  union {
    uint32_t symbol_data;
    struct {
      uint16_t size;
      uint16_t type;
    } pre_defined;
  };
};

#define IR_TYPE_TYPE_UNKNOWN 0
#define IR_TYPE_TYPE_INT_UNSIGNED 1
#define IR_TYPE_TYPE_INT_SIGNED 2
#define IR_TYPE_TYPE_FLOAT 3

static struct ir_type ir_type_unknown = { .is_symbol = false, .pre_defined = { .size = 0, .type = IR_TYPE_TYPE_UNKNOWN } };
static struct ir_type ir_type_u8 = { .is_symbol = false, .pre_defined = { .size = 1, .type = IR_TYPE_TYPE_INT_UNSIGNED } };
static struct ir_type ir_type_i8 = { .is_symbol = false, .pre_defined = { .size = 1, .type = IR_TYPE_TYPE_INT_SIGNED } };
static struct ir_type ir_type_f8 = { .is_symbol = false, .pre_defined = { .size = 1, .type = IR_TYPE_TYPE_FLOAT } };
static struct ir_type ir_type_u16 = { .is_symbol = false, .pre_defined = { .size = 2, .type = IR_TYPE_TYPE_INT_UNSIGNED } };
static struct ir_type ir_type_i16 = { .is_symbol = false, .pre_defined = { .size = 2, .type = IR_TYPE_TYPE_INT_SIGNED } };
static struct ir_type ir_type_f16 = { .is_symbol = false, .pre_defined = { .size = 2, .type = IR_TYPE_TYPE_FLOAT } };
static struct ir_type ir_type_u32 = { .is_symbol = false, .pre_defined = { .size = 4, .type = IR_TYPE_TYPE_INT_UNSIGNED } };
static struct ir_type ir_type_i32 = { .is_symbol = false, .pre_defined = { .size = 4, .type = IR_TYPE_TYPE_INT_SIGNED } };
static struct ir_type ir_type_f32 = { .is_symbol = false, .pre_defined = { .size = 4, .type = IR_TYPE_TYPE_FLOAT } };
static struct ir_type ir_type_u64 = { .is_symbol = false, .pre_defined = { .size = 8, .type = IR_TYPE_TYPE_INT_UNSIGNED } };
static struct ir_type ir_type_i64 = { .is_symbol = false, .pre_defined = { .size = 8, .type = IR_TYPE_TYPE_INT_SIGNED } };
static struct ir_type ir_type_f64 = { .is_symbol = false, .pre_defined = { .size = 8, .type = IR_TYPE_TYPE_FLOAT } };

static bool ir_equal_types(struct ir_type a, struct ir_type b) {
  return a.is_symbol == b.is_symbol && a.symbol_data == b.symbol_data;
}