#include <stdlib.h>
#include "class_format.h"

static void* arch_load_data(uint8_t* code, int code_length) {
  return NULL;
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