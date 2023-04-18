#pragma once

#include <stdint.h>

struct jvm_class_loaded_attribute {
  uint16_t name_index;
  uint32_t length;
  void* bytes;
};

struct jvm_class_loaded_field {
  uint16_t access_flags;
  uint16_t name_index;
  uint16_t descriptor_index;
  uint16_t attribute_count;
  struct jvm_class_loaded_attribute* attributes;
};

struct jvm_class_loaded_method {
  uint16_t access_flags;
  uint16_t name_index;
  uint16_t descriptor_index;
  uint16_t attribute_count;
  struct jvm_class_loaded_attribute* attributes;
};

struct jvm_class_loaded_file {
  uint16_t minor_version;
  uint16_t major_version;
  int constant_pool_count;
  struct jvm_class_loaded_constant_pool_entry* constant_pool_entries;
  uint16_t access_flags;
  uint16_t this_class;
  uint16_t super_class;
  uint16_t interface_count;
  uint16_t* interfaces;
  uint16_t field_count;
  struct jvm_class_loaded_field* fields;
  uint16_t method_count;
  struct jvm_class_loaded_method* methods;
  uint16_t attribute_count;
  struct jvm_class_loaded_attribute* attributes;
};


struct jvm_class_loaded_constant_pool_entry {
  union jvmclass_loaded_entry {
    struct jvmclass_loaded_utf8_entry {
      char* bytes;
      uint16_t length;
    } __attribute__((packed, aligned(1))) utf8;
    struct jvmclass_loaded_name_and_type_entry {
      uint16_t name_index;
      uint16_t descriptor_index;
    } name_and_type;
    struct jvmclass_loaded_method_handle_entry {
      uint16_t reference_index;
      uint8_t reference_kind;
    } method_handle;
    struct jvmclass_loaded_dynamic_entry {
      uint16_t bootstrap_method_attr_index;
      uint16_t name_and_type_index;
    } dynamic;
    struct jvmclass_loaded_ref_entry {
      uint16_t class_index;
      uint16_t name_and_type_index;
    } ref;
    struct jvmclass_loaded_index_entry { uint16_t index; } index;
    struct jvmclass_loaded_int_entry { int num; } u4;
    struct jvmclass_loaded_float_entry { float num; } f4;
    struct jvmclass_loaded_long_entry { long num; } u8;
    struct jvmclass_loaded_double_entry { double num; } f8;
  } __attribute__((packed, aligned(1))) entry;
  uint8_t tag;
};