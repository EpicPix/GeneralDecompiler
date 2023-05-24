#include "x86_64.h"

struct arch_data {
    uint8_t* bytes;
    int byte_count;
};

static void* arch_load_data(uint8_t* bytes, int byte_count) {
  struct arch_data* ad = malloc(sizeof(struct arch_data));
  ad->bytes = bytes;
  ad->byte_count = byte_count;
  return ad;
}

static void* arch_prepare_data(void* loaded_data) {
  return loaded_data;
}

static struct ir_data arch_generate_ir(void* prepared_data) {
  struct arch_data* data = prepared_data;
  struct ir_instruction_list* instructions = ir_instruction_create_list(NULL, 0x10000, 1024, ir_instruction_level_high);

  return (struct ir_data) {
    .instruction_level = ir_instruction_level_high,
    .instructions = instructions,
    .symbol_table = ir_symbol_create_table(NULL),
    .type_table = NULL,
    .memory_page_start = NULL
  };
}

const arch_info arch_x86_64 = (const arch_info){
  .load_data = arch_load_data,
  .prepare_data = arch_prepare_data,
  .generate_ir = arch_generate_ir,
};