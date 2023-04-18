#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "class_format.h"
#include "../byte_reader.h"
#include "class_format_loaded.h"

static struct jvm_class_loaded_attribute* arch_load_attributes(uint8_t* code, int* index, uint32_t code_length, int attribute_count) {
  struct jvm_class_loaded_attribute* attributes = malloc(sizeof(struct jvm_class_loaded_attribute) * attribute_count);
  for(int i = 0; i<attribute_count; i++) {
    attributes[i].name_index = read_word_be(code, index, code_length);
    attributes[i].length = read_int_be(code, index, code_length);
    if(*index + attributes[i].length > code_length) {
      fprintf(stderr, "Cannot read %d bytes from index %d\n", attributes[i].length, *index);
      exit(1);
    }
    void* data = malloc(attributes[i].length);
    memcpy(data, code + *index, attributes[i].length);
    attributes[i].bytes = data;
    *index += attributes[i].length;
  }
  return attributes;
}

static void* arch_load_data(uint8_t* code, int code_length) {
  int index = 0;
  if(read_int_be(code, &index, code_length) != 0xCAFEBABE) {
    fprintf(stderr, "Invalid class magic number\n");
    exit(1);
  }
  struct jvm_class_loaded_file* cf = malloc(sizeof(struct jvm_class_loaded_file));
  cf->minor_version = read_word_be(code, &index, code_length);
  cf->major_version = read_word_be(code, &index, code_length);
  ARCH_LOG("Class file version: %d.%d", cf->major_version, cf->minor_version);

  cf->constant_pool_count = read_word_be(code, &index, code_length) - 1;
  ARCH_LOG("Constant pool entry count: %d", cf->constant_pool_count);

  cf->constant_pool_entries = malloc(cf->constant_pool_count * sizeof(struct jvm_class_loaded_constant_pool_entry));
  for(int i = 0; i<cf->constant_pool_count; i++) {
    struct jvm_class_loaded_constant_pool_entry* entry = &cf->constant_pool_entries[i];
    entry->tag = read_byte(code, &index, code_length);
    switch(entry->tag) {
    case 1:
      entry->entry.utf8.length = read_word_be(code, &index, code_length);
      entry->entry.utf8.bytes = malloc(entry->entry.utf8.length);
      if(index + entry->entry.utf8.length > code_length) {
        fprintf(stderr, "Cannot read %d bytes from index %d\n", entry->entry.utf8.length, index);
        exit(1);
      }
      memcpy(entry->entry.utf8.bytes, code + index, entry->entry.utf8.length);
      index += entry->entry.utf8.length;
      break;
    case 3:
      entry->entry.u4.num = read_int_be(code, &index, code_length);
      break;
    case 4:
      entry->entry.f4.num = read_float_be(code, &index, code_length);
      break;
    case 5:
      entry->entry.u8.num = read_long_be(code, &index, code_length);
      i++;
      break;
    case 6:
      entry->entry.f8.num = read_double_be(code, &index, code_length);
      i++;
      break;
    case 7:
    case 8:
    case 16:
    case 19:
    case 20:
      entry->entry.index.index = read_word_be(code, &index, code_length);
      break;
    case 9:
    case 10:
    case 11:
      entry->entry.ref.class_index = read_word_be(code, &index, code_length);
      entry->entry.ref.name_and_type_index = read_word_be(code, &index, code_length);
      break;
    case 12:
      entry->entry.name_and_type.name_index = read_word_be(code, &index, code_length);
      entry->entry.name_and_type.descriptor_index = read_word_be(code, &index, code_length);
      break;
    case 15:
      entry->entry.method_handle.reference_kind = read_byte(code, &index, code_length);
      entry->entry.method_handle.reference_index = read_word_be(code, &index, code_length);
      break;
    case 17:
    case 18:
      entry->entry.dynamic.bootstrap_method_attr_index = read_word_be(code, &index, code_length);
      entry->entry.dynamic.name_and_type_index = read_word_be(code, &index, code_length);
      break;
    default:
      fprintf(stderr, "Unrecognized constant pool entry tag: %d\n", entry->tag);
      exit(1);
    }
  }

  cf->access_flags = read_word_be(code, &index, code_length);
  cf->this_class = read_word_be(code, &index, code_length);
  cf->super_class = read_word_be(code, &index, code_length);

  cf->interface_count = read_word_be(code, &index, code_length);
  cf->interfaces = malloc(cf->interface_count * sizeof(uint16_t));
  ARCH_LOG("Interface count: %d", cf->interface_count);
  for(int i = 0; i<cf->interface_count; i++) {
    cf->interfaces[i] = read_word_be(code, &index, code_length);
  }

  cf->field_count = read_word_be(code, &index, code_length);
  ARCH_LOG("Field count: %d", cf->field_count);
  cf->fields = malloc(cf->field_count * sizeof(struct jvm_class_loaded_field));
  for(int i = 0; i<cf->field_count; i++) {
    struct jvm_class_loaded_field* field = &cf->fields[i];
    field->access_flags = read_word_be(code, &index, code_length);
    field->name_index = read_word_be(code, &index, code_length);
    field->descriptor_index = read_word_be(code, &index, code_length);
    field->attribute_count = read_word_be(code, &index, code_length);
    field->attributes = arch_load_attributes(code, &index, code_length, field->attribute_count);
  }

  cf->method_count = read_word_be(code, &index, code_length);
  ARCH_LOG("Method count: %d", cf->method_count);
  cf->methods = malloc(cf->method_count * sizeof(struct jvm_class_loaded_method));
  for(int i = 0; i<cf->method_count; i++) {
    struct jvm_class_loaded_method* method = &cf->methods[i];
    method->access_flags = read_word_be(code, &index, code_length);
    method->name_index = read_word_be(code, &index, code_length);
    method->descriptor_index = read_word_be(code, &index, code_length);
    method->attribute_count = read_word_be(code, &index, code_length);
    method->attributes = arch_load_attributes(code, &index, code_length, method->attribute_count);
  }

  cf->attribute_count = read_word_be(code, &index, code_length);
  ARCH_LOG("Attribute count: %d", cf->attribute_count);
  cf->attributes = arch_load_attributes(code, &index, code_length, cf->attribute_count);

  return cf;
}

static void* arch_prepare_data(void* loaded_data) {
  return NULL;
}

static struct ir_data* arch_generate_ir(void* prepared_data) {
  return NULL;
}

const arch_info arch_jvm = (const arch_info){
    .load_data = arch_load_data,
    .prepare_data = arch_prepare_data,
    .generate_ir = arch_generate_ir,
};