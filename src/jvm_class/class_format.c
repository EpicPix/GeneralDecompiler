#include <alloca.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../ir/IR.h"
#include "class_format.h"
#include "../byte_reader.h"
#include "class_format_loaded.h"
#include "class_format_prepared.h"

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

static struct jvm_class_prepared_attribute* arch_prepare_convert_attributes(struct jvm_class_prepared_file* cf, struct jvm_class_loaded_attribute* loaded_attributes, int loaded_attribute_count) {
  struct jvm_class_prepared_attribute* start_attr = NULL;
  struct jvm_class_prepared_attribute* current_attr = NULL;

  for(int i = 0; i<loaded_attribute_count; i++) {
    struct jvmclass_prepared_utf8_entry name = cf->constant_pool_entries[loaded_attributes[i].name_index - 1].entry.utf8;
    if(name.length == 4 && memcmp("Code", name.bytes, name.length) == 0) {
      struct jvm_class_prepared_attribute* code_attr = malloc(sizeof(struct jvm_class_prepared_attribute));
      code_attr->type = jvm_class_prepared_attribute_type_code;
      code_attr->next = NULL;
      void* loaded_bytes = loaded_attributes[i].bytes;
      uint32_t loaded_length = loaded_attributes[i].length;

      struct jvm_class_prepared_attribute_code* code = &code_attr->data.code;
      int read_index = 0;
      code->max_stack = read_word_be(loaded_bytes, &read_index, loaded_length);
      code->max_locals = read_word_be(loaded_bytes, &read_index, loaded_length);
      code->code_length = read_int_be(loaded_bytes, &read_index, loaded_length);
      if(read_index + code->code_length > loaded_length) {
        fprintf(stderr, "Cannot read %d bytes from index %d\n", code->code_length, read_index);
        exit(1);
      }
      code->code = malloc(code->code_length);
      memcpy(code->code, (char*) loaded_bytes + read_index, code->code_length);
      read_index += code->code_length;

      code->exception_table_length = read_word_be(loaded_bytes, &read_index, loaded_length);
      code->exception_table = malloc(sizeof(struct jvm_class_prepared_attribute_code_exception_table) * code->exception_table_length);
      for(int i = 0; i<code->exception_table_length; i++) {
        struct jvm_class_prepared_attribute_code_exception_table* table = &code->exception_table[i];
        table->start_pc = read_word_be(loaded_bytes, &read_index, loaded_length);
        table->end_pc = read_word_be(loaded_bytes, &read_index, loaded_length);
        table->handler_pc = read_word_be(loaded_bytes, &read_index, loaded_length);
        int catch_type = read_word_be(loaded_bytes, &read_index, loaded_length);
        if(catch_type == 0) table->catch_type = NULL;
        else table->catch_type = cf->constant_pool_entries[catch_type - 1].entry.index.entry;
      }

      int attribute_count = read_word_be(loaded_bytes, &read_index, loaded_length);
      struct jvm_class_loaded_attribute* loaded_attributes = arch_load_attributes(loaded_bytes, &read_index, loaded_length, attribute_count);
      code->attributes = arch_prepare_convert_attributes(cf, loaded_attributes, attribute_count);

      if(current_attr) current_attr->next = code_attr;
      current_attr = code_attr;
      if(start_attr == NULL) start_attr = code_attr;
    }else {
#ifdef DEBUG
      char* null_terminated_str = alloca(name.length + 1);
      memcpy(null_terminated_str, name.bytes, name.length);
      null_terminated_str[name.length] = '\0';
      ARCH_LOG("Unknown attribute: '%s', skipping", null_terminated_str);
#endif
    }
  }
  return start_attr;
}

