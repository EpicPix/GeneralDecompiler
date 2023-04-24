#include "IR.h"
#include <stdlib.h>

struct ir_data ir_optimize(struct ir_data data) {
  return data;
}

void ir_print_instructions(struct ir_data data) {

}

void ir_print_decompiled(struct ir_data data) {

}



struct ir_type_table* ir_type_create_table(struct ir_type_table* prev) {
  int start = prev ? (prev->start_index + prev->allocated_count) : 0;
  int alloc_count = 256;

  struct ir_type_table* type_table = calloc(sizeof(struct ir_type_table) + alloc_count * sizeof(struct ir_type_composed*), 1);
  type_table->allocated_count = alloc_count;
  if(prev) prev->next = type_table;
  return type_table;
}