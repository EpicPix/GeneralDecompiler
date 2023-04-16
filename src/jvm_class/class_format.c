#include <stdlib.h>
#include <stdio.h>
#include "class_format.h"
#include "../byte_reader.h"

static void* arch_load_data(uint8_t* code, int code_length) {
  int index = 0;
  if(read_int_be(code, &index, code_length) != 0xCAFEBABE) {
    fprintf(stderr, "Invalid java magic number\n");
    exit(1);
  }
  struct jvm_class_loaded_file* cf = malloc(sizeof(struct jvm_class_loaded_file));
  cf->minor_version = read_word_be(code, &index, code_length);
  cf->major_version = read_word_be(code, &index, code_length);
  ARCH_LOG("Class file version: %d.%d", cf->major_version, cf->minor_version);
  return cf;
}

static struct arch_prepared_data arch_prepare_data(void* loaded_data) {
  return (struct arch_prepared_data) { .prepared_data = NULL, .details_data = NULL };
}

static struct ir_data* arch_generate_ir(void* prepared_data) {
  return NULL;
}

const arch_info arch_jvm = (const arch_info){
    .load_data = arch_load_data,
    .prepare_data = arch_prepare_data,
    .generate_ir = arch_generate_ir,
};