static void* arch_prepare_data(void* loaded_data) {
  struct jvm_class_loaded_file* cfl = (struct jvm_class_loaded_file*) loaded_data;
  struct jvm_class_prepared_file* cf = malloc(sizeof(struct jvm_class_prepared_file));
  int constant_pool_count = cfl->constant_pool_count;
  cf->major_version = cfl->major_version;
  cf->constant_pool_count = constant_pool_count;
  cf->constant_pool_entries = malloc(sizeof(struct jvm_class_prepared_constant_pool_entry) * constant_pool_count);
  for(int i = 0; i<constant_pool_count; i++) {
    struct jvm_class_prepared_constant_pool_entry* entry = &cf->constant_pool_entries[i];
    entry->tag = cfl->constant_pool_entries[i].tag;
    switch(entry->tag) {
    case 1:
      entry->entry.utf8.length = cfl->constant_pool_entries[i].entry.utf8.length;
      entry->entry.utf8.bytes = malloc(entry->entry.utf8.length + 1);
      memcpy(entry->entry.utf8.bytes, cfl->constant_pool_entries[i].entry.utf8.bytes, entry->entry.utf8.length);
      entry->entry.utf8.bytes[entry->entry.utf8.length] = '\0';
      break;
    case 3:
      entry->entry.u4.num = cfl->constant_pool_entries[i].entry.u4.num;
      break;
    case 4:
      entry->entry.f4.num = cfl->constant_pool_entries[i].entry.f4.num;
      break;
    case 5:
      entry->entry.u8.num = cfl->constant_pool_entries[i].entry.u8.num;
      i++;
      break;
    case 6:
      entry->entry.f8.num = cfl->constant_pool_entries[i].entry.f8.num;
      i++;
      break;
    case 7:
    case 8:
    {
      int index_loc = cfl->constant_pool_entries[i].entry.index.index - 1;
      if(cfl->constant_pool_entries[index_loc].tag != 1) {
        ARCH_LOG("Failed check for tag %d, tag: %d expected %d.", entry->tag, cfl->constant_pool_entries[index_loc].tag, 1);
        fprintf(stderr, "Invalid tag\n");
        exit(1);
      }
      entry->entry.index.entry = &cf->constant_pool_entries[index_loc].entry.utf8;
      break;
    }
    case 16:
    case 19:
    case 20:
    {
      int index_loc = cfl->constant_pool_entries[i].entry.index.index - 1;
      if(cfl->constant_pool_entries[index_loc].tag != 1) {
        ARCH_LOG("Failed check for tag %d, tag: %d expected %d.", entry->tag, cfl->constant_pool_entries[index_loc].tag, 1);
        fprintf(stderr, "Invalid tag\n");
        exit(1);
      }
      entry->entry.index.entry = &cf->constant_pool_entries[index_loc].entry.utf8;
      break;
    }
    case 9:
    case 10:
    case 11:
    {
      int class_index_loc = cfl->constant_pool_entries[i].entry.ref.class_index - 1;
      if(cfl->constant_pool_entries[class_index_loc].tag != 7) {
        ARCH_LOG("Failed check for tag %d, tag: %d expected %d.", entry->tag, cfl->constant_pool_entries[class_index_loc].tag, 7);
        fprintf(stderr, "Invalid tag\n");
        exit(1);
      }

      int class_name_loc = cfl->constant_pool_entries[class_index_loc].entry.index.index - 1;
      int name_and_type_loc = cfl->constant_pool_entries[i].entry.ref.name_and_type_index - 1;
      if(cfl->constant_pool_entries[class_name_loc].tag != 1 || cfl->constant_pool_entries[name_and_type_loc].tag != 12) {
        ARCH_LOG("Failed check for tag %d, tag: %d,%d expected %d,%d.", entry->tag, cfl->constant_pool_entries[class_name_loc].tag, cfl->constant_pool_entries[name_and_type_loc].tag, 1, 12);
        fprintf(stderr, "Invalid tag\n");
        exit(1);
      }
      entry->entry.ref.class_name = &cf->constant_pool_entries[class_name_loc].entry.utf8;
      entry->entry.ref.name_and_type = &cf->constant_pool_entries[name_and_type_loc].entry.name_and_type;
      break;
    }
    case 12:
    {
      int name_loc = cfl->constant_pool_entries[i].entry.name_and_type.name_index - 1;
      int descriptor_loc = cfl->constant_pool_entries[i].entry.name_and_type.descriptor_index - 1;
      if(cfl->constant_pool_entries[name_loc].tag != 1 || cfl->constant_pool_entries[descriptor_loc].tag != 1) {
        ARCH_LOG("Failed check for tag %d, tag: %d,%d expected %d,%d.", entry->tag, cfl->constant_pool_entries[name_loc].tag, cfl->constant_pool_entries[descriptor_loc].tag, 1, 1);
        fprintf(stderr, "Invalid tag\n");
        exit(1);
      }
      entry->entry.name_and_type.name = &cf->constant_pool_entries[name_loc].entry.utf8;
      entry->entry.name_and_type.descriptor = &cf->constant_pool_entries[descriptor_loc].entry.utf8;
      break;
    }
    case 15:
      entry->entry.method_handle.reference_kind = cfl->constant_pool_entries[i].entry.method_handle.reference_kind;
      entry->entry.method_handle.reference = &cf->constant_pool_entries[cfl->constant_pool_entries[i].entry.method_handle.reference_index - 1];
      break;
    case 17:
    case 18:
    {
      int name_and_type_loc = cfl->constant_pool_entries[i].entry.dynamic.name_and_type_index - 1;
      if(cfl->constant_pool_entries[name_and_type_loc].tag != 12) {
        ARCH_LOG("Failed check for tag %d, tag: %d expected %d.", entry->tag, cfl->constant_pool_entries[name_and_type_loc].tag, 12);
        fprintf(stderr, "Invalid tag\n");
        exit(1);
      }
      entry->entry.dynamic.bootstrap_method_attr_index = cfl->constant_pool_entries[i].entry.dynamic.bootstrap_method_attr_index;
      entry->entry.dynamic.name_and_type = &cf->constant_pool_entries[name_and_type_loc].entry.name_and_type;
      break;
    }
    }
  }

  memset(&cf->access_flags, 0, sizeof(cf->access_flags));
  cf->access_flags.is_public = (cfl->access_flags & 0x0001) != 0;
  cf->access_flags.is_final = (cfl->access_flags & 0x0010) != 0;
  cf->access_flags.is_super = (cfl->access_flags & 0x0020) != 0;
  cf->access_flags.is_interface = (cfl->access_flags & 0x0200) != 0;
  cf->access_flags.is_abstract = (cfl->access_flags & 0x0400) != 0;
  cf->access_flags.is_synchronized = (cfl->access_flags & 0x1000) != 0;
  cf->access_flags.is_annotation = (cfl->access_flags & 0x2000) != 0;
  cf->access_flags.is_enum = (cfl->access_flags & 0x4000) != 0;
  cf->access_flags.is_module = (cfl->access_flags & 0x8000) != 0;

  cf->this_class_name = cf->constant_pool_entries[cfl->this_class - 1].entry.index.entry;
  cf->super_class_name = cfl->super_class == 0 ? NULL : cf->constant_pool_entries[cfl->super_class - 1].entry.index.entry;

  cf->interface_count = cfl->interface_count;
  cf->interfaces = malloc(sizeof(struct jvmclass_prepared_utf8_entry*) * cf->interface_count);
  for(int i = 0; i<cf->interface_count; i++) {
    cf->interfaces[i] = cf->constant_pool_entries[cfl->interfaces[i] - 1].entry.index.entry;
  }

  cf->field_count = cfl->field_count;
  cf->fields = malloc(sizeof(struct jvm_class_prepared_field) * cf->field_count);
  for(int i = 0; i<cf->field_count; i++) {
    struct jvm_class_loaded_field* lfield = &cfl->fields[i];
    struct jvm_class_prepared_field* field = &cf->fields[i];

    field->name = &cf->constant_pool_entries[lfield->name_index - 1].entry.utf8;
    field->descriptor = &cf->constant_pool_entries[lfield->descriptor_index - 1].entry.utf8;
    memset(&field->access_flags, 0, sizeof(field->access_flags));
    field->access_flags.is_public = (lfield->access_flags & 0x0001) != 0;
    field->access_flags.is_private = (lfield->access_flags & 0x0002) != 0;
    field->access_flags.is_protected = (lfield->access_flags & 0x0004) != 0;
    field->access_flags.is_static = (lfield->access_flags & 0x0008) != 0;
    field->access_flags.is_final = (lfield->access_flags & 0x0010) != 0;
    field->access_flags.is_volatile = (lfield->access_flags & 0x0040) != 0;
    field->access_flags.is_transient = (lfield->access_flags & 0x0080) != 0;
    field->access_flags.is_synthetic = (lfield->access_flags & 0x1000) != 0;
    field->access_flags.is_enum = (lfield->access_flags & 0x4000) != 0;

    field->attributes = arch_prepare_convert_attributes(cf, lfield->attributes, lfield->attribute_count);
  }

  cf->method_count = cfl->method_count;
  cf->methods = malloc(sizeof(struct jvm_class_prepared_method) * cf->method_count);
  for(int i = 0; i<cf->method_count; i++) {
    struct jvm_class_loaded_method* lmethod = &cfl->methods[i];
    struct jvm_class_prepared_method* method = &cf->methods[i];

    method->name = &cf->constant_pool_entries[lmethod->name_index - 1].entry.utf8;
    method->descriptor = &cf->constant_pool_entries[lmethod->descriptor_index - 1].entry.utf8;
    memset(&method->access_flags, 0, sizeof(lmethod->access_flags));
    method->access_flags.is_public = (lmethod->access_flags & 0x0001) != 0;
    method->access_flags.is_private = (lmethod->access_flags & 0x0002) != 0;
    method->access_flags.is_protected = (lmethod->access_flags & 0x0004) != 0;
    method->access_flags.is_static = (lmethod->access_flags & 0x0008) != 0;
    method->access_flags.is_final = (lmethod->access_flags & 0x0010) != 0;
    method->access_flags.is_synchronized = (lmethod->access_flags & 0x0020) != 0;
    method->access_flags.is_bridge = (lmethod->access_flags & 0x0040) != 0;
    method->access_flags.is_varargs = (lmethod->access_flags & 0x0080) != 0;
    method->access_flags.is_native = (lmethod->access_flags & 0x0100) != 0;
    method->access_flags.is_abstract = (lmethod->access_flags & 0x0100) != 0;
    method->access_flags.is_strict = (lmethod->access_flags & 0x0800) != 0;
    method->access_flags.is_synthetic = (lmethod->access_flags & 0x1000) != 0;

    method->attributes = arch_prepare_convert_attributes(cf, lmethod->attributes, lmethod->attribute_count);
  }

  cf->attributes = arch_prepare_convert_attributes(cf, cfl->attributes, cfl->attribute_count);

  return cf;
}

