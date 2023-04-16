#pragma once

#include <stdint.h>
#include "../arch/arch.h"
#include "../utils.h"

struct jvm_class_loaded_file {
  uint16_t minor_version;
  uint16_t major_version;
  int constant_pool_count;
  struct jvm_class_loaded_constant_pool_entry* constant_pool_entries;
};

struct jvmclass_utf8_entry {
  char* bytes;
  uint16_t length;
} __attribute__((packed, aligned(1)));
struct jvmclass_name_and_type_entry {
  uint16_t name_index;
  uint16_t descriptor_index;
};
struct jvmclass_method_handle_entry {
  uint16_t reference_index;
  uint8_t reference_kind;
};
struct jvmclass_dynamic_entry {
  uint16_t bootstrap_method_attr_index;
  uint16_t name_and_type_index;
};
struct jvmclass_ref_entry {
  uint16_t class_index;
  uint16_t name_and_type_index;
};
struct jvmclass_index_entry { uint16_t index; };
struct jvmclass_int_entry { int num; };
struct jvmclass_float_entry { float num; };
struct jvmclass_long_entry { long num; };
struct jvmclass_double_entry { double num; };

union jvmclass_entry {
  struct jvmclass_utf8_entry utf8;
  struct jvmclass_name_and_type_entry name_and_type;
  struct jvmclass_method_handle_entry method_handle;
  struct jvmclass_dynamic_entry dynamic;
  struct jvmclass_ref_entry ref;
  struct jvmclass_index_entry index;
  struct jvmclass_int_entry u4;
  struct jvmclass_float_entry f4;
  struct jvmclass_long_entry u8;
  struct jvmclass_double_entry f8;
} __attribute__((packed, aligned(1)));

struct jvm_class_loaded_constant_pool_entry {
  union jvmclass_entry entry;
  uint8_t tag;
};

#define ARCH_LOG(format, ...) DEBUG_LOG("[jvmclass] " format, __VA_ARGS__)