#pragma once

#include <stdbool.h>
#include <stdint.h>

struct jvm_class_prepared_access_flags {
  bool is_public : 1;
  bool is_protected : 1;
  bool is_private : 1;
  bool is_static : 1;
  bool is_final : 1;
  bool is_super : 1;
  bool is_interface : 1;
  bool is_abstract : 1;
  bool is_synthetic : 1;
  bool is_annotation : 1;
  bool is_enum : 1;
  bool is_module : 1;
  bool is_volatile : 1;
  bool is_transient : 1;
  bool is_synchronized : 1;
  bool is_bridge : 1;
  bool is_varargs : 1;
  bool is_native : 1;
  bool is_strict : 1;
};

enum jvm_class_prepared_attribute_type {
  JVMCLASS_ATTRIBUTE_CODE
};

struct jvm_class_prepared_attribute {
  union {
    struct jvm_class_prepared_attribute_code {

    } code;
  } data;
  struct jvm_class_prepared_attribute* next;
  enum jvm_class_prepared_attribute_type type;
};

struct jvm_class_prepared_field {
  struct jvmclass_prepared_utf8_entry* name;
  struct jvmclass_prepared_utf8_entry* descriptor;
  struct jvm_class_prepared_attribute* attributes;
  struct jvm_class_prepared_access_flags access_flags;
};

struct jvm_class_prepared_method {
  struct jvmclass_prepared_utf8_entry* name;
  struct jvmclass_prepared_utf8_entry* descriptor;
  struct jvm_class_prepared_attribute* attributes;
  struct jvm_class_prepared_access_flags access_flags;
};

struct jvm_class_prepared_file {
  struct jvm_class_prepared_constant_pool_entry* constant_pool_entries;
  struct jvmclass_prepared_utf8_entry* this_class_name;
  struct jvmclass_prepared_utf8_entry* super_class_name;
  struct jvmclass_prepared_utf8_entry** interfaces;
  struct jvm_class_prepared_field* fields;
  struct jvm_class_prepared_method* methods;
  struct jvm_class_prepared_attribute* attributes;
  struct jvm_class_prepared_access_flags access_flags;
  uint16_t major_version;
  uint16_t constant_pool_count;
  uint16_t interface_count;
  uint16_t field_count;
  uint16_t method_count;
};


struct jvm_class_prepared_constant_pool_entry {
  union jvmclass_prepared_entry {
    struct jvmclass_prepared_utf8_entry {
      char* bytes;
      uint16_t length;
    } utf8;
    struct jvmclass_prepared_name_and_type_entry {
      struct jvmclass_prepared_utf8_entry* name;
      struct jvmclass_prepared_utf8_entry* descriptor;
    } name_and_type;
    struct jvmclass_prepared_method_handle_entry {
      struct jvm_class_prepared_constant_pool_entry* reference;
      uint8_t reference_kind;
    } method_handle;
    struct jvmclass_prepared_dynamic_entry {
      struct jvmclass_prepared_name_and_type_entry* name_and_type;
      uint16_t bootstrap_method_attr_index;
    } dynamic;
    struct jvmclass_prepared_ref_entry {
      struct jvmclass_prepared_utf8_entry* class_name;
      struct jvmclass_prepared_name_and_type_entry* name_and_type;
    } ref;
    struct jvmclass_prepared_index_entry { struct jvmclass_prepared_utf8_entry* entry; } index;
    struct jvmclass_prepared_int_entry { int num; } u4;
    struct jvmclass_prepared_float_entry { float num; } f4;
    struct jvmclass_prepared_long_entry { long num; } u8;
    struct jvmclass_prepared_double_entry { double num; } f8;
  } entry;
  uint8_t tag;
};