static struct ir_data arch_generate_ir(void* prepared_data) {
  struct jvm_class_prepared_file* cf = (struct jvm_class_prepared_file*) prepared_data;
  struct ir_symbol_table* symbol_table = ir_symbol_create_table(NULL);
  struct ir_type_table* type_table = ir_type_create_table(NULL);

  struct ir_instruction_list* instructions_start = ir_instruction_create_list(NULL, 0x10000, 1024, true);

  for(int i = 0; i<cf->method_count; i++) {
    struct jvm_class_prepared_method* method = &cf->methods[i];
    ARCH_LOG("Generating IR for method '%s' with descriptor '%s'", method->name->bytes, method->descriptor->bytes);
    if(method->access_flags.is_abstract) {
      ARCH_LOG("%s", "Method skipped, method is defined as abstract");
      continue;
    }
    struct jvm_class_prepared_attribute_code* code = NULL;
    struct jvm_class_prepared_attribute* iter_attr = method->attributes;
    while(iter_attr != NULL) {
      if(iter_attr->type == jvm_class_prepared_attribute_type_code) {
        code = &iter_attr->data.code;
        break;
      }
      iter_attr = iter_attr->next;
    }
    if(code == NULL) {
      ARCH_LOG("%s", "Method skipped, method is missing Code attribute");
      continue;
    }
    for(uint32_t x = 0; x<code->code_length; x++) {
      uint8_t opcode = code->code[x];
      if(opcode == 0x04) { // iconst_1
        instructions_start = ir_instruction_add_instruction_high(instructions_start, 1024, (struct ir_instruction_high) { .type = ir_instruction_high_type_push, .data = { .i = { .input = { .type = ir_type_i32, .location_type = ir_instruction_high_location_type_immediate, .data = { .imm = 1 } } } } });
        continue;
      }
      ARCH_LOG("Unknown opcode 0x%02x, stopping generation", opcode);
      break;
    }
  }

  return (struct ir_data){ .symbol_table = symbol_table, .type_table = type_table, .memory_page_start = NULL, .instructions = instructions_start, .is_high_level = true };
}

const arch_info arch_jvm = (const arch_info){
    .load_data = arch_load_data,
    .prepare_data = arch_prepare_data,
    .generate_ir = arch_generate_ir,